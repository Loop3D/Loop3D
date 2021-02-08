#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractListModel>
#include "eventlist.h"

//class EventList;

class EventModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(EventList *events READ getEvents WRITE setEvents NOTIFY eventsChanged)

public:
    explicit EventModel(QObject *parent = nullptr);

    enum {
        nameRole = Qt::UserRole,
        isActiveRole,
        minAgeRole,
        maxAgeRole,
        eventIDRole,
        rankRole,
        typeRole,
        avgDisplacementRole,
        periodicRole,
        wavelengthRole,
        amplitudeRole,
        asymmetryRole,
        asymmetryShiftRole,
        secondaryWavelengthRole,
        secondaryAmplitudeRole,
        lowerScalarValueRole,
        upperScalarValueRole,
        scalarValueRole,
        thicknessRole,
        colour1RedRole,
        colour1GreenRole,
        colour1BlueRole,
        colour2RedRole,
        colour2GreenRole,
        colour2BlueRole,
        permutation
    };

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant dataIndexed(int index, QString role) const;
    Q_INVOKABLE int countPBlocks(void) const;
    Q_INVOKABLE QVariant pBlockDataIndexed(int index, QString role) const;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Q_INVOKABLE bool setDataIndexed(int index, const QVariant &value, QString role);

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE int lookupRoleName(QString name) const;

    EventList *getEvents() const;
    void setEvents(EventList *value);
    Q_INVOKABLE LoopProjectFile::Event get(int index) const;
    Q_INVOKABLE void sortEvents(void);
    Q_INVOKABLE int findEventByID(int eventID);
    Q_INVOKABLE void refreshModel(void);

//    Q_INVOKABLE void loadEvents(QVariant filename);

Q_SIGNALS:
    void eventsChanged();

private:
    EventList *events;
};

#endif // EVENTMODEL_H
