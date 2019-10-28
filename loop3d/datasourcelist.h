#ifndef DATASOURCELIST_H
#define DATASOURCELIST_H
#include <QString>
#include <QAbstractListModel>
#include <QVector>

struct DataSourceItem {
    QString name;
    bool isExpanded;
    bool isParent;
    QString group;
    QString dlState;
    double extents[4];
    double stride;
    QString url;
    QString format;
    void startDownload(void);
};

class DataSourceList : public QObject {
    Q_OBJECT
    public:
        DataSourceList(QObject* parent = nullptr);
        ~DataSourceList() { dataSources.clear(); }

        bool setDataSourceAt(int index, const DataSourceItem& dataSource);
        void loadDataSources(QString resourceFile);

        QVector<DataSourceItem> getDataSources() const;

        void expandGroup(int index);

Q_SIGNALS:
        void preItemAppended();
        void postItemAppended();

        void preItemRemoved(int index);
        void postItemRemoved();

    public Q_SLOTS:
        bool appendItem(QString name, QString group, QString url, QString format="");
        bool appendGroup(QString name, QString group);
        bool removeItem(int index);

    private:
        QString resourceFilename;
        QVector<DataSourceItem> dataSources;
        static bool order(const DataSourceItem& a, const DataSourceItem& b)
        {   return (a.group > b.group && a.isParent > b.isParent); }
};

#endif // DATASOURCELIST_H
