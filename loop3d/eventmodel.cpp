#include "eventmodel.h"
#include "eventlist.h"

EventModel::EventModel(QObject *parent)
    : QAbstractListModel(parent)
    , events(nullptr)
{

}

int EventModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !events)
        return 0;

    return events->getEvents().size();
}

QVariant EventModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() | !events)
        return QVariant();

    const EventItem item = events->getEvents().at(index.row());
    switch(role) {
    case nameRole:
        return QVariant(item.name);
    case isActiveRole:
        return QVariant(item.isActive);
    case minAgeRole:
        return QVariant(item.minAge);
    case maxAgeRole:
        return QVariant(item.maxAge);
    case eventIDRole:
        return QVariant(item.eventID);
    case rankRole:
        return QVariant(item.rank);
    case typeRole:
        return QVariant(item.type);
    default:
        return QVariant(false);
    }
}

QVariant EventModel::dataIndexed(int index, QString role) const
{
    if (index < 0 || index > events->getEvents().size() ) return QVariant();
    return data(this->index(index,0),lookupRoleName(role));
}

bool EventModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!events) return false;

    EventItem item = events->getEvents().at(index.row());
    switch (role) {
    case nameRole:
        item.name = value.toString();
        break;
    case isActiveRole:
        item.isActive = value.toBool();
        break;
    case minAgeRole:
        item.minAge = value.toFloat();
        break;
    case maxAgeRole:
        item.maxAge = value.toFloat();
        break;
    case eventIDRole:
        item.eventID = value.toInt();
        break;
    case rankRole:
        item.rank = value.toInt();
        break;
    case typeRole:
        item.type = value.toString();
        break;
    }
    if (item.minAge > item.maxAge) {
        float tmp = item.minAge;
        item.minAge = item.maxAge;
        item.maxAge = tmp;
    }

    if (events->setEventAt(index.row(), item)) {
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

bool EventModel::setDataIndexed(int index, const QVariant &value, QString role)
{
    if (index < 0 || index > events->getEvents().size() ) return false;
    return setData(this->index(index,0),value,lookupRoleName(role));
}

Qt::ItemFlags EventModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() | !events)
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role[nameRole] = "name";
    role[isActiveRole] = "isActive";
    role[minAgeRole] = "minAge";
    role[maxAgeRole] = "maxAge";
    role[eventIDRole] = "eventID";
    role[rankRole] = "rank";
    role[typeRole] = "type";

    return role;
}

int EventModel::lookupRoleName(QString name) const
{
    if (name == "name") return nameRole;
    if (name == "isActive") return isActiveRole;
    if (name == "minAge") return minAgeRole;
    if (name == "maxAge") return maxAgeRole;
    if (name == "eventID") return eventIDRole;
    if (name == "rank") return rankRole;
    if (name == "type") return typeRole;
    return 0;

}

void EventModel::setEvents(EventList *value)
{
    beginResetModel();

    if (events) events->disconnect(this);

    events = value;

    if (events) {
        connect(events, &EventList::preItemAppended, this, [this]() {
            const int index = events->getEvents().size();
            beginInsertRows(QModelIndex(), index, index);
        });
        connect(events, &EventList::postItemAppended, this, [this]() {
            endInsertRows();
        });
        connect(events, &EventList::preItemRemoved, this, [this](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(events, &EventList::postItemRemoved, this, [this]() {
            endRemoveRows();
        });
    }

    endResetModel();
}

EventList *EventModel::getEvents() const
{
    return events;
}

EventItem EventModel::get(int index) const
{
    EventItem res;
    if (!events || index <0 || index > events->getEvents().size())
        return res;
    return (events->getEvents()[index]);
}

void EventModel::sortEvents()
{
    events->sort();
    for (int i=0;i<events->getEvents().size();i++)
        Q_EMIT dataChanged(index(i),index(i));
}

int EventModel::findEventByID(int eventID)
{
    if (!events || events->getEvents().size() <=0) return -1;
    for (int i=0;i<events->getEvents().size();i++) {
        if (events->getEvents().at(i).eventID == eventID) return i;
    }
    return -1;
}


int EventModel::countPBlocks(void) const
{
    return events->getPermutationBlocks().size();
}

QVariant EventModel::pBlockDataIndexed(int index, QString role) const
{
    if (index < 0 || index > events->getPermutationBlocks().size() ) return QVariant();
    const PermutationBlock item = events->getPermutationBlocks().at(index);
    if (role == "minAge") {
        return QVariant(item.minAge);
    } else if (role == "maxAge") {
        return QVariant(item.maxAge);
    } else if (role == "permutations") {
        return QVariant(item.permutations);
    } else if (role == "maxRank") {
        return QVariant(item.maxRank);
    } else return QVariant();
}
