#include "observationmodel.h"
#include "observationlist.h"

ObservationModel::ObservationModel(QObject *parent)
    : QAbstractListModel(parent)
    , observations(nullptr)
{

}

int ObservationModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !observations)
        return 0;

    return observations->getObservations().size();
}

QVariant ObservationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() | !observations)
        return QVariant();

    const LoopProjectFile::Observation item = observations->getObservations().at(index.row());
    switch(role) {
    case eastingRole:
        return QVariant(item.easting);
    case northingRole:
        return QVariant(item.northing);
    case altitudeRole:
        return QVariant(item.altitude);
    case eventIDRole:
        return QVariant(item.eventId);
    case typeRole:
        return QVariant(item.type);
    default:
        return QVariant(false);
    }
}

QVariant ObservationModel::dataIndexed(int index, QString role) const
{
    if (index < 0 || index > observations->getObservations().size() ) return QVariant();
    return data(this->index(index,0),lookupRoleName(role));
}

bool ObservationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!observations) return false;

    LoopProjectFile::Observation item = observations->getObservations().at(index.row());
    switch (role) {
    case eastingRole:
        item.easting = value.toDouble();
        break;
    case northingRole:
        item.northing = value.toDouble();
        break;
    case altitudeRole:
        item.altitude = value.toDouble();
        break;
    case eventIDRole:
        item.eventId = value.toInt();
        break;
    case typeRole:
        if (value == "fault") item.type = LoopProjectFile::FAULTOBSERVATION;
        else if (value == "fold") item.type = LoopProjectFile::FOLDOBSERVATION;
        else if (value == "foliation") item.type = LoopProjectFile::FOLIATIONOBSERVATION;
        else if (value == "discontinuity") item.type = LoopProjectFile::DISCONTINUITYOBSERVATION;
        else item.type = LoopProjectFile::INVALIDOBSERVATION;
        break;
    }

    if (observations->setObservationAt(index.row(), item)) {
        Q_EMIT dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

bool ObservationModel::setDataIndexed(int index, const QVariant &value, QString role)
{
    if (index < 0 || index > observations->getObservations().size() ) return false;
    return setData(this->index(index,0),value,lookupRoleName(role));
}

Qt::ItemFlags ObservationModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() | !observations)
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> ObservationModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role[eastingRole] = "easting";
    role[northingRole] = "northing";
    role[altitudeRole] = "altitude";
    role[eventIDRole] = "eventID";
    role[typeRole] = "type";

    return role;
}

int ObservationModel::lookupRoleName(QString name) const
{
    if (name == "easting") return eastingRole;
    if (name == "northing") return northingRole;
    if (name == "altitude") return altitudeRole;
    if (name == "eventID") return eventIDRole;
    if (name == "type") return typeRole;
    return 0;

}

void ObservationModel::setObservations(ObservationList *value)
{
    beginResetModel();

    if (observations) observations->disconnect(this);

    observations = value;

    if (observations) {
        connect(observations, &ObservationList::preItemAppended, this, [this]() {
            const int index = observations->getObservations().size();
            beginInsertRows(QModelIndex(), index, index);
        });
        connect(observations, &ObservationList::postItemAppended, this, [this]() {
            endInsertRows();
        });
        connect(observations, &ObservationList::preItemRemoved, this, [this](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(observations, &ObservationList::postItemRemoved, this, [this]() {
            endRemoveRows();
        });
    }

    endResetModel();
}

ObservationList *ObservationModel::getObservations() const
{
    return observations;
}

LoopProjectFile::Observation ObservationModel::get(int index) const
{
    LoopProjectFile::Observation res;
    if (!observations || index <0 || index > observations->getObservations().size())
        return res;
    return (observations->getObservations()[index]);
}

int ObservationModel::findObservationByID(int eventId)
{
    if (!observations || observations->getObservations().size() <=0) return -1;
    for (int i=0;i<observations->getObservations().size();i++) {
        if (observations->getObservations().at(i).eventId == eventId) return i;
    }
    return -1;
}

