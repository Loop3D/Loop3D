#include "eventlist.h"
#include <QtAlgorithms>
#include <QDebug>

EventList::EventList(QObject *parent) : QObject(parent)
{
}

int EventList::loadFromFile(QString filename)
{
    int result = 0;
    while (events.size()) removeItem(0);

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

    std::vector<LoopProjectFile::FaultEvent> faultEvents;
    LoopProjectFile::GetFaultEvents(name.toStdString(),faultEvents,true);
    for (auto it=faultEvents.begin();it!=faultEvents.end();it++) {
        appendItem(it->eventId,it->name,it->minAge,it->maxAge,it->type,LoopProjectFile::FAULTEVENT,it->enabled?true:false);
    }
    std::vector<LoopProjectFile::FoldEvent> foldEvents;
    LoopProjectFile::GetFoldEvents(name.toStdString(),foldEvents,true);
    for (auto it=foldEvents.begin();it!=foldEvents.end();it++) {
        appendItem(it->eventId,it->name,it->minAge,it->maxAge,it->type,LoopProjectFile::FOLDEVENT,it->enabled?true:false);
    }
    std::vector<LoopProjectFile::FoliationEvent> foliationEvents;
    LoopProjectFile::GetFoliationEvents(name.toStdString(),foliationEvents,true);
    for (auto it=foliationEvents.begin();it!=foliationEvents.end();it++) {
        appendItem(it->eventId,it->name,it->minAge,it->maxAge,it->type,LoopProjectFile::FOLIATIONEVENT,it->enabled?true:false);
    }
    std::vector<LoopProjectFile::DiscontinuityEvent> discontinuityEvents;
    LoopProjectFile::GetDiscontinuityEvents(name.toStdString(),discontinuityEvents,true);
    for (auto it=discontinuityEvents.begin();it!=discontinuityEvents.end();it++) {
        appendItem(it->eventId,it->name,it->minAge,it->maxAge,it->type,LoopProjectFile::DISCONTINUITYEVENT,it->enabled?true:false);
    }
    std::vector<LoopProjectFile::StratigraphicLayer> stratigraphicLayers;
    LoopProjectFile::GetStratigraphicLayers(name.toStdString(),stratigraphicLayers,true);
    for (auto it=stratigraphicLayers.begin();it!=stratigraphicLayers.end();it++) {
        appendItem(it->eventId,it->name,it->minAge,it->maxAge,it->type,LoopProjectFile::STRATIGRAPHICLAYER,it->enabled?true:false);
    }
    sort();
    return result;
}

int EventList::saveToFile(QString filename)
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

    std::vector<LoopProjectFile::FaultEvent> faultEvents;
    std::vector<LoopProjectFile::FoldEvent> foldEvents;
    std::vector<LoopProjectFile::FoliationEvent> foliationEvents;
    std::vector<LoopProjectFile::DiscontinuityEvent> discontinuityEvents;
    std::vector<LoopProjectFile::StratigraphicLayer> stratigraphicLayers;
    auto eventList = getEvents();
    for (auto it=eventList.begin();it!=eventList.end();it++) {
        // Load data into appropriate structure
        // TODO map event data with type casting rather than copying data
        if (it->type == LoopProjectFile::FAULTEVENT) {
            LoopProjectFile::FaultEvent event;
            strncpy(event.name,it->name,30);
            event.maxAge = it->maxAge;
            event.minAge = it->minAge;
            event.enabled = it->enabled;
            event.eventId = it->eventId;
            event.rank = it->rank;
            event.type = it->type;
            faultEvents.push_back(event);
        } else if (it->type == LoopProjectFile::FOLDEVENT) {
            LoopProjectFile::FoldEvent event;
            strncpy(event.name,it->name,30);
            event.maxAge = it->maxAge;
            event.minAge = it->minAge;
            event.enabled = it->enabled;
            event.eventId = it->eventId;
            event.rank = it->rank;
            event.type = it->type;
            foldEvents.push_back(event);
        } else if (it->type == LoopProjectFile::FOLIATIONEVENT) {
            LoopProjectFile::FoliationEvent event;
            strncpy(event.name,it->name,30);
            event.maxAge = it->maxAge;
            event.minAge = it->minAge;
            event.enabled = it->enabled;
            event.eventId = it->eventId;
            event.rank = it->rank;
            event.type = it->type;
            foliationEvents.push_back(event);
        } else if (it->type == LoopProjectFile::DISCONTINUITYEVENT) {
            LoopProjectFile::DiscontinuityEvent event;
            strncpy(event.name,it->name,30);
            event.maxAge = it->maxAge;
            event.minAge = it->minAge;
            event.enabled = it->enabled;
            event.eventId = it->eventId;
            event.rank = it->rank;
            event.type = it->type;
            discontinuityEvents.push_back(event);
        } else if (it->type == LoopProjectFile::STRATIGRAPHICLAYER) {
            LoopProjectFile::StratigraphicLayer layer;
            strncpy(layer.name,it->name,30);
            layer.maxAge = it->maxAge;
            layer.minAge = it->minAge;
            layer.enabled = it->enabled;
            layer.eventId = it->eventId;
            layer.rank = it->rank;
            layer.type = it->type;
            stratigraphicLayers.push_back(layer);
        } else {
            std::cout << "UNIDENTIFIED type for event " << it->name << std::endl;
        }
    }
    if (faultEvents.size()) LoopProjectFile::SetFaultEvents(name.toStdString(),faultEvents,true);
    if (foldEvents.size()) LoopProjectFile::SetFoldEvents(name.toStdString(),foldEvents,true);
    if (foliationEvents.size()) LoopProjectFile::SetFoliationEvents(name.toStdString(),foliationEvents,true);
    if (discontinuityEvents.size()) LoopProjectFile::SetDiscontinuityEvents(name.toStdString(),discontinuityEvents,true);
    if (stratigraphicLayers.size()) LoopProjectFile::SetStratigraphicLayers(name.toStdString(),stratigraphicLayers,true);

    return result;
}

bool EventList::setEventAt(int index, const LoopProjectFile::Event& event)
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
        if (!event->enabled) continue;
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

bool EventList::appendItem(int eventID, QString name, float minAge, float maxAge, LoopProjectFile::EventType type, int rank, bool isActive)
{
    LoopProjectFile::Event event;
    strncpy(event.name,name.toStdString().c_str(),30);
//    event.name = name;
    event.eventId = eventID;
    event.minAge = minAge;
    event.maxAge = maxAge;
    event.type = type;
    event.enabled = isActive;
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

unsigned long long EventList::factorial(unsigned int n)
{
    return ((n==0) || (n==1)) ? 1 : n*factorial(n-1);
}

unsigned long long EventList::ApproxPerm(unsigned int numElements, unsigned long long numRestrictions)
{
    unsigned long long divisor = 1;
    unsigned int step = 1;
    while (1) {
        if (step >= numRestrictions) {
            divisor += (numRestrictions * factorial(step));
            break;
        } else {
            divisor += (step * factorial(step));
            numRestrictions -= step;
            step++;
        }
    }
    return (unsigned long long)std::round(factorial(numElements)/divisor);
}

unsigned long long EventList::numPermutations(std::vector<int> elements, std::vector<int> currentPerm, std::vector<int>& restrictionElem1, std::vector<int>& restrictionElem2)
{
    if (elements.size() < 1) {
        return 1;
    }
    unsigned long long permutations = 0;
    for (unsigned int i=0;i<elements.size();i++) {
        // Add element i to currentPerm and remove from elements
        std::vector<int> newPerm = currentPerm;
        newPerm.push_back(elements[i]);
        std::vector<int> newElems;
        for (unsigned int j=0;j<elements.size();j++) {
            if (j != i) newElems.push_back(elements[j]);
        }
        bool validAddition = true;
        // Check this doesn't break restrictions
        for (unsigned int j=0;j<restrictionElem1.size();j++) {
            auto it1 = std::find(newPerm.begin(),newPerm.end(),restrictionElem1[j]);
            auto it2 = std::find(newPerm.begin(),newPerm.end(),restrictionElem2[j]);
            if (it1 != newPerm.end() && it2 != newPerm.end() && std::distance(it1,it2) <= 0) {
                validAddition = false;
                break;
            }
        }
        // Call with adjusted elements and currentPerm
        if (validAddition) permutations += numPermutations(newElems,newPerm,restrictionElem1,restrictionElem2);
    }
    return permutations;
}

unsigned long long EventList::calcPermutations()
{
    pBlocks.clear();
    if (events.size() == 0) return 1;
    sort();

    unsigned long long totalPermutations = 1;
    int startIndex = 0;
    while (startIndex < events.size() && !events[startIndex].enabled) {
        startIndex++;
    }
    int endIndex = startIndex+1;
    while (startIndex < events.size() && events[startIndex].enabled) {
        endIndex = startIndex+1;
        if (endIndex == events.size() || !events[endIndex].enabled) break;
        float minAge = events[startIndex].minAge;
        float maxAge = events[startIndex].maxAge;
        int maxRank = 1;
        while (endIndex < events.size() && events[endIndex].minAge <= maxAge && events[endIndex].enabled) {
            if (events[endIndex].maxAge > maxAge) maxAge = events[endIndex].maxAge;
            if (events[endIndex].rank > maxRank) maxRank = events[endIndex].rank;
            endIndex++;
        }

        std::vector<int> restrictionElem1;
        std::vector<int> restrictionElem2;
        // Calculate Restrictions of sub-list
        for (int i=startIndex;i<endIndex;i++) {
            for (int j=i+1;j<endIndex;j++) {
                if (events[j].minAge > events[i].maxAge) {
                    restrictionElem1.push_back(i);
                    restrictionElem2.push_back(j);
                }
            }
        }

        unsigned int numElems = (unsigned int) (endIndex - startIndex);
        unsigned long long permutationValue = 1;
        if (restrictionElem1.size() == 0) {
            permutationValue = factorial(numElems);
        } else if (restrictionElem1.size() == 1) {
            permutationValue = factorial(numElems) / 2;
        } else {
            // Calculate Permutations of sub-list given restrictions
            // More than about 8 elements in a set causing calc to take longer than 1 second
            if ((endIndex - startIndex) > 8) {
                permutationValue = ApproxPerm(numElems,restrictionElem1.size());
            } else {
                std::vector<int> elements;
                std::vector<int> tmpPerm;
                for (int i=startIndex;i<endIndex;i++) elements.push_back(i);
                permutationValue = numPermutations(elements,tmpPerm,restrictionElem1,restrictionElem2);
            }
        }
        if (permutationValue > 1) {
            pBlocks.push_back(PermutationBlock(minAge,maxAge,permutationValue,maxRank));
        }
        // setup for next loop
        totalPermutations *= permutationValue;
        startIndex = endIndex;
    }
    return totalPermutations;
}
