#ifndef OBSERVATIONMODEL_H
#define OBSERVATIONMODEL_H

#include <QAbstractListModel>
#include "observationlist.h"

//class ObservationList;

class ObservationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ObservationList *observations READ getObservations WRITE setObservations NOTIFY observationsChanged)

public:
    explicit ObservationModel(QObject *parent = nullptr);

    enum {
        eastingRole = Qt::UserRole,
        northingRole,
        altitudeRole,
        eventIDRole,
        typeRole,
        dipDirRole,
        dipRole,
        dipPolarityRole,
        valRole,
        displacementRole,
        axisXRole,
        axisYRole,
        axisZRole,
        foliationRole,
        whatIsFoldedRole,
        layerRole,
        permutation
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant dataIndexed(int index, QString role) const;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool setDataIndexed(int index, const QVariant &value, QString role);

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int lookupRoleName(QString name) const;

    ObservationList *getObservations() const;
    void setObservations(ObservationList *value);
    Q_INVOKABLE LoopProjectFile::Observation get(int index) const;
    Q_INVOKABLE int findObservationByID(int eventID);
    Q_INVOKABLE void refreshModel(void);

Q_SIGNALS:
    void observationsChanged();

private:
    ObservationList *observations;
};

#endif // OBSERVATIONMODEL_H
