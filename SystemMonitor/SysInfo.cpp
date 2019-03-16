#include "SysInfo.h"
#include <sys/sysinfo.h>
#include <string>
#include <QtGlobal>

QString SysInfo::hostname() {
    std::string info;
    mFile.open("/proc/sys/kernel/hostname");
    std::getline(mFile, info);
    mFile.close();
    return QString::fromStdString(info);
}

QString SysInfo::systemVer() {
    std::string info;
    mFile.open("/proc/sys/kernel/ostype");
    std::getline(mFile, info);
    mFile.close();
    mFile.open("/proc/sys/kernel/osrelease");
    std::string version;
    std::getline(mFile, version);
    info += ' ' + version;
    mFile.close();
    return QString::fromStdString(info);
}

QString SysInfo::cpuInfo() {
    std::string info;
    mFile.open("/proc/cpuinfo");
    QString cpuInfo;
    while (std::getline(mFile, info)) {
        QString line = QString::fromStdString(info);
        if (line.startsWith("model name"))
            cpuInfo = line.mid(line.indexOf(':') + 2);
        else if (line.startsWith("siblings")) {
            cpuInfo += "  x  " + line.mid(line.indexOf(':') + 2);
            break;
        }
    }
    mFile.close();
    return cpuInfo;
}

void SysInfo::update() {
    setCpuUsage();
    setMemSwapUsage();
    emit toMainWindow();
    emit toStatusWidget();
    emit toResourceWidget();
}

void SysInfo::setCpuUsage() {
    mPrevCpuRawData = std::move(mCurCpuRawData);
    std::string cpu;
    mFile.open("/proc/stat");
    mFile >> cpu;
    unsigned long long a[10];
    for (auto &i : a)
        mFile >> i;
    mFile.close();
    mCurCpuRawData[1] = a[3];
    mCurCpuRawData[0] = 0;
    for (auto &i : a)
        mCurCpuRawData[0] += i;
    auto deltaCpuTotal = mCurCpuRawData[0] - mPrevCpuRawData[0];
    auto cpuUsage = (deltaCpuTotal - (mCurCpuRawData[1] - mPrevCpuRawData[1])) * 100.0 / deltaCpuTotal;
    mCpuUsage = qBound(0.0, cpuUsage, 100.0);
}

void SysInfo::setMemSwapUsage() {
    struct sysinfo info;
    sysinfo(&info);
    auto memUsage = (info.totalram - info.freeram) * 100.0 / info.totalram;
    mMemUsage = qBound(0.0, memUsage, 100.0);
    auto swapUsage = (info.totalswap - info.freeswap) * 100.0 / info.totalswap;
    mSwapUsage =  qBound(0.0, swapUsage, 100.0);
}

SysInfo::SysInfo() {
    mTimer.setInterval(1000);
    connect(&mTimer, &QTimer::timeout, this, &SysInfo::update);
    mTimer.start();

    std::string info;
    mFile.open("/proc/uptime");
    std::getline(mFile, info);
    QString lastingTime = QString::fromStdString(info.substr(0, info.find('.')));
    mStartingTime = QDateTime::currentDateTime().addSecs(-lastingTime.toInt());
    mFile.close();

    setCpuUsage();
    setMemSwapUsage();
}
