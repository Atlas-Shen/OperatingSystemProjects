#ifndef SYSINFO_H
#define SYSINFO_H

#include <array>
#include <QString>

class SysInfo {

public:
    static SysInfo &instance();

    QString &hostname();
    QString &systemVer();
    QString &cpuInfo();
    QString &startingTime();
    QString &lastingTime();
    double cpuUsage();
    double memUsage();
    double swapUsage();

private:
    QString mHostname;
    QString mSystemVer;
    QString mCpuInfo;
    QString mStartingTime;
    QString mLastingTime;
    std::array<unsigned long long, 10> mCpuData;

    std::array<unsigned long long, 10> cpuRawData();

    SysInfo();
    SysInfo(const SysInfo &) = delete;
    SysInfo(SysInfo &&) = delete;
    SysInfo &operator=(const SysInfo &) = delete;
    SysInfo &operator=(SysInfo &&) = delete;
};

inline SysInfo &SysInfo::instance() {
    static SysInfo singleton;
    return singleton;
}

inline QString &SysInfo::hostname() {
    return mHostname;
}

inline QString &SysInfo::systemVer() {
    return mSystemVer;
}

inline QString &SysInfo::cpuInfo() {
    return mCpuInfo;
}

inline QString &SysInfo::lastingTime() {
    return mLastingTime;
}

inline QString &SysInfo::startingTime() {
    return mStartingTime;
}

#endif // SYSINFO_H
