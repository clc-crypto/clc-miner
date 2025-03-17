#include "config.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include "colors.h"

using namespace std;
namespace fs = std::filesystem;
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
        cout << YELLOW << "Warning: 'server' key not found in config file! Defaulting to `https://clc.ix.tc`" << RESET << endl;
        server = "https://clc.ix.tc";
    }

    if (configData.contains("rewardsDir") && configData["rewardsDir"].is_string()) {
        rewards = configData["rewardsDir"].get<string>();
    } else {
        cout << YELLOW << "Warning: 'rewardsDir' key not found in config file! Defaulting to `rewards`" << RESET << endl;
        rewards = "rewards";
    }

    if (!fs::exists(rewards)) {
        if (fs::create_directory(rewards)) {
            cout << GREEN << "Created rewards dir: " << rewards << RESET << endl;
        } else {
            cout << RED << "Failed to create rewards dir: " << rewards << RESET << endl;
        }
    }

    if (configData.contains("threads") && configData["threads"].is_number_integer()) {
        threads = configData["threads"].get<int>();
    } else {
        cout << YELLOW << "Warning: 'threads' key not found in config file! Defaulting to `-1`" << RESET << endl;
        threads = -1;
    }

    if (configData.contains("reportServer") && configData["reportServer"].is_string()) {
        reportServer = configData["reportServer"].get<string>();
    } else {
        reportServer = "none";
    }

    if (configData.contains("reportUser") && configData["reportUser"].is_string()) {
        reportUser = configData["reportUser"].get<string>();
    } else {
        reportUser = "none";
    }

    if (configData.contains("onMined") && configData["onMined"].is_string()) {
        onMined = configData["onMined"].get<string>();
        cout << YELLOW << "Going to execute: '" << onMined << "' everytime a coin is mined!" << RESET << endl;
    } else {
        onMined = "";
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

string Config::getReportServer() const {
    return reportServer;
}

string Config::getReportUser() const {
    return reportUser;
}

string Config::getOnMined() const {
    return onMined;
}