#ifndef SYSINFO_H
#define SYSINFO_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTimer>
#include <QStringList>
#include <fstream>
#include <string>
#include <array>

class System : public QObject {
    Q_OBJECT

public:
    static System &instance();

    QString hostname();
    QString version();
    QString startingTime() const;
    QString lastingTime() const;
    double cpuUsage() const;
    double memUsage() const;
    double swapUsage() const;
    const QStringList &cpuInfo() const;

signals:
    void toMainWindow();
    void toStatusWidget();
    void toResourceWidget();

private:
    std::ifstream file;
    QDateTime mStartingTime;
    QTimer timer;
    std::array<unsigned long long, 2> prevCpuRawData;
    std::array<unsigned long long, 2> curCpuRawData;
    double mCpuUsage;
    double mMemUsage;
    double mSwapUsage;
    QStringList mCpuInfo;

    void update();
    void setCpuUsage();
    void setMemSwapUsage();
    void setCpuInfo();

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
    return QString::number(mStartingTime.secsTo(QDateTime::currentDateTime())) + 's';
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

inline const QStringList &System::cpuInfo() const {
    return mCpuInfo;
}

#endif // SYSINFO_H
