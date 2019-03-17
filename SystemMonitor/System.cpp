#include "System.h"
#include <sys/sysinfo.h>
#include <QtGlobal>
#include <QDir>
#include <sstream>

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
    setProcessMap();
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
    auto memUsage = (info.totalram - info.freeram) * 100.0 / info.totalram;
    mMemUsage = qBound(0.0, memUsage, 100.0);
    auto swapUsage = (info.totalswap - info.freeswap) * 100.0 / info.totalswap;
    mSwapUsage =  qBound(0.0, swapUsage, 100.0);
}

void System::setProcessMap() {
    auto dirNameList = QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto &dirName : dirNameList) {
        bool ok;
        int pid = dirName.toInt(&ok);
        string name;
        mProcessIdMap[pid] = make_shared<ProcessInfo>();
        if (ok) {
            string path = "/proc/" + to_string(pid);
            bool shouldEnd = false;
            int count = 0;

            string line, temp;
            stringstream ss;
            mFile.open(path + "status");
            while (getline(mFile, line) && !shouldEnd) {
                ++count;
                switch (count) {
                case 1:
                    name = line.substr(line.find_last_of(' ') + 1);
                    break;
                case 3: {
                    auto start = line.find_first_of('(') + 1;
                    auto end = line.find_last_of(')') - 1;
                    mProcessIdMap[pid]->state = line.substr(start, end - start + 1);
                    break;
                }
                case 9: {
                    auto start = line.find_first_not_of(' ');
                    string uid = line.substr(start, line.find_first_of(' ', start) - start);
                    ifstream searchFile("/etc/passwd");
                    while (getline(searchFile, temp)) {
                        auto pos = temp.find(uid);
                        if (pos != string::npos) {
                            mProcessIdMap[pid]->user = temp.substr(0, pos - 3);
                            break;
                        }
                    }
                    searchFile.close();
                    break;
                }
                case 18:
                    ss.str(line);
                    ss >> temp >> mProcessIdMap[pid]->size;
                    break;
                case 22: {
                    ss.str(line);
                    ss >> temp >> mProcessIdMap[pid]->resident;
                    struct sysinfo info;
                    sysinfo(&info);
                    auto percent = mProcessIdMap[pid]->resident * 1024 * 100.0 / info.totalram;
                    mProcessIdMap[pid]->memPercent = qBound(0.0, percent, 100.0);
                    break;
                }
                case 24: //part of shared
                    ss.str(line);
                    ss >> temp >> mProcessIdMap[pid]->shared;
                    break;
                case 25: {
                    unsigned long rssShmem;
                    ss.str(line);
                    ss >> temp >> rssShmem;
                    mProcessIdMap[pid]->shared += rssShmem;
                    shouldEnd = true;
                    break;
                }
                }
            }
            mFile.close();

            shouldEnd = false;
            count = 0;
            unsigned long long curCpuTime = 0;
            mFile.open(path + "stat");
            while (mFile >> temp && !shouldEnd) {
                ++count;
                switch (count) {
                case 4:
                    mProcessIdMap[pid]->ppid = stoi(temp);
                    break;
                case 14:
                    curCpuTime = stoull(temp);
                    break;
                case 15: {
                    curCpuTime += stoull(temp);
                    auto percent = (curCpuTime - mProcessIdMap[pid]->prevCpuTime) * 100.0 / (mCurCpuRawData[0] - mPrevCpuRawData[0]);
                    mProcessIdMap[pid]->cpuPercent = qBound(0.0, percent, 100.0);
                    mProcessIdMap[pid]->prevCpuTime = curCpuTime;
                    break;
                }
                case 18:
                    mProcessIdMap[pid]->priority = stol(temp);
                    break;
                case 19:
                    mProcessIdMap[pid]->nice = stol(temp);
                    break;
                case 20:
                    mProcessIdMap[pid]->threadNum = stol(temp);
                    break;
                case 22:
                    mProcessIdMap[pid]->startTime = stoull(temp);
                    shouldEnd = true;
                    break;
                }
            }
            mFile.close();
        }
        mProcessNameMap[name] = mProcessIdMap[pid];
    }
}

System::System() {
    mTimer.setInterval(1000);
    connect(&mTimer, &QTimer::timeout, this, &System::update);
    mTimer.start();

    string info;
    mFile.open("/proc/uptime");
    getline(mFile, info);
    QString lastingTime = QString::fromStdString(info.substr(0, info.find('.')));
    mStartingTime = QDateTime::currentDateTime().addSecs(-lastingTime.toInt());
    mFile.close();

    setCpuUsage();
    setMemSwapUsage();
    setProcessMap();
}
