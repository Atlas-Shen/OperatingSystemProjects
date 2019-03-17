#ifndef SYSINFO_H
#define SYSINFO_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <fstream>
#include <string>
#include <array>
#include <unordered_map>
#include <memory>

class System : public QObject {
    Q_OBJECT

private:
    struct ProcessInfo {
        std::string user;
        std::string state;
        int ppid;
        unsigned long long prevCpuTime;
        double cpuPercent;
        unsigned long size;     //in KB
        unsigned long resident; //in KB
        unsigned long shared;   //in KB
        double memPercent;
        long priority;
        long nice;
        long threadNum;
        unsigned long long startTime;
    };

public:
    static System &instance();

    QString hostname();
    QString systemVer();
    QString cpuInfo();
    QString startingTime() const;
    QString lastingTime() const;

    double cpuUsage() const;
    double memUsage() const;
    double swapUsage() const;

    const std::unordered_map<int, std::shared_ptr<ProcessInfo>> &processIdMap() const;
    const std::unordered_map<std::string, std::shared_ptr<ProcessInfo>> &processNameMap() const;

signals:
    void toMainWindow();
    void toStatusWidget();
    void toResourceWidget();

private:
    std::ifstream mFile;
    QDateTime mStartingTime;
    QTimer mTimer;
    std::array<unsigned long long, 2> mPrevCpuRawData;
    std::array<unsigned long long, 2> mCurCpuRawData;
    double mCpuUsage;
    double mMemUsage;
    double mSwapUsage;

    std::unordered_map<int, std::shared_ptr<ProcessInfo>> mProcessIdMap;
    std::unordered_map<std::string, std::shared_ptr<ProcessInfo>> mProcessNameMap;

    void update();
    void setCpuUsage();
    void setMemSwapUsage();
    void setProcessMap();

    System();
    System(const System &) = delete;
    System(System &&) = delete;
    System &operator=(const System &) = delete;
    System &operator=(System &&) = delete;
};

inline System &System::instance() {
    static System singleton;
    return singleton;
}

inline QString System::startingTime() const {
    return mStartingTime.toString("yyyy-MM-dd ddd hh:mm:ss");
}

inline QString System::lastingTime() const {
    return QString::number(mStartingTime.secsTo(QDateTime::currentDateTime()) + 's');
}

inline double System::cpuUsage() const {
    return mCpuUsage;
}

inline double System::memUsage() const {
    return mMemUsage;
}

inline double System::swapUsage() const {
    return mSwapUsage;
}

inline const std::unordered_map<int, std::shared_ptr<System::ProcessInfo> > &System::processIdMap() const {
    return mProcessIdMap;
}

inline const std::unordered_map<std::string, std::shared_ptr<System::ProcessInfo> > &System::processNameMap() const {
    return mProcessNameMap;
}

#endif // SYSINFO_H
