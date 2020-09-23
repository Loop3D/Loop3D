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

    std::shared_ptr<LoopProjectFile::Observation> item = observations->getObservations().at(index.row());
    switch(role) {
        case eastingRole:
            return QVariant(item->easting);
        case northingRole:
            return QVariant(item->northing);
        case altitudeRole:
            return QVariant(item->altitude);
        case eventIDRole:
            return QVariant(item->eventId);
        case typeRole:
            return QVariant(item->type);
        default: break;
    }
    switch (item->type) {
        case LoopProjectFile::FAULTOBSERVATION: {
            LoopProjectFile::FaultObservation* fault_item = (LoopProjectFile::FaultObservation*)(item.get());
            switch (role) {
                case dipDirRole:
                        return QVariant(fault_item->dipdir);
                case dipRole:
                        return QVariant(fault_item->dip);
                case dipPolarityRole:
                        return QVariant(fault_item->dipPolarity);
                case valRole:
                        return QVariant(fault_item->val);
                case displacementRole:
                        return QVariant(fault_item->displacement);
            }
        }
        case LoopProjectFile::FOLDOBSERVATION: {
            LoopProjectFile::FoldObservation* fold_item = (LoopProjectFile::FoldObservation*)(item.get());
            switch (role) {
                case axisXRole:
                        return QVariant(fold_item->axisX);
                case axisYRole:
                        return QVariant(fold_item->axisY);
                case axisZRole:
                        return QVariant(fold_item->axisZ);
                case foliationRole:
                        return QVariant(fold_item->foliation);
                case whatIsFoldedRole:
                        return QVariant(fold_item->whatIsFolded);
            }
        }
        case LoopProjectFile::FOLIATIONOBSERVATION: {
            LoopProjectFile::FoliationObservation* foliation_item = (LoopProjectFile::FoliationObservation*)(item.get());
            switch (role) {
                case dipDirRole:
                        return QVariant(foliation_item->dipdir);
                case dipRole:
                        return QVariant(foliation_item->dip);
            }
        }
        case LoopProjectFile::DISCONTINUITYOBSERVATION: {
            LoopProjectFile::DiscontinuityObservation* discontinuity_item = (LoopProjectFile::DiscontinuityObservation*)(item.get());
            switch (role) {
                case dipDirRole:
                        return QVariant(discontinuity_item->dipdir);
                case dipRole:
                        return QVariant(discontinuity_item->dip);
            }
        }
        case LoopProjectFile::STRATIGRAPHICOBSERVATION: {
            LoopProjectFile::StratigraphicObservation* stratigraphic_item = (LoopProjectFile::StratigraphicObservation*)(item.get());
            switch (role) {
                case dipDirRole:
                        return QVariant(stratigraphic_item->dipdir);
                case dipRole:
                        return QVariant(stratigraphic_item->dip);
                case dipPolarityRole:
                        return QVariant(stratigraphic_item->dipPolarity);
                case layerRole:
                        return QVariant(stratigraphic_item->layer);
            }
        }
        default: break;
    }
    return QVariant(false);
}

QVariant ObservationModel::dataIndexed(int index, QString role) const
{
    if (index < 0 || index > observations->getObservations().size() ) return QVariant();
    return data(this->index(index,0),lookupRoleName(role));
}

bool ObservationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!observations) return false;

    std::shared_ptr<LoopProjectFile::Observation> item = observations->getObservations().at(index.row());
    switch (role) {
    case eastingRole:
        item->easting = value.toDouble();
        break;
    case northingRole:
        item->northing = value.toDouble();
        break;
    case altitudeRole:
        item->altitude = value.toDouble();
        break;
    case eventIDRole:
        item->eventId = value.toInt();
        break;
    case typeRole:
        // TODO: when changing type via the GUI need to remove old item, create new and refile
        // Currently forbid this
        std::cout << "Changing of type individually is a very bad idea, currently not implimented!!" << std::endl;
//        if (value == "fault") item->type = LoopProjectFile::FAULTOBSERVATION;
//        else if (value == "fold") item->type = LoopProjectFile::FOLDOBSERVATION;
//        else if (value == "foliation") item->type = LoopProjectFile::FOLIATIONOBSERVATION;
//        else if (value == "discontinuity") item->type = LoopProjectFile::DISCONTINUITYOBSERVATION;
//        else if (value == "stratigraphic") item->type = LoopProjectFile::STRATIGRAPHICOBSERVATION;
//        else item->type = LoopProjectFile::INVALIDOBSERVATION;
        break;
    default: break;
    }
    switch (item->type) {
        case LoopProjectFile::FAULTOBSERVATION: {
                LoopProjectFile::FaultObservation* fault_item = (LoopProjectFile::FaultObservation*)(item.get());
                switch (role) {
                    case dipDirRole:
                        fault_item->dipdir = value.toDouble();
                        break;
                    case dipRole:
                        fault_item->dip = value.toDouble();
                        break;
                    case dipPolarityRole:
                        fault_item->dipPolarity = value.toDouble();
                        break;
                    case valRole:
                        fault_item->val = value.toDouble();
                        break;
                    case displacementRole:
                        fault_item->displacement = value.toDouble();
                        break;
                }
            break;
        }
        case LoopProjectFile::FOLDOBSERVATION: {
                LoopProjectFile::FoldObservation* fold_item = (LoopProjectFile::FoldObservation*)(item.get());
                switch (role) {
                    case dipDirRole:
                        fold_item->axisX = value.toDouble();
                        break;
                    case dipRole:
                        fold_item->axisY = value.toDouble();
                        break;
                    case dipPolarityRole:
                        fold_item->axisZ = value.toDouble();
                        break;
                    case valRole:
                        strncpy_s(fold_item->foliation,value.toString().toStdString().c_str(),LOOP_FOLIATION_NAME_LENGTH);
                        break;
                    case displacementRole:
                        strncpy_s(fold_item->whatIsFolded,value.toString().toStdString().c_str(),LOOP_WHAT_IS_FOLDED_NAME_LENGTH);
                        break;
                }
            break;
        }
        case LoopProjectFile::FOLIATIONOBSERVATION: {
                LoopProjectFile::FoliationObservation* foliation_item = (LoopProjectFile::FoliationObservation*)(item.get());
                switch (role) {
                    case dipDirRole:
                        foliation_item->dipdir = value.toDouble();
                        break;
                    case dipRole:
                        foliation_item->dip = value.toDouble();
                        break;
                }
            break;
        }
        case LoopProjectFile::DISCONTINUITYOBSERVATION: {
                LoopProjectFile::DiscontinuityObservation* discontinuity_item = (LoopProjectFile::DiscontinuityObservation*)(item.get());
                switch (role) {
                    case dipDirRole:
                        discontinuity_item->dipdir = value.toDouble();
                        break;
                    case dipRole:
                        discontinuity_item->dip = value.toDouble();
                        break;
                }
            break;
        }
        case LoopProjectFile::STRATIGRAPHICOBSERVATION: {
                LoopProjectFile::StratigraphicObservation* stratigraphic_item = (LoopProjectFile::StratigraphicObservation*)(item.get());
                switch (role) {
                    case dipDirRole:
                        stratigraphic_item->dipdir = value.toDouble();
                        break;
                    case dipRole:
                        stratigraphic_item->dip = value.toDouble();
                        break;
                    case dipPolarityRole:
                        stratigraphic_item->dipPolarity = value.toDouble();
                        break;
                    case valRole:
                        strncpy_s(stratigraphic_item->layer,value.toString().toStdString().c_str(),LOOP_NAME_LENGTH);
                        break;
                }
            break;
        }
        default: break;
    }

    Q_EMIT dataChanged(index, index, QVector<int>() << role);
    return true;
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
    role[dipDirRole] = "dipDir";
    role[dipRole] = "dip";
    role[dipPolarityRole] = "dipPolarity";
    role[valRole] = "val";
    role[displacementRole] = "displacement";
    role[axisXRole] = "axisX";
    role[axisYRole] = "axisY";
    role[axisZRole] = "axisZ";
    role[foliationRole] = "folation";
    role[whatIsFoldedRole] = "whatIsFolded";
    role[layerRole] = "layer";

    return role;
}

int ObservationModel::lookupRoleName(QString name) const
{
    if (name == "easting") return eastingRole;
    if (name == "northing") return northingRole;
    if (name == "altitude") return altitudeRole;
    if (name == "eventID") return eventIDRole;
    if (name == "type") return typeRole;
    if (name == "dipDir") return dipDirRole;
    if (name == "dip") return dipRole;
    if (name == "dipPolarity") return dipPolarityRole;
    if (name == "val") return valRole;
    if (name == "displacement") return displacementRole;
    if (name == "axisX") return axisXRole;
    if (name == "axisY") return axisYRole;
    if (name == "axisZ") return axisZRole;
    if (name == "foliation") return foliationRole;
    if (name == "whatIsFolded") return whatIsFoldedRole;
    if (name == "layer") return layerRole;

    return 0;

}

void ObservationModel::setObservations(ObservationList *value)
{
    beginResetModel();

    if (observations) observations->disconnect(this);

    observations = value;

    if (observations) {
        connect(observations, &ObservationList::preItemAppended, this, [this](int start, int count) {
//            const int index = observations->getObservations().size();
            beginInsertRows(QModelIndex(), start, start+count-1);
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
    return (*(observations->getObservations()[index]));
}

int ObservationModel::findObservationByID(int eventId)
{
    if (!observations || observations->getObservations().size() <=0) return -1;
    for (int i=0;i<observations->getObservations().size();i++) {
        if (observations->getObservations().at(i)->eventId == eventId) return i;
    }
    return -1;
}

