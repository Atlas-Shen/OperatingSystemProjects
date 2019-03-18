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

QString System::systemVer() {
    string info;
    file.open("/proc/sys/kernel/ostype");
    getline(file, info);
    file.close();
    file.open("/proc/sys/kernel/osrelease");
    string version;
    getline(file, version);
    info += ' ' + version;
    file.close();
    return QString::fromStdString(info);
}

QString System::cpuInfo() {
    string info;
    file.open("/proc/cpuinfo");
    QString cpuInfo;
    while (getline(file, info)) {
        QString line = QString::fromStdString(info);
        if (line.startsWith("model name"))
            cpuInfo = line.mid(line.indexOf(':') + 2);
        else if (line.startsWith("siblings")) {
            cpuInfo += "  x  " + line.mid(line.indexOf(':') + 2);
            break;
        }
    }
    file.close();
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
