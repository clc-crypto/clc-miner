#include "config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "colors.h"

using namespace std;
using json = nlohmann::json;

Config::Config(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        cout << YELLOW << "Error opening config file: " << path << RESET << endl;
        cout << "Creating an empty config file..." << endl;
        std::ofstream newFile("clcminer.json");

        if (newFile.is_open()) {
            newFile << "{}";
            newFile.close();
            std::cout << "Config created successfully";
        } else {
            std::cerr << "Error creating config file";
        }
        cout << RED << "Reset clcminer to apply new config!" << RESET << endl;
        exit(1);
    }

    json configData;
    file >> configData;
    file.close();

    // Correct assignments
    if (configData.contains("server") && configData["server"].is_string()) {
        server = configData["server"].get<string>();
    } else {
        cout << YELLOW << "Warning: 'server' key not found in config file! Defaulting to `http://localhost:3000`" << RESET << endl;
        server = "http://localhost:3000";
    }

    if (configData.contains("rewardsDir") && configData["rewardsDir"].is_string()) {
        rewards = configData["rewardsDir"].get<string>();  // ✅ Corrected
    } else {
        cout << YELLOW << "Warning: 'rewardsDir' key not found in config file! Defaulting to `rewards`" << RESET << endl;
        rewards = "rewards";
    }

    if (configData.contains("threads") && configData["threads"].is_number_integer()) {
        threads = configData["threads"].get<int>();  // ✅ Corrected
    } else {
        cout << YELLOW << "Warning: 'threads' key not found in config file! Defaulting to `-1`" << RESET << endl;
        threads = -1;
    }
}

string Config::getServer() const {
    return server;
}

string Config::getRewardsDir() const {
    return rewards;
}

int Config::getThreads() const {
    return threads;
}
