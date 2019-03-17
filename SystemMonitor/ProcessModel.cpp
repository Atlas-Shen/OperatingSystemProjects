#include "ProcessModel.h"
#include "System.h"

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent) {
    connect(&System::instance(), &System::toProcessModel, this, &ProcessModel::update);
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
    return static_cast<int>(System::instance().processIdMap().size());
}

int ProcessModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    //name user state pid ppid cpu% mem% size resident shared priority nice threadNum startTime
    return 6;
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const {
    int pid = System::instance().processIdList().at(static_cast<size_t>(index.row()));
    auto pointer = System::instance().processIdMap().at(pid);

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
        /*case Qt::FontRole:
            if (row == 0 && col == 0) { //change font only for cell(0,0)
                QFont boldFont;
                boldFont.setBold(true);
                return boldFont;
            }/
            break;
        case Qt::BackgroundRole:
            //if (row == 1 && col == 2)  //change background only for cell(1,2)
            //return QBrush(Qt::red);
            break;
        case Qt::TextAlignmentRole:
            //if (row == 1 && col == 1) //change text alignment only for cell(1,1)
            //return Qt::AlignRight + Qt::AlignVCenter;
            break;
        case Qt::CheckStateRole:
            //if (row == 1 && col == 0) //add a checkbox to cell(1,0)
            //return Qt::Checked;
            break;*/
    }
    return QVariant();
}

void ProcessModel::update() {
    emit dataChanged(index(0, 0, QModelIndex()), index(rowCount(), columnCount(), QModelIndex()), {Qt::DisplayRole});
}
