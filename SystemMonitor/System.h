#ifndef SYSINFO_H
#define SYSINFO_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <fstream>
#include <string>
#include <array>

class System : public QObject {
    Q_OBJECT

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

#endif // SYSINFO_H
