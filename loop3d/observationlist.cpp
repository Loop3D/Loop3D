#include "observationlist.h"
#include <QtAlgorithms>
#include <QDebug>

ObservationList::ObservationList(QObject *parent) : QObject(parent)
{
}

int ObservationList::loadFromFile(QString filename)
{
    int result = 0;
    while (observations.size()) removeItem(0);

    QStringList list;
    QString name;
    // Find last '/' of the first set of '/'s as in file:/// or url:///
#ifdef _WIN32
    list = filename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = filename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    std::vector<LoopProjectFile::FaultObservation> faultObservations;
    LoopProjectFile::GetFaultObservations(name.toStdString(),faultObservations,true);
    for (auto it=faultObservations.begin();it!=faultObservations.end();it++) {
        appendItem(it->eventId,it->easting,it->northing,it->altitude,LoopProjectFile::FAULTOBSERVATION);
    }
    std::vector<LoopProjectFile::FoldObservation> foldObservations;
    LoopProjectFile::GetFoldObservations(name.toStdString(),foldObservations,true);
    for (auto it=foldObservations.begin();it!=foldObservations.end();it++) {
        appendItem(it->eventId,it->easting,it->northing,it->altitude,LoopProjectFile::FOLDOBSERVATION);
    }
    std::vector<LoopProjectFile::FoliationObservation> foliationObservations;
    LoopProjectFile::GetFoliationObservations(name.toStdString(),foliationObservations,true);
    for (auto it=foliationObservations.begin();it!=foliationObservations.end();it++) {
        appendItem(it->eventId,it->easting,it->northing,it->altitude,LoopProjectFile::FOLIATIONOBSERVATION);
    }
    std::vector<LoopProjectFile::DiscontinuityObservation> discontinuityObservations;
    LoopProjectFile::GetDiscontinuityObservations(name.toStdString(),discontinuityObservations,true);
    for (auto it=discontinuityObservations.begin();it!=discontinuityObservations.end();it++) {
        appendItem(it->eventId,it->easting,it->northing,it->altitude,LoopProjectFile::DISCONTINUITYOBSERVATION);
    }
    std::vector<LoopProjectFile::StratigraphicObservation> stratigraphicObservations;
    LoopProjectFile::GetStratigraphicObservations(name.toStdString(),stratigraphicObservations,true);
    for (auto it=stratigraphicObservations.begin();it!=stratigraphicObservations.end();it++) {
        appendItem(it->eventId,it->easting,it->northing,it->altitude,LoopProjectFile::STRATIGRAPHICOBSERVATION);
    }

    return result;
}

int ObservationList::saveToFile(QString filename)
{
    int result = 0;
    // Find last '/' of the first set of '/'s as in file:/// or url:///
    QStringList list;
    QString name;

#ifdef _WIN32
    list = filename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = filename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    std::vector<LoopProjectFile::FaultObservation> faultObservations;
    std::vector<LoopProjectFile::FoldObservation> foldObservations;
    std::vector<LoopProjectFile::FoliationObservation> foliationObservations;
    std::vector<LoopProjectFile::DiscontinuityObservation> discontinuityObservations;
    std::vector<LoopProjectFile::StratigraphicObservation> stratigraphicObservations;
    auto observationList = getObservations();
    for (auto it=observationList.begin();it!=observationList.end();it++) {
        // Load data into appropriate structure
        // TODO map observation data with type casting rather than copying data
        if (it->type == LoopProjectFile::FAULTOBSERVATION) {
            LoopProjectFile::FaultObservation observation;
            observation.easting = it->easting;
            observation.northing = it->northing;
            observation.altitude = it->altitude;
            observation.eventId = it->eventId;
            observation.type = it->type;
            faultObservations.push_back(observation);
        } else if (it->type == LoopProjectFile::FOLDOBSERVATION) {
            LoopProjectFile::FoldObservation observation;
            observation.easting = it->easting;
            observation.northing = it->northing;
            observation.altitude = it->altitude;
            observation.eventId = it->eventId;
            observation.type = it->type;
            foldObservations.push_back(observation);
        } else if (it->type == LoopProjectFile::FOLIATIONOBSERVATION) {
            LoopProjectFile::FoliationObservation observation;
            observation.easting = it->easting;
            observation.northing = it->northing;
            observation.altitude = it->altitude;
            observation.eventId = it->eventId;
            observation.type = it->type;
            foliationObservations.push_back(observation);
        } else if (it->type == LoopProjectFile::DISCONTINUITYOBSERVATION) {
            LoopProjectFile::DiscontinuityObservation observation;
            observation.easting = it->easting;
            observation.northing = it->northing;
            observation.altitude = it->altitude;
            observation.eventId = it->eventId;
            observation.type = it->type;
            discontinuityObservations.push_back(observation);
        } else if (it->type == LoopProjectFile::STRATIGRAPHICOBSERVATION) {
            LoopProjectFile::StratigraphicObservation observation;
            observation.easting = it->easting;
            observation.northing = it->northing;
            observation.altitude = it->altitude;
            observation.eventId = it->eventId;
            observation.type = it->type;
            stratigraphicObservations.push_back(observation);
        } else {
            std::cout << "UNIDENTIFIED type for observation of event " << it->eventId << std::endl;
        }
    }
    if (faultObservations.size()) LoopProjectFile::SetFaultObservations(name.toStdString(),faultObservations,true);
    if (foldObservations.size()) LoopProjectFile::SetFoldObservations(name.toStdString(),foldObservations,true);
    if (foliationObservations.size()) LoopProjectFile::SetFoliationObservations(name.toStdString(),foliationObservations,true);
    if (discontinuityObservations.size()) LoopProjectFile::SetDiscontinuityObservations(name.toStdString(),discontinuityObservations,true);
    if (stratigraphicObservations.size()) LoopProjectFile::SetStratigraphicObservations(name.toStdString(),stratigraphicObservations,true);

    return result;
}

bool ObservationList::setObservationAt(int index, const LoopProjectFile::Observation& observation)
{
    if (index < 0 || index >= observations.size())
        return false;

    observations[index] = observation;
    return true;
}

bool ObservationList::appendItem(int eventId, double easting, double northing, double altitude, LoopProjectFile::ObservationType type)
{
    LoopProjectFile::Observation observation;
    observation.eventId = eventId;
    observation.easting = easting;
    observation.northing = northing;
    observation.altitude = altitude;
    observation.type = type;

    preItemAppended();
    observations.append(observation);
    postItemAppended();
    return 1;
}


bool ObservationList::removeItem(int index)
{
    if (index < 0 || index >= observations.size()) return false;

    preItemRemoved(index);
    observations.removeAt(index);
    postItemRemoved();
    return true;
}

