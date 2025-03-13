#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <secp256k1.h>
#include <fstream>
#include <cassert>
#include "lib/uint256.h"
#include "config.h"
#include "colors.h"

using namespace std;
using json = nlohmann::json;

std::string SEED = "wait";
uint256 DIFF("0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
std::mutex job_mutex;
secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);

Config cfg("clcminer.json");

int i = 0;
int totalHashes = 0;  // Shared variable to track the total number of hashes across all threads
uint256 best("0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
std::mutex hash_mutex;  // Mutex to protect totalHashes

// Function to perform SHA-256 hashing
string sha256(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    string hexStr;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02x", hash[i]);
        hexStr += buf;
    }
    return hexStr;
}

void sha256_array(const string& input, unsigned char msg_hash[32]) {
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), msg_hash);
}

// Function to update mining job
void updateJob() {
    CURL* curl = curl_easy_init();
    if (!curl) return;

    string url = cfg.getServer() + "/get-challenge";
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, string* output) -> size_t {
        output->append((char*)contents, size * nmemb);
        return size * nmemb;
    });

    string response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cout << RED << "Error syncing job status: " << res << " url: " << url << RESET << endl;
        return;
    }

    json data = json::parse(response);
    std::lock_guard<std::mutex> lock(job_mutex);

    if (data["seed"] != SEED) {
        SEED = data["seed"];
        best = uint256("0000FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
        DIFF = uint256(data["diff"].get<std::string>());

        cout << BLUE << "\nNEW JOB" << RESET << "\nDIFF: " << DIFF.toHex() << "\nSEED: " << SEED << "\nREWARD: " << GREEN << data["reward"] << RESET << endl << endl;
        i = 0;
    }
}

// Function to submit the found hash, public key, and signature
void submitHash(const string& pubKeyHex, const string& sign, const string& hashHex, const string& privHexKey) {
    CURL* curl = curl_easy_init();
    if (!curl) return;
    
    string url = cfg.getServer() + "/challenge-solved?holder=" + pubKeyHex + "&sign=" + sign + "&hash=" + hashHex;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](void* contents, size_t size, size_t nmemb, string* output) -> size_t {
        output->append((char*)contents, size * nmemb);
        return size * nmemb;
    });

    string response;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) return;
    
    json data = json::parse(response);
    std::lock_guard<std::mutex> lock(job_mutex);

    if (!data.contains("id")) {
        if (data.contains("error")) cout << YELLOW << data["error"].get<string>() << RESET << endl;
        else cout << RED << "Error submitting hash: " << response << RESET << endl;
        return;
    }
    
    cout << GREEN << "Submmited successfully" << RESET << endl;

    ofstream outputFile(cfg.getRewardsDir() + "/" + to_string(data["id"].get<int>()) + ".coin");
    if (outputFile.is_open()) {
        outputFile << privHexKey;
        outputFile.close();
    } else {
        cout << RED << "Error: " << cfg.getRewardsDir() << " directory does not exist!" << RESET << endl;
        assert(false);
    }
}

// Mining function for each thread
void mine(int thread_id) {
    unsigned char privateKey[32];
    secp256k1_pubkey pubKey;
    long long localHashCount = 0;  // Local counter for the number of hashes mined by the current thread
    auto startTime = chrono::steady_clock::now();

    while (true) {
        {
            std::lock_guard<std::mutex> lock(job_mutex);
            if (SEED == "wait") continue;
        }

        RAND_bytes(privateKey, 32);
        if (!secp256k1_ec_pubkey_create(ctx, &pubKey, privateKey)) continue;

        unsigned char pubKeySerialized[65];
        size_t pubKeyLen = 65;
        secp256k1_ec_pubkey_serialize(ctx, pubKeySerialized, &pubKeyLen, &pubKey, SECP256K1_EC_UNCOMPRESSED);

        string pubKeyHex;
        for (size_t j = 0; j < pubKeyLen; j++) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", pubKeySerialized[j]);
            pubKeyHex += buf;
        }

        string hashHex = sha256(pubKeyHex + SEED);
        uint256 hashValue(hashHex);
        
        if (hashValue <= best) best = hashValue;

        // check if the hash meets the difficulty criteria
        if (hashValue <= DIFF) {
            cout << GREEN << "\nFound CLC!" << RESET << "\nPublic Key: " << pubKeyHex << "\nPrivate Key: ";
            for (int j = 0; j < 32; j++) printf("%02x", privateKey[j]);
            cout << "\nHash: " << hashHex << endl;
            
            
            string privKeyHex;
            for (size_t j = 0; j < 32; j++) {
                char buf[3];
                snprintf(buf, sizeof(buf), "%02x", privateKey[j]);
                privKeyHex += buf;
            }

            secp256k1_ecdsa_signature signature;
            
            // get the hash of the public key in array format
            unsigned char msg_hash[32];
            sha256_array(pubKeyHex, msg_hash);

            if (secp256k1_ecdsa_sign(ctx, &signature, msg_hash, privateKey, nullptr, nullptr) == 1) {
                // Serialize the signature to a byte array
                unsigned char sigSerialized[72];
                size_t sigLen = 72;
                if (secp256k1_ecdsa_signature_serialize_der(ctx, sigSerialized, &sigLen, &signature) == 1) {
                    // Convert the serialized signature to a hex string
                    string signHex;
                    for (size_t j = 0; j < sigLen; j++) {
                        char buf[3];
                        snprintf(buf, sizeof(buf), "%02x", sigSerialized[j]);
                        signHex += buf;
                    }

                    // Submit the found hash
                    cout << GREEN << "Signature: " << signHex << RESET << endl << endl;
                    submitHash(pubKeyHex, signHex, hashHex, privKeyHex);
                }
            }

            updateJob();
        }

        localHashCount++;  // Increment local count for hashes mined
        {
            std::lock_guard<std::mutex> lock(hash_mutex);
            totalHashes++;  // Update the global hash counter
        }

        // Print the hash speed every 5 seconds from the first thread
        if (thread_id == 0) {
            auto elapsed = chrono::steady_clock::now() - startTime;
            auto seconds = chrono::duration_cast<chrono::seconds>(elapsed).count();
            if (seconds >= 4) {
                double hashesPerSecond = static_cast<double>(totalHashes) / seconds;
                
                // Determine the appropriate unit
                string unit = "H";
                double readableHashRate = hashesPerSecond;
        
                if (readableHashRate >= 1e12) {  // Greater than or equal to 1 TH/s
                    readableHashRate /= 1e12;
                    unit = "TH";
                } else if (readableHashRate >= 1e9) {  // Greater than or equal to 1 GH/s
                    readableHashRate /= 1e9;
                    unit = "GH";
                } else if (readableHashRate >= 1e6) {  // Greater than or equal to 1 MH/s
                    readableHashRate /= 1e6;
                    unit = "MH";
                } else if (readableHashRate >= 1e3) {  // Greater than or equal to 1 KH/s
                    readableHashRate /= 1e3;
                    unit = "KH";
                }
        
                cout << YELLOW << "Hash Rate: " << readableHashRate << " " << unit << "/sec Best: " << best.toHex() << RESET << endl;
        
                // Reset the counters
                startTime = chrono::steady_clock::now();
                totalHashes = 0;
            }
        }        
    }
}

int main() {
    cout << GREEN << "Starting CLC miner...\nSyncing job..." << RESET << endl << endl;
    cout << YELLOW << "clcminer.json config:" << RESET << endl;
    cout << BLUE << "Server: " << cfg.getServer().c_str() << endl;
    int t = cfg.getThreads();
    if (t == -1) t = thread::hardware_concurrency();
    cout << "Threads: " << RESET << RED << t << RESET << endl;

    updateJob();
    thread jobThread([]() {
        while (true) {
            updateJob();
            this_thread::sleep_for(chrono::seconds(1));
        }
    });

    vector<thread> threads;
    int numThreads = 1;
    // check if number of threads is specified in clc config
    if (cfg.getThreads() == -1) numThreads = thread::hardware_concurrency();
    else numThreads = cfg.getThreads();

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back(mine, i);
    }

    for (auto& t : threads) t.join();
    jobThread.join();
    return 0;
}
