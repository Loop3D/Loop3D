#ifndef DATASOURCEMODEL_H
#define DATASOURCEMODEL_H

#include <QAbstractListModel>
#include "projectmanagement.h"

class DataSourceList;

class DataSourceModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(DataSourceList *dataSources READ getDataSources WRITE setDataSources)

public:
    explicit DataSourceModel(QObject *parent = nullptr);

    enum {
        nameRole = Qt::UserRole,
        groupRole,
        typeRole,
        isParentRole,
        isExpandedRole,
        selectedRole,
        urlRole
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

//    // Add data:
//    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

//    // Remove data:
//    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    virtual QHash<int, QByteArray> roleNames() const override;

    DataSourceList *getDataSources() const;
    void setDataSources(DataSourceList *value);

    Q_INVOKABLE void expandGroup(QVariant group);
    Q_INVOKABLE void loadDataSources(QVariant filename);
    Q_INVOKABLE void selectItem(int index, bool value);
    Q_INVOKABLE void selectItemById(QString id, bool value);

private:
    DataSourceList *dataSources;
};

#endif // DATASOURCEMODEL_H
