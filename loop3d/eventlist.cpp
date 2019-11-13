#include "eventlist.h"
#include <QtAlgorithms>
#include <QDebug>

EventList::EventList(QObject *parent) : QObject(parent)
{
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
