#ifndef CONFIG_H
#define CONFIG_H

#include <string>

class Config {
private:
    std::string server;
    std::string rewards;
    std::string reportServer;
    std::string reportUser;
    std::string onMined;
    int threads;

public:
    Config(const std::string& path);
    std::string getServer() const;
    std::string getRewardsDir() const;
    std::string getReportServer() const;
    std::string getReportUser() const;
    std::string getOnMined() const;
    int getThreads() const;
};

#endif
