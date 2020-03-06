#include "eventlist.h"
#include <QtAlgorithms>
#include <QDebug>


// Need to move this to LoopProjectFile lib
struct faultEventType {
    uint32_t eventId;
    double_t minAge;
    double_t maxAge;
    char enabled;
    char name[30];
};

EventList::EventList(QObject *parent) : QObject(parent)
{
}

int EventList::loadFromFile(QString filename)
{
    int result = 0;
    while (events.size()) removeItem(0);
    netCDF::NcFile dataFile;
    try {
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

        dataFile.open(name.toStdString().c_str(), netCDF::NcFile::read);

        netCDF::NcGroup regionInformationGroup = dataFile.getGroup("ExtractedInformation");
        if (regionInformationGroup.isNull()) {
            qDebug() << "No Extracted Information Group in netCDF file";
            result = 1;
        } else {
            netCDF::NcGroup eventLogGroup = regionInformationGroup.getGroup("EventLog");
            if (eventLogGroup.isNull()) {
                qDebug() << "No Event Log Group in netCDF file";
                result = 1;
            } else {
                netCDF::NcVar events = eventLogGroup.getVar("faultEvents");
                netCDF::NcDim index = eventLogGroup.getDim("index");

                faultEventType* faultEvents = static_cast<faultEventType*>(malloc(sizeof(faultEventType)* index.getSize()));
                std::vector<size_t> start;  start.push_back(0);
                std::vector<size_t> count;  count.push_back(index.getSize());
                events.getVar(start,count,faultEvents);
                for (unsigned int i=0;i<index.getSize();i++){
                    appendItem(faultEvents[i].eventId,
                               faultEvents[i].name,
                               faultEvents[i].minAge,
                               faultEvents[i].maxAge,
                               "Fault",
                               0,
                               faultEvents[i].enabled?true:false);
                }
                sort();
            }
        }
    } catch(netCDF::exceptions::NcException& e) {
        qFatal("%s", e.what());
        qFatal("Error creating file (%s)", filename.toStdString().c_str());
        if (!dataFile.isNull()) dataFile.close();
        result = 1;
    }
    return result;
}

bool EventList::setEventAt(int index, const EventItem& event)
{
    if (index < 0 || index >= events.size())
        return false;

    events[index] = event;
    return true;
}

void EventList::sort()
{
    std::sort(events.begin(),events.end(),order);
    std::list<float> minAges;
    std::list<float> maxAges;
    int rank = 0;
    for (auto event= events.begin();event!=events.end();event++) {
        if (!event->isActive) continue;
        if (minAges.size() == 0) {
            event->rank = 0;
        } else {
            while (minAges.size() && event->minAge > maxAges.back()) {
                minAges.pop_back(); maxAges.pop_back(); rank--;
            }
            event->rank = rank;
        }
        minAges.push_back(event->minAge);
        maxAges.push_back(event->maxAge);
        rank++;
    }
}

bool EventList::appendItem(int eventID, QString name, float minAge, float maxAge, QString type, int rank, bool isActive)
{
    EventItem event;
    event.name = name;
    event.eventID = eventID;
    event.minAge = minAge;
    event.maxAge = maxAge;
    event.type = type;
    event.isActive = isActive;
    event.rank = rank;

    preItemAppended();
    events.append(event);
    postItemAppended();
    return 1;
}


bool EventList::removeItem(int index)
{
    if (index < 0 || index >= events.size()) return false;

    preItemRemoved(index);
    events.removeAt(index);
    postItemRemoved();
    return true;
}
