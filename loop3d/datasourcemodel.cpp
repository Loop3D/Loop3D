#include "datasourcemodel.h"
#include "datasourcelist.h"
#include "tokenise.h"
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
    case selectedRole:
        return QVariant(item.selected);
    case urlRole:
        return QVariant(item.url);
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
    case selectedRole:
        if (value.toString() == "downloading") item.startDownload();
        item.selected = value.toBool();
        break;
    case urlRole:
        item.url= value.toString();
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
    role[selectedRole] = "selected";
    role[urlRole] = "url";

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

void DataSourceModel::selectItem(int index, bool value)
{
    if (!dataSources || index >= dataSources->getDataSources().size()) return;

    DataSourceItem reference = dataSources->getDataSources().at(index);

    // If this item is a CDS now selectItems on all ids that are in the CDS's url
    if (reference.format == "CDS") {
        // tokenise the ids from the url
        std::vector<std::string> tokens;
        StringTokenise::tokenise(reference.url.toStdString().c_str(),tokens," ,");
        // find the list element that matches the id
        for (auto it=tokens.begin();it!=tokens.end();it++) {
            selectItemById(QString((*it).c_str()), value);
        }
    } else {
        for (int i=0;i<dataSources->getDataSources().size();i++) {
            DataSourceItem item = dataSources->getDataSources().at(i);
            if (item.format == "CDS") {
                item.selected = false;
                if (dataSources->setDataSourceAt(index, reference)) {
                    QModelIndex ind = this->index(i,0);
                    Q_EMIT dataChanged(ind, ind, QVector<int>() << selectedRole);
                }
            }
        }
    }

    if (reference.selected == false) {
        for (int i=0;i<dataSources->getDataSources().size();i++) {
            DataSourceItem item = dataSources->getDataSources().at(i);
            if (item.group == reference.group) {
                item.selected = false;
                if (dataSources->setDataSourceAt(i, item)) {
                    QModelIndex ind = this->index(i,0);
                    Q_EMIT dataChanged(ind, ind, QVector<int>() << selectedRole);
                }
            }
        }
    }
    reference.selected = value;
    if (ProjectManagement::instance()) {
        if (reference.group == "STRUCT")   ProjectManagement::instance()->m_structureUrl = value ? reference.url.toStdString() : "";
        if (reference.group == "GEOL")     ProjectManagement::instance()->m_geologyUrl   = value ? reference.url.toStdString() : "";
        if (reference.group == "FAULT")    ProjectManagement::instance()->m_faultUrl     = value ? reference.url.toStdString() : "";
        if (reference.group == "FOLD")     ProjectManagement::instance()->m_foldUrl      = value ? reference.url.toStdString() : "";
        if (reference.group == "MINDEP")   ProjectManagement::instance()->m_mindepUrl    = value ? reference.url.toStdString() : "";
        if (reference.group == "METADATA") ProjectManagement::instance()->m_metadataUrl  = value ? reference.url.toStdString() : "";
    }
    if (dataSources->setDataSourceAt(index, reference)) {
        QModelIndex ind = this->index(index,0);
        Q_EMIT dataChanged(ind, ind, QVector<int>() << selectedRole);
    }
}

void DataSourceModel::selectItemById(QString id, bool value)
{
    for (int i=0;i<dataSources->getDataSources().size();i++) {
        DataSourceItem item = dataSources->getDataSources().at(i);
        if (item.id == id) {
            // call selectItem on that element
            selectItem(i, value);
            break;
        }
    }
}

DataSourceList *DataSourceModel::getDataSources() const
{
    return dataSources;
}
