#include "datasourcemodel.h"
#include "datasourcelist.h"
#include <iostream>
#include <QDebug>

DataSourceModel::DataSourceModel(QObject *parent)
    : QAbstractListModel(parent)
    , dataSources(nullptr)
{
}

int DataSourceModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !dataSources)
        return 0;

    return dataSources->getDataSources().size();
}

QVariant DataSourceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() | !dataSources)
        return QVariant();

    const DataSourceItem item = dataSources->getDataSources().at(index.row());
    switch(role) {
    case nameRole:
        return QVariant(item.name);
    case groupRole:
        return QVariant(item.group);
    case isParentRole:
        return QVariant(item.isParent);
    case isExpandedRole:
        return QVariant(item.isExpanded);
    case dlStateRole:
        return QVariant(item.dlState);
    default:
        return QVariant(false);
    }
}

bool DataSourceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!dataSources) return false;

    DataSourceItem item = dataSources->getDataSources().at(index.row());
    switch (role) {
    case nameRole:
        item.name = value.toString();
        break;
    case groupRole:
        item.group = value.toString();
        break;
    case isParentRole:
        item.isParent = value.toBool();
        break;
    case isExpandedRole:
        item.isExpanded = value.toBool();
        break;
    case dlStateRole:
        if (value.toString() == "downloading") item.startDownload();
        item.dlState = value.toString();
        break;
    }

    if (dataSources->setDataSourceAt(index.row(), item)) {
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags DataSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() | !dataSources)
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

//bool DataSourceModel::insertRows(int row, int count, const QModelIndex &parent)
//{
//    beginInsertRows(parent, row, row + count - 1);
//    // FIXME: Implement me!
//    endInsertRows();
//}

//bool DataSourceModel::removeRows(int row, int count, const QModelIndex &parent)
//{
//    beginRemoveRows(parent, row, row + count - 1);
//    // FIXME: Implement me!
//    return 1;
//}

QHash<int, QByteArray> DataSourceModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role[isParentRole] = "isParent";
    role[isExpandedRole] = "isExpanded";
    role[groupRole] = "group";
    role[nameRole] = "name";
    role[dlStateRole] = "dlState";

    return role;
}

void DataSourceModel::setDataSources(DataSourceList *value)
{
    beginResetModel();

    if (dataSources) dataSources->disconnect(this);

    dataSources = value;

    if (dataSources) {
        connect(dataSources, &DataSourceList::preItemAppended, this, [this]() {
            const int index = dataSources->getDataSources().size();
            beginInsertRows(QModelIndex(), index, index);
        });
        connect(dataSources, &DataSourceList::postItemAppended, this, [this]() {
            endInsertRows();
        });
        connect(dataSources, &DataSourceList::preItemRemoved, this, [this](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(dataSources, &DataSourceList::postItemRemoved, this, [this]() {
            endRemoveRows();
        });
    }

    endResetModel();
}

void DataSourceModel::expandGroup(QVariant group)
{
    QString comp = group.toString();
    if (comp == "") return;

    for (int i=0;i<dataSources->getDataSources().size();i++) {
        DataSourceItem item = dataSources->getDataSources().at(i);
        if (item.group == comp) {
            item.isExpanded = !item.isExpanded;
            if (dataSources->setDataSourceAt(i, item)) {
                QModelIndex index = this->index(i,0);
                Q_EMIT dataChanged(index, index, QVector<int>() << isExpandedRole);
            }
        }
    }
}

void DataSourceModel::loadDataSources(QVariant filename)
{
    beginResetModel();
    if(dataSources) {
        dataSources->loadDataSources(filename.toString());
    } else {
        qDebug() << "No List of data sources to load data into!";
    }
    endResetModel();
}

DataSourceList *DataSourceModel::getDataSources() const
{
    return dataSources;
}
