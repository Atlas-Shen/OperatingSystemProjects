#ifndef SYSINFO_H
#define SYSINFO_H

#include <QObject>
#include <fstream>
#include <QString>
#include <QDateTime>
#include <QTimer>

class SysInfo : public QObject {
    Q_OBJECT

public:
    static SysInfo &instance();

    QString hostname();
    QString systemVer();
    QString cpuInfo();
    QString startingTime();
    QString lastingTime();

    double cpuUsage() const;
    double memUsage() const;
    double swapUsage() const;

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

    void update();
    void setCpuUsage();
    void setMemSwapUsage();

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

inline QString SysInfo::startingTime() {
    return mStartingTime.toString("yyyy-MM-dd ddd hh:mm:ss");
}

inline QString SysInfo::lastingTime() {
    return QString::number(mStartingTime.secsTo(QDateTime::currentDateTime()) + 's');
}

inline double SysInfo::cpuUsage() const {
    return mCpuUsage;
}

inline double SysInfo::memUsage() const {
    return mMemUsage;
}

inline double SysInfo::swapUsage() const {
    return mSwapUsage;
}

#endif // SYSINFO_H
