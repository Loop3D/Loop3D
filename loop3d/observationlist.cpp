#include "observationlist.h"
#include <QtAlgorithms>
#include <QDebug>

ObservationList::ObservationList(QObject *parent) : QObject(parent)
{
}

void ObservationList::clearList()
{
    qDebug() << "Loop: Clearing observation list";
    preItemReset();
    observations.clear();
    postItemReset();
    qDebug() << "Loop: Cleared observation list";
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

    preItemReset();
    std::vector<LoopProjectFile::FaultObservation> faultObservations;
    LoopProjectFile::GetFaultObservations(name.toStdString(),faultObservations,false);
    if (faultObservations.size()) {
        for (auto it=faultObservations.begin();it!=faultObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::FaultObservation> observation = std::make_shared<LoopProjectFile::FaultObservation>(std::move(*it));
            observation->type = LoopProjectFile::FAULTOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::FoldObservation> foldObservations;
    LoopProjectFile::GetFoldObservations(name.toStdString(),foldObservations,false);
    if (foldObservations.size()) {
        for (auto it=foldObservations.begin();it!=foldObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::FoldObservation> observation = std::make_shared<LoopProjectFile::FoldObservation>(std::move(*it));
            observation->type = LoopProjectFile::FOLDOBSERVATION;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::FoliationObservation> foliationObservations;
    LoopProjectFile::GetFoliationObservations(name.toStdString(),foliationObservations,false);
    if (foliationObservations.size()) {
        for (auto it=foliationObservations.begin();it!=foliationObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::FoliationObservation> observation = std::make_shared<LoopProjectFile::FoliationObservation>(std::move(*it));
            observation->type = LoopProjectFile::FOLIATIONOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::DiscontinuityObservation> discontinuityObservations;
    LoopProjectFile::GetDiscontinuityObservations(name.toStdString(),discontinuityObservations,false);
    if (discontinuityObservations.size()) {
        for (auto it=discontinuityObservations.begin();it!=discontinuityObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::DiscontinuityObservation> observation = std::make_shared<LoopProjectFile::DiscontinuityObservation>(std::move(*it));
            observation->type = LoopProjectFile::DISCONTINUITYOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::StratigraphicObservation> stratigraphicObservations;
    LoopProjectFile::GetStratigraphicObservations(name.toStdString(),stratigraphicObservations,false);
    if (stratigraphicObservations.size()) {
        for (auto it=stratigraphicObservations.begin();it!=stratigraphicObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::StratigraphicObservation> observation = std::make_shared<LoopProjectFile::StratigraphicObservation>(std::move(*it));
            observation->type = LoopProjectFile::STRATIGRAPHICOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    postItemReset();
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
        if ((*it)->type == LoopProjectFile::FAULTOBSERVATION) {
            faultObservations.push_back(*(LoopProjectFile::FaultObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::FOLDOBSERVATION) {
            foldObservations.push_back(*(LoopProjectFile::FoldObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::FOLIATIONOBSERVATION) {
            foliationObservations.push_back(*(LoopProjectFile::FoliationObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::DISCONTINUITYOBSERVATION) {
            discontinuityObservations.push_back(*(LoopProjectFile::DiscontinuityObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::STRATIGRAPHICOBSERVATION) {
            stratigraphicObservations.push_back(*(LoopProjectFile::StratigraphicObservation*)(it->get()));
        } else {
            std::cout << "UNIDENTIFIED type for observation of event " << (*it)->eventId << std::endl;
        }
    }
    if (faultObservations.size()) LoopProjectFile::SetFaultObservations(name.toStdString(),faultObservations,false);
    if (foldObservations.size()) LoopProjectFile::SetFoldObservations(name.toStdString(),foldObservations,false);
    if (foliationObservations.size()) LoopProjectFile::SetFoliationObservations(name.toStdString(),foliationObservations,false);
    if (discontinuityObservations.size()) LoopProjectFile::SetDiscontinuityObservations(name.toStdString(),discontinuityObservations,false);
    if (stratigraphicObservations.size()) LoopProjectFile::SetStratigraphicObservations(name.toStdString(),stratigraphicObservations,false);

    return result;
}

bool ObservationList::setObservationAt(int index, const LoopProjectFile::Observation& observation)
{
    if (index < 0 || index >= observations.size())
        return false;

    (*observations[index]) = observation;
    return true;
}

bool ObservationList::appendItem(int eventId, double easting, double northing, double altitude, LoopProjectFile::ObservationType type)
{
    std::shared_ptr<LoopProjectFile::Observation> observation = std::make_shared<LoopProjectFile::Observation>();
    observation->eventId = eventId;
    observation->easting = easting;
    observation->northing = northing;
    observation->altitude = altitude;
    observation->type = type;

    preItemAppended(observations.size(),1);
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

