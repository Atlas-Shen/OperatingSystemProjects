#include <QDebug>
#include "ProcessModel.h"
#include <sys/sysinfo.h>
#include <fstream>
#include <sstream>
#include <QDir>
#include <QTimer>

using namespace std;

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent),
      pTimer(new QTimer(this)) {
    update();
    pTimer->setInterval(1000);
    connect(pTimer, &QTimer::timeout, this, &ProcessModel::update);
    pTimer->start();
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Pid");
        case 1:
            return QString("Name");
        case 2:
            return QString("User");
        case 3:
            return QString("State");
        case 4:
            return QString("CPU%");
        case 5:
            return QString("Mem%");
        }
    }
    return QVariant();
}

int ProcessModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(mProcessIdList.size());
}

int ProcessModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    //name user state pid ppid cpu% mem% size resident shared priority nice threadNum startTime
    return 6;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const {
    int pid = mProcessIdList.at(static_cast<size_t>(index.row()));
    auto pointer = mProcessIdMap.at(pid);

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0:
            return pid;
        case 1:
            return QString::fromStdString(pointer->name);
        case 2:
            return QString::fromStdString(pointer->user);
        case 3:
            return QString::fromStdString(pointer->state);
        case 4:
            return QString("%1%").arg(pointer->cpuPercent, 0, 'f', 2);
        case 5:
            return QString("%1%").arg(pointer->memPercent, 0, 'f', 2);
        }
    }
    return QVariant();
}

void ProcessModel::sort(int column, Qt::SortOrder order) {

}

void ProcessModel::update() {
    mProcessIdList.clear();
    mProcessIdMap.clear();
    mProcessNameMap.clear();

    auto dirNameList = QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto &dirName : dirNameList) {
        bool ok;
        int pid = dirName.toInt(&ok);
        if (ok) {
            string path = "/proc/" + to_string(pid);

            string statusBuffer[25];
            ifstream statusFile(path + "/status");
            for (auto &i : statusBuffer)
                getline(statusFile, i);
            statusFile.close();

            string statBuffer[22];
            ifstream statFile(path + "/stat");
            for (auto &i : statBuffer)
                statFile >> i;
            statFile.close();

            mProcessIdList.push_back(pid);
            mProcessIdMap.emplace(pid, make_shared<ProcessInfo>());

            mProcessIdMap[pid]->pid = pid;
            mProcessIdMap[pid]->ppid = stoi(statBuffer[3]);
            mProcessIdMap[pid]->name = statusBuffer[0].substr(statusBuffer[0].find_last_of('\t') + 1, string::npos);
            mProcessNameMap.emplace(mProcessIdMap[pid]->name, mProcessIdMap[pid]);

            size_t start = statusBuffer[8].find_first_of('\t') + 1;
            string uidToFind = ":x:" + statusBuffer[8].substr(start, statusBuffer[8].find_first_of('\t', start) - start);
            ifstream searchFile("/etc/passwd");
            string temp;
            while (getline(searchFile, temp)) {
                auto pos = temp.find(uidToFind);
                if (pos != string::npos) {
                    mProcessIdMap[pid]->user = temp.substr(0, pos);
                    break;
                }
            }
            searchFile.close();

            start = statusBuffer[2].find_first_of('(') + 1;
            size_t end = statusBuffer[2].find_last_of(')') - 1;
            mProcessIdMap[pid]->state = statusBuffer[2].substr(start, end - start + 1);

            stringstream ss(statusBuffer[17]);
            ss >> temp >> mProcessIdMap[pid]->size;

            ss.str(statusBuffer[21]);
            ss >> temp >> mProcessIdMap[pid]->resident;

            struct sysinfo info;
            sysinfo(&info);
            double memPercent = mProcessIdMap[pid]->resident * 1024 * 100.0 / info.totalram;
            mProcessIdMap[pid]->memPercent = qBound(0.0, memPercent, 100.0);

            ss.str(statusBuffer[23]);
            ss >> temp >> mProcessIdMap[pid]->shared;

            unsigned long rssShmem;
            ss.str(statusBuffer[24]);
            ss >> temp >> rssShmem;
            mProcessIdMap[pid]->shared += rssShmem;

            unsigned long long curCpuTime = 0;
            curCpuTime = stoull(statBuffer[13]) + stoull(statBuffer[14]);
            ifstream cpuFile("/proc/stat");
            string cpu;
            cpuFile >> cpu;
            unsigned long long a[10];
            for (auto &i : a)
                cpuFile >> i;
            cpuFile.close();
            unsigned long long curCpuTotal = 0;
            for (auto &i : a)
                curCpuTotal += i;
            double cpuPercent = (curCpuTime - mProcessIdMap[pid]->prevCpuTime) * 100.0
                                / (curCpuTotal - mProcessIdMap[pid]->prevCpuTotal);
            mProcessIdMap[pid]->cpuPercent = qBound(0.0, cpuPercent, 100.0);
            mProcessIdMap[pid]->prevCpuTime = curCpuTime;
            mProcessIdMap[pid]->prevCpuTotal = curCpuTotal;

            mProcessIdMap[pid]->priority = stol(statBuffer[17]);
            mProcessIdMap[pid]->nice = stol(statBuffer[18]);
            mProcessIdMap[pid]->threadNum = stol(statBuffer[19]);
            mProcessIdMap[pid]->startTime = stoull(statBuffer[21]);
        }
    }

    emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(), columnCount(), QModelIndex()), {Qt::DisplayRole});
}
