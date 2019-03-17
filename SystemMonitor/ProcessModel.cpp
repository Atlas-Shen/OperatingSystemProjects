#include "ProcessModel.h"

ProcessModel::ProcessModel(QObject *parent)
    : QAbstractTableModel(parent) {
}

QVariant ProcessModel::headerData(int section, Qt::Orientation orientation, int role) const {
    // FIXME: Implement me!
}

int ProcessModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int ProcessModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
}

QVariant ProcessModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}
