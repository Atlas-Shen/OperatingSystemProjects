#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractTableModel>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

class QTimer;

class ProcessModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit ProcessModel(QObject *parent = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void sort(int column, Qt::SortOrder order = Qt::DescendingOrder) override;

public slots:
    void update();

private:
    struct ProcessInfo {
        std::string name;
        std::string user;
        std::string state;
        int pid;
        int ppid;
        unsigned long long prevCpuTime;
        unsigned long long prevCpuTotal;
        double cpuPercent;
        double memPercent;
        unsigned long size;     //in KB
        unsigned long resident; //in KB
        unsigned long shared;   //in KB
        long priority;
        long nice;
        long threadNum;
        unsigned long long startTime;
    };

    std::vector<int> mProcessIdList;
    std::unordered_map<int, std::shared_ptr<ProcessInfo>> mProcessIdMap;
    std::unordered_map<std::string, std::shared_ptr<ProcessInfo>> mProcessNameMap;
    QTimer *pTimer;
};

#endif // PROCESSMODEL_H
