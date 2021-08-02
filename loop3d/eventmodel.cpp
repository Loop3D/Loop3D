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

    std::shared_ptr<LoopProjectFile::Event> item = events->getEvents().at(index.row());
    switch(role) {
    case nameRole:
        return QVariant(item->name);
    case isActiveRole:
        return QVariant(item->enabled);
    case minAgeRole:
        return QVariant(item->minAge);
    case maxAgeRole:
        return QVariant(item->maxAge);
    case eventIDRole:
        return QVariant(item->eventId);
    case rankRole:
        return QVariant(item->rank);
    case typeRole:
        return QVariant(item->type);
    }
    switch (item->type) {
        case LoopProjectFile::FAULTEVENT: {
            LoopProjectFile::FaultEvent* fault_item = (LoopProjectFile::FaultEvent*)(item.get());
            switch (role) {
                case avgDisplacementRole:
                    return QVariant(fault_item->avgDisplacement);
            }
        }
        case LoopProjectFile::FOLDEVENT: {
            LoopProjectFile::FoldEvent* fold_item = (LoopProjectFile::FoldEvent*)(item.get());
            switch (role) {
                case periodicRole:
                    return QVariant(fold_item->periodic);
                case wavelengthRole:
                    return QVariant(fold_item->wavelength);
                case amplitudeRole:
                    return QVariant(fold_item->amplitude);
                case asymmetryRole:
                    return QVariant(fold_item->asymmetry);
                case asymmetryShiftRole:
                    return QVariant(fold_item->asymmetryShift);
                case secondaryWavelengthRole:
                    return QVariant(fold_item->secondaryWavelength);
                case secondaryAmplitudeRole:
                    return QVariant(fold_item->secondaryAmplitude);
            }
        }
        case LoopProjectFile::FOLIATIONEVENT: {
            LoopProjectFile::FoliationEvent* foliation_item = (LoopProjectFile::FoliationEvent*)(item.get());
            switch (role) {
                case lowerScalarValueRole:
                    return QVariant(foliation_item->lowerScalarValue);
            case upperScalarValueRole:
                return QVariant(foliation_item->upperScalarValue);
            }
        }
        case LoopProjectFile::DISCONTINUITYEVENT: {
            LoopProjectFile::DiscontinuityEvent* discontinuity_item = (LoopProjectFile::DiscontinuityEvent*)(item.get());
            switch (role) {
                case scalarValueRole:
                    return QVariant(discontinuity_item->scalarValue);
            }
        }
        case LoopProjectFile::STRATIGRAPHICLAYER: {
            LoopProjectFile::StratigraphicLayer* stratigraphic_item = (LoopProjectFile::StratigraphicLayer*)(item.get());
            switch (role) {
                case thicknessRole:
                    return QVariant(stratigraphic_item->thickness);
                case colour1RedRole:
                    return QVariant(stratigraphic_item->colour1Red);
                case colour1GreenRole:
                    return QVariant(stratigraphic_item->colour1Green);
                case colour1BlueRole:
                    return QVariant(stratigraphic_item->colour1Blue);
                case colour2RedRole:
                    return QVariant(stratigraphic_item->colour2Red);
                case colour2GreenRole:
                    return QVariant(stratigraphic_item->colour2Green);
                case colour2BlueRole:
                    return QVariant(stratigraphic_item->colour2Blue);
            }
        }
        default: break;
    }

    return QVariant(false);
}

QVariant EventModel::dataIndexed(int index, QString role) const
{
    if (index < 0 || index > events->getEvents().size() ) return QVariant();
    return data(this->index(index,0),lookupRoleName(role));
}

bool EventModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!events) return false;

    std::shared_ptr<LoopProjectFile::Event> item = events->getEvents().at(index.row());
    switch (role) {
    case nameRole:
        strncpy(item->name,value.toString().toStdString().c_str(),30);
        break;
    case isActiveRole:
        item->enabled = value.toBool();
        break;
    case minAgeRole:
        item->minAge = value.toDouble();
        break;
    case maxAgeRole:
        item->maxAge = value.toDouble();
        break;
    case eventIDRole:
        item->eventId = value.toInt();
        break;
    case rankRole:
        item->rank = value.toInt();
        break;
    case typeRole:
        // TODO: when changing type via the GUI need to remove old item, create new and refile
        // Currently forbid this
        std::cout << "Changing of type individually is a very bad idea, currently not implimented!!" << std::endl;
//        if (value == "fault") item->type = LoopProjectFile::FAULTEVENT;
//        else if (value == "fold") item->type = LoopProjectFile::FOLDEVENT;
//        else if (value == "foliation") item->type = LoopProjectFile::FOLIATIONEVENT;
//        else if (value == "discontinuity") item->type = LoopProjectFile::DISCONTINUITYEVENT;
//        else if (value == "stratigraphic") item->type = LoopProjectFile::STRATIGRAPHICLAYER;
//        else item->type = LoopProjectFile::INVALIDEVENT;
        break;
    }
    switch (item->type) {
        case LoopProjectFile::FAULTEVENT: {
            LoopProjectFile::FaultEvent* fault_item = (LoopProjectFile::FaultEvent*)(item.get());
            switch (role) {
                case avgDisplacementRole:
                    fault_item->avgDisplacement = value.toDouble();
                    break;
            }
        }
        break;
        case LoopProjectFile::FOLDEVENT: {
            LoopProjectFile::FoldEvent* fold_item = (LoopProjectFile::FoldEvent*)(item.get());
            switch (role) {
                case periodicRole:
                    fold_item->periodic = value.toInt();
                    break;
                case wavelengthRole:
                    fold_item->wavelength = value.toDouble();
                    break;
                case amplitudeRole:
                    fold_item->amplitude = value.toDouble();
                    break;
                case asymmetryRole:
                    fold_item->asymmetry = value.toInt();
                    break;
                case asymmetryShiftRole:
                    fold_item->asymmetryShift = value.toDouble();
                    break;
                case secondaryWavelengthRole:
                    fold_item->secondaryWavelength = value.toDouble();
                    break;
                case secondaryAmplitudeRole:
                    fold_item->secondaryAmplitude = value.toDouble();
                    break;
            }
            break;
        }
        case LoopProjectFile::FOLIATIONEVENT: {
            LoopProjectFile::FoliationEvent* foliation_item = (LoopProjectFile::FoliationEvent*)(item.get());
            switch (role) {
                case lowerScalarValueRole:
                    foliation_item->lowerScalarValue = value.toDouble();
                    break;
                case upperScalarValueRole:
                    foliation_item->upperScalarValue = value.toDouble();
                    break;
            }
            break;
        }
        case LoopProjectFile::DISCONTINUITYEVENT: {
            LoopProjectFile::DiscontinuityEvent* discontinuity_item = (LoopProjectFile::DiscontinuityEvent*)(item.get());
            switch (role) {
                case scalarValueRole:
                    discontinuity_item->scalarValue = value.toDouble();
                    break;
            }
            break;
        }
        case LoopProjectFile::STRATIGRAPHICLAYER: {
            LoopProjectFile::StratigraphicLayer* stratigraphic_item = (LoopProjectFile::StratigraphicLayer*)(item.get());
            switch (role) {
                case thicknessRole:
                    stratigraphic_item->thickness = value.toDouble();
                    break;
                case colour1RedRole:
                    stratigraphic_item->colour1Red = value.toInt();
                    break;
                case colour1GreenRole:
                    stratigraphic_item->colour1Green = value.toInt();
                    break;
                case colour1BlueRole:
                    stratigraphic_item->colour1Blue = value.toInt();
                    break;
                case colour2RedRole:
                    stratigraphic_item->colour2Red = value.toInt();
                    break;
                case colour2GreenRole:
                    stratigraphic_item->colour2Green = value.toInt();
                    break;
                case colour2BlueRole:
                    stratigraphic_item->colour2Blue = value.toInt();
                    break;
            }
            break;
        }
    default: break;
    }

    if (item->minAge > item->maxAge) {
        double tmp = item->minAge;
        item->minAge = item->maxAge;
        item->maxAge = tmp;
    }

    Q_EMIT dataChanged(index, index, QVector<int>() << role);
    return true;
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
    role[avgDisplacementRole] = "avgDisplacement";
    role[periodicRole] = "periodic";
    role[wavelengthRole] = "wavelength";
    role[amplitudeRole] = "amplitude";
    role[asymmetryRole] = "asymmetry";
    role[asymmetryShiftRole] = "asymmetryShift";
    role[secondaryWavelengthRole] = "secondaryWavelength";
    role[secondaryAmplitudeRole] = "secondaryAmplitude";
    role[lowerScalarValueRole] = "lowerScalarValue";
    role[upperScalarValueRole] = "upperScalarValue";
    role[scalarValueRole] = "scalarValue";
    role[thicknessRole] = "thickness";
    role[colour1RedRole] = "colour1Red";
    role[colour1GreenRole] = "colour1Green";
    role[colour1BlueRole] = "colour1Blue";
    role[colour2RedRole] = "colour2Red";
    role[colour2GreenRole] = "colour2Green";
    role[colour2BlueRole] = "colour2Blue";

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
    if (name == "avgDisplacement") return avgDisplacementRole;
    if (name == "periodic") return periodicRole;
    if (name == "wavelength") return wavelengthRole;
    if (name == "amplitude") return amplitudeRole;
    if (name == "asymmetry") return asymmetryRole;
    if (name == "asymmetryShift") return asymmetryShiftRole;
    if (name == "secondaryWavelength") return secondaryWavelengthRole;
    if (name == "secondaryAmplitude") return secondaryAmplitudeRole;
    if (name == "lowerScalarValue") return lowerScalarValueRole;
    if (name == "upperScalarValue") return upperScalarValueRole;
    if (name == "scalarValue") return scalarValueRole;
    if (name == "thickness") return thicknessRole;
    if (name == "colour1Red") return colour1RedRole;
    if (name == "colour1Green") return colour1GreenRole;
    if (name == "colour1Blue") return colour1BlueRole;
    if (name == "colour2Red") return colour2RedRole;
    if (name == "colour2Green") return colour2GreenRole;
    if (name == "colour2Blue") return colour2BlueRole;
    return 0;

}

void EventModel::setEvents(EventList *value)
{
    beginResetModel();

    if (events) events->disconnect(this);

    events = value;

    if (events) {
        connect(events, &EventList::preItemAppended, this, [this](int start, int count) {
            beginInsertRows(QModelIndex(), start, start+count-1);
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
        connect(events, &EventList::preItemReset, this, [this]() {
            beginResetModel();
        });
        connect(events, &EventList::postItemReset, this, [this]() {
            endResetModel();
        });
    }

    endResetModel();
}

EventList *EventModel::getEvents() const
{
    return events;
}

LoopProjectFile::Event EventModel::get(int index) const
{
    LoopProjectFile::Event res;
    if (!events || index < 0 || index > events->getEvents().size())
        return res;
    return (*(events->getEvents()[index]));
}

void EventModel::sortEvents()
{
    events->sort();
    refreshModel();
}

int EventModel::findEventByID(int eventId)
{
    if (!events || events->getEvents().size() <=0) return -1;
    for (int i=0;i<events->getEvents().size();i++) {
        if (events->getEvents().at(i)->eventId == eventId) return i;
    }
    return -1;
}

void EventModel::refreshModel()
{
    beginResetModel();
    endResetModel();
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


/* ******************* EventRelationshipList area ****************** */

EventRelationshipModel::EventRelationshipModel(QObject *parent)
    : QAbstractListModel(parent)
    , relationships(nullptr)
{

}

int EventRelationshipModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid() || !relationships)
        return 0;

    return relationships->getRelationships().size();
}

QVariant EventRelationshipModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() | !relationships)
        return QVariant();

    std::shared_ptr<LoopProjectFile::EventRelationship> item = relationships->getRelationships().at(index.row());
    switch(role) {
        case eventID1Role:
            return QVariant(item->eventId1);
        case eventID2Role:
            return QVariant(item->eventId2);
        case bidirectionalRole:
            return QVariant(item->bidirectional);
        default: break;
    }

    return QVariant(false);
}

QVariant EventRelationshipModel::dataIndexed(int index, QString role) const
{
    if (index < 0 || index > relationships->getRelationships().size() ) return QVariant();
    return data(this->index(index,0),lookupRoleName(role));
}

bool EventRelationshipModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!relationships) return false;

    std::shared_ptr<LoopProjectFile::EventRelationship> item = relationships->getRelationships().at(index.row());
    switch (role) {
        case eventID1Role:
            item->eventId1 = value.toInt();
            break;
        case eventID2Role:
            item->eventId2 = value.toInt();
            break;
        case bidirectionalRole:
            item->bidirectional = value.toBool();
            break;
        default: break;
    }

    Q_EMIT dataChanged(index, index, QVector<int>() << role);
    return true;
}

bool EventRelationshipModel::setDataIndexed(int index, const QVariant &value, QString role)
{
    if (index < 0 || index > relationships->getRelationships().size() ) return false;
    return setData(this->index(index,0),value,lookupRoleName(role));
}

Qt::ItemFlags EventRelationshipModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() | !relationships)
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

QHash<int, QByteArray> EventRelationshipModel::roleNames() const
{
    QHash<int, QByteArray> role;
    role[eventID1Role] = "eventId1";
    role[eventID2Role] = "eventId2";
    role[bidirectionalRole] = "bidirectional";

    return role;
}

int EventRelationshipModel::lookupRoleName(QString name) const
{
    if (name == "eventId1") return eventID1Role;
    if (name == "eventId2") return eventID2Role;
    if (name == "bidirectional") return bidirectionalRole;
    return 0;
}

void EventRelationshipModel::setRelationships(EventRelationshipList *value)
{
    beginResetModel();

    if (relationships) relationships->disconnect(this);

    relationships = value;

    if (relationships) {
        connect(relationships, &EventRelationshipList::preRelationshipAppended, this, [this](int start, int count) {
            beginInsertRows(QModelIndex(), start, start+count-1);
        });
        connect(relationships, &EventRelationshipList::postRelationshipAppended, this, [this]() {
            endInsertRows();
        });
        connect(relationships, &EventRelationshipList::preRelationshipRemoved, this, [this](int index) {
            beginRemoveRows(QModelIndex(), index, index);
        });
        connect(relationships, &EventRelationshipList::postRelationshipRemoved, this, [this]() {
            endRemoveRows();
        });
        connect(relationships, &EventRelationshipList::preRelationshipReset, this, [this]() {
            beginResetModel();
        });
        connect(relationships, &EventRelationshipList::postRelationshipReset, this, [this]() {
            endResetModel();
        });
    }
    endResetModel();
}

EventRelationshipList *EventRelationshipModel::getRelationships() const
{
    return relationships;
}

LoopProjectFile::EventRelationship EventRelationshipModel::get(int index) const
{
    LoopProjectFile::EventRelationship res;
    if (!relationships || index < 0 || index > relationships->getRelationships().size())
        return res;
    return (*(relationships->getRelationships()[index]));
}

void EventRelationshipModel::refreshModel()
{
    beginResetModel();
    endResetModel();
}

