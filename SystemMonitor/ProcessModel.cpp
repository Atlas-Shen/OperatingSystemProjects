#include "ProcessModel.h"
#include <sys/sysinfo.h>
#include <unistd.h>
#include <fstream>
#include <algorithm>
#include <QDir>
#include <QTimer>

using namespace std;

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent),
      sortedColumn(CPUPercent),
      pTimer(new QTimer(this)) {
    update();
    pTimer->setInterval(1000);
    connect(pTimer, &QTimer::timeout, this, &ProcessModel::update);
    pTimer->start();
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case Name:
            return QString("Name");
        case User:
            return QString("User");
        case State:
            return QString("State");
        case PID:
            return QString("PID");
        case PPID:
            return QString("PPID");
        case CPUPercent:
            return QString("CPU%");
        case MemPercent:
            return QString("Mem%");
        case Priority:
            return QString("Priority");
        case Nice:
            return QString("Nice");
        case ThreadNum:
            return QString("Thread");
        }
    }
    return QVariant();
}

int ProcessModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return static_cast<int>(showList.size());
}

int ProcessModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 10;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const {
    int pid = showList.at(static_cast<size_t>(index.row()));

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case Name:
            return QString::fromStdString(pidMap.at(pid).name);
        case User:
            return QString::fromStdString(pidMap.at(pid).user);
        case State:
            return QString::fromStdString(pidMap.at(pid).state);
        case PID:
            return pidMap.at(pid).pid;
        case PPID:
            return pidMap.at(pid).ppid;
        case CPUPercent:
            return QString("%1%").arg(pidMap.at(pid).cpuPercent, 0, 'f', 4);
        case MemPercent:
            return QString("%1%").arg(pidMap.at(pid).memPercent, 0, 'f', 4);
        case Priority:
            return pidMap.at(pid).priority;
        case Nice:
            return pidMap.at(pid).nice;
        case ThreadNum:
            return pidMap.at(pid).threadNum;
        }
    }
    return QVariant();
}

void ProcessModel::sort(int column, Qt::SortOrder order) {
    Q_UNUSED(order);
    sortedColumn = column;
    switch (column) {
    case Name:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].name < pidMap[pid2].name;
        });
        break;
    case User:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].user < pidMap[pid2].user;
        });
        break;
    case PID:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].pid < pidMap[pid2].pid;
        });
        break;
    case PPID:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].ppid < pidMap[pid2].ppid;
        });
        break;
    case CPUPercent:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].cpuPercent > pidMap[pid2].cpuPercent;
        });
        break;
    case MemPercent:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].memPercent > pidMap[pid2].memPercent;
        });
        break;
    case Priority:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].priority > pidMap[pid2].priority;
        });
        break;
    case Nice:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].nice < pidMap[pid2].nice;
        });
        break;
    case ThreadNum:
        stable_sort(showList.begin(), showList.end(), [&](int pid1, int pid2)->bool {
            return pidMap[pid1].threadNum > pidMap[pid2].threadNum;
        });
        break;
    }
    emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(), columnCount(), QModelIndex()), {Qt::DisplayRole});
}

void ProcessModel::refresh() {
    showList.clear();
    update();
}

bool ProcessModel::search(int pid) {
    if (pidMap.count(pid) == 1) {
        showList.clear();
        showList.push_back(pid);
        emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(), columnCount(), QModelIndex()), {Qt::DisplayRole});
        return true;
    }
    return false;
}

bool ProcessModel::search(string name) {
    if (nameMap.count(name) != 0) {
        showList.clear();
        auto equalRange = nameMap.equal_range(name);
        for (auto iter = equalRange.first; iter != equalRange.second; ++iter)
            showList.push_back(iter->second);
        emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(), columnCount(), QModelIndex()), {Qt::DisplayRole});
        return true;
    }
    return false;
}

void ProcessModel::update() {
    pidMap.clear();
    nameMap.clear();

    auto dirNameList = QDir("/proc").entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (auto &dirName : dirNameList) {
        bool ok;
        int pid = dirName.toInt(&ok);
        if (ok) {
            string path = "/proc/" + to_string(pid);

            //read /proc/[pid]/status for
            //name, user, state
            //1     9     3
            string statusBuffer[9];
            ifstream statusFile(path + "/status");
            if (!statusFile.is_open())
                continue;
            for (auto &i : statusBuffer)
                getline(statusFile, i);
            statusFile.close();

            //read /proc/[pid]/stat for
            //ppid, utime, stime, rss, priority, nice, num_thread
            //4     14     15     24   18        19    20
            string statBuffer[24];
            ifstream statFile(path + "/stat");
            if (!statFile.is_open())
                continue;
            for (auto &i : statBuffer)
                statFile >> i;
            statFile.close();

            //read cpu data
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

            //insert pid and info pair into the map
            pidMap.emplace(pid, ProcessInfo());

            //set name field
            pidMap[pid].name = statusBuffer[0].substr(statusBuffer[0].find_last_of('\t') + 1, string::npos);
            nameMap.emplace(pidMap[pid].name, pid);

            //set user field
            size_t start = statusBuffer[8].find_first_of('\t') + 1;
            string uidToFind = ":x:" + statusBuffer[8].substr(start, statusBuffer[8].find_first_of('\t', start) - start);
            ifstream searchFile("/etc/passwd");
            string temp;
            while (getline(searchFile, temp)) {
                auto pos = temp.find(uidToFind);
                if (pos != string::npos) {
                    pidMap[pid].user = temp.substr(0, pos);
                    break;
                }
            }
            searchFile.close();

            //set state field
            start = statusBuffer[2].find_first_of('(') + 1;
            size_t end = statusBuffer[2].find_last_of(')') - 1;
            pidMap[pid].state = statusBuffer[2].substr(start, end - start + 1);

            //set pid, ppid field
            pidMap[pid].pid = pid;
            pidMap[pid].ppid = stoi(statBuffer[3]);

            //set cpu field
            unsigned long long curCpuTime = stoull(statBuffer[13]) + stoull(statBuffer[14]);
            double cpuPercent = (curCpuTime - pidMap[pid].prevCpuTime) * 100.0
                                / (curCpuTotal - pidMap[pid].prevCpuTotal);
            pidMap[pid].cpuPercent = qBound(0.0, cpuPercent, 100.0);
            pidMap[pid].prevCpuTime = curCpuTime;
            pidMap[pid].prevCpuTotal = curCpuTotal;

            //set mem field
            auto resident = stol(statBuffer[23]) * sysconf(_SC_PAGESIZE);
            struct sysinfo info;
            sysinfo(&info);
            double memPercent = resident * 100.0 / info.totalram;
            pidMap[pid].memPercent = qBound(0.0, memPercent, 100.0);

            //set priority, nice, threadNum, startTime field
            pidMap[pid].priority = stoll(statBuffer[17]);
            pidMap[pid].nice = stoll(statBuffer[18]);
            pidMap[pid].threadNum = stoll(statBuffer[19]);
        }
    }

    for (auto iter = showList.begin(); iter < showList.end();) {
        if (pidMap.count(*iter) == 0)
            iter = showList.erase(iter);
        else
            ++iter;
    }
    if (showList.empty()) {
        for (auto &i : pidMap)
            showList.push_back(i.first);
    }
    sort(sortedColumn);
}
