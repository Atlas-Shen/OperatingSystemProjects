#include "System.h"
#include <sys/sysinfo.h>
#include <QtGlobal>

using namespace std;

QString System::hostname() {
    string info;
    file.open("/proc/sys/kernel/hostname");
    getline(file, info);
    file.close();
    return QString::fromStdString(info);
}

QString System::version() {
    string info;
    file.open("/proc/version_signature");
    getline(file, info);
    file.close();
    return QString::fromStdString(info);
}

void System::update() {
    setCpuUsage();
    setMemSwapUsage();
    setCpuInfo();
    emit toMainWindow();
    emit toStatusWidget();
    emit toResourceWidget();
}

void System::setCpuUsage() {
    prevCpuRawData = move(curCpuRawData);
    string cpu;
    file.open("/proc/stat");
    file >> cpu;
    unsigned long long a[10];
    for (auto &i : a)
        file >> i;
    file.close();
    curCpuRawData[1] = a[3];
    curCpuRawData[0] = 0;
    for (auto &i : a)
        curCpuRawData[0] += i;
    auto deltaCpuTotal = curCpuRawData[0] - prevCpuRawData[0];
    auto cpuUsage = (deltaCpuTotal - (curCpuRawData[1] - prevCpuRawData[1])) * 100.0 / deltaCpuTotal;
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

void System::setCpuInfo() {
    mCpuInfo.clear();
    string info;
    file.open("/proc/cpuinfo");
    while (getline(file, info)) {
        QString line = QString::fromStdString(info);
        if (line.startsWith("processor"))
            mCpuInfo << line;
        else if (line.startsWith("model name"))
            mCpuInfo << line;
        else if (line.startsWith("cpu MHz"))
            mCpuInfo << line;
    }
    file.close();
}

System::System() {
    string info;
    file.open("/proc/uptime");
    getline(file, info);
    QString lastingTime = QString::fromStdString(info.substr(0, info.find('.')));
    mStartingTime = QDateTime::currentDateTime().addSecs(-lastingTime.toInt());
    file.close();

    update();

    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, &System::update);
    timer.start();
}
