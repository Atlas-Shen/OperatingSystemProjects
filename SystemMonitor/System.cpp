#include "System.h"
#include <sys/sysinfo.h>
#include <QtGlobal>

using namespace std;

QString System::hostname() {
    string info;
    mFile.open("/proc/sys/kernel/hostname");
    getline(mFile, info);
    mFile.close();
    return QString::fromStdString(info);
}

QString System::systemVer() {
    string info;
    mFile.open("/proc/sys/kernel/ostype");
    getline(mFile, info);
    mFile.close();
    mFile.open("/proc/sys/kernel/osrelease");
    string version;
    getline(mFile, version);
    info += ' ' + version;
    mFile.close();
    return QString::fromStdString(info);
}

QString System::cpuInfo() {
    string info;
    mFile.open("/proc/cpuinfo");
    QString cpuInfo;
    while (getline(mFile, info)) {
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

void System::update() {
    setCpuUsage();
    setMemSwapUsage();
    emit toMainWindow();
    emit toStatusWidget();
    emit toResourceWidget();
}

void System::setCpuUsage() {
    mPrevCpuRawData = move(mCurCpuRawData);
    string cpu;
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

void System::setMemSwapUsage() {
    struct sysinfo info;
    sysinfo(&info);
    auto memUsage = (info.totalram - info.freeram - info.bufferram) * 100.0 / info.totalram;
    mMemUsage = qBound(0.0, memUsage, 100.0);
    auto swapUsage = (info.totalswap - info.freeswap) * 100.0 / info.totalswap;
    mSwapUsage =  qBound(0.0, swapUsage, 100.0);
}

System::System() {
    string info;
    mFile.open("/proc/uptime");
    getline(mFile, info);
    QString lastingTime = QString::fromStdString(info.substr(0, info.find('.')));
    mStartingTime = QDateTime::currentDateTime().addSecs(-lastingTime.toInt());
    mFile.close();

    update();

    mTimer.setInterval(1000);
    connect(&mTimer, &QTimer::timeout, this, &System::update);
    mTimer.start();
}
