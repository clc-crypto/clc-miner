#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
private:
    std::string server;
    std::string rewards;
    int threads;

public:
    Config(const std::string& path);
    std::string getServer() const;
    std::string getRewardsDir() const;
    int getThreads() const;
};

#endif
