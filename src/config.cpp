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
        cout << "Creating a empty config file..." << endl;
        std::ofstream file("clcminer.json");

        if (file.is_open()) {
            file << "{}";
            file.close();
            std::cout << "Config created successfully";
        } else {
            std::cerr << "Error creating config file";
        }
        cout << RED << "Reset clcminer to apply config!" << RESET << endl;
        exit(1);
    }

    json configData;
    file >> configData;
    file.close();

    if (configData.contains("server")) {
        server = configData["server"].get<string>();
    } else {
        cout << YELLOW << "Warning: 'server' key not found in config file! Defaulting to `http://localhost:3000`" << RESET << endl;
        server = "http://localhost:3000";
    }

    if (configData.contains("rewardsDir")) {
        server = configData["rewardsDir"].get<string>();
    } else {
        cout << YELLOW << "Warning: 'rewards' key not found in config file! Defaulting to `rewards`" << RESET << endl;
        rewards = "rewards";
    }

    if (configData.contains("threads")) {
        server = configData["threads"].get<int>();
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
