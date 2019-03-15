#include "SysInfo.h"
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <fstream>
#include <string>
#include <QtGlobal>
#include <QDateTime>

double SysInfo::cpuUsage() {
    auto lastCpuData = mCpuData;
    mCpuData = cpuRawData();
    unsigned long long lastCpuTotal = 0, curCpuTotal = 0;
    for (size_t i = 0; i < 10; ++i) {
        lastCpuTotal += lastCpuData[i];
        curCpuTotal += mCpuData[i];
    }
    auto deltaCpuTotal = curCpuTotal - lastCpuTotal;
    auto cpuUsage = (deltaCpuTotal - (mCpuData[3] - lastCpuData[3])) * 100.0 / deltaCpuTotal;
    return qBound(0.0, cpuUsage, 100.0);
}

double SysInfo::memUsage() {
    struct sysinfo info;
    sysinfo(&info);
    auto memUsage = (info.totalram - info.freeram) * 100.0 / info.totalram;
    return qBound(0.0, memUsage, 100.0);
}

double SysInfo::swapUsage() {
    struct sysinfo info;
    sysinfo(&info);
    auto swapUsage = (info.totalswap - info.freeswap) * 100.0 / info.totalswap;
    return qBound(0.0, swapUsage, 100.0);
}

std::array<unsigned long long, 10> SysInfo::cpuRawData() {
    std::ifstream inFile("/proc/stat");
    std::string cpu;
    std::array<unsigned long long, 10> rawData;
    inFile >> cpu;
    for (auto &i : rawData)
        inFile >> i;
    inFile.close();
    return rawData;
}

SysInfo::SysInfo()
    : mCpuData(cpuRawData()) {

    std::ifstream inFile;
    std::string infoText;

    inFile.open("/proc/sys/kernel/hostname");
    std::getline(inFile, infoText);
    mHostname = QString::fromStdString(infoText);
    inFile.close();

    inFile.open("/proc/sys/kernel/ostype");
    std::getline(inFile, infoText);
    inFile.close();
    inFile.open("/proc/sys/kernel/osrelease");
    std::string version;
    std::getline(inFile, version);
    infoText += ' ' + version;
    mSystemVer = QString::fromStdString(infoText);
    inFile.close();

    inFile.open("/proc/cpuinfo");
    QString cpuinfo;
    while (std::getline(inFile, infoText)) {
        QString line = QString::fromStdString(infoText);
        if (line.startsWith("model name"))
            cpuinfo = line.mid(line.indexOf(':') + 2);
        else if (line.startsWith("siblings")) {
            cpuinfo += "  x  " + line.mid(line.indexOf(':') + 2);
            break;
        }
    }
    mCpuInfo = cpuinfo;
    inFile.close();

    inFile.open("/proc/uptime");
    std::getline(inFile, infoText);
    QString lastingTime = QString::fromStdString(infoText.substr(0, infoText.find('.')));
    mLastingTime = lastingTime + 's';
    QString startingTime = QDateTime::currentDateTime().
                           addSecs(-lastingTime.toInt()).
                           toString("yyyy-MM-dd ddd hh:mm:ss");
    mStartingTime = startingTime;
    inFile.close();
}
