#ifndef PROCESSMODEL_H
#define PROCESSMODEL_H

#include <QAbstractTableModel>
#include <string>
#include <vector>
#include <unordered_map>

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

    void refresh();

    bool search(int pid);
    bool search(std::string name);

private slots:
    void update();

private:
    enum {
        Name,
        User,
        State,
        PID,
        PPID,
        CPUPercent,
        MemPercent,
        Priority,
        Nice,
        ThreadNum
    };

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
        long long priority;
        long long nice;
        long long threadNum;
    };

    int sortedColumn;
    std::vector<int> showList; //process to show
    std::unordered_map<int, ProcessInfo> pidMap;
    std::unordered_multimap<std::string, int> nameMap;
    QTimer *pTimer;
};

#endif // PROCESSMODEL_H
