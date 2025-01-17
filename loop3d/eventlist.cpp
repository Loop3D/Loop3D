#include "eventlist.h"
#include "3dviewer.h"
#include <QtAlgorithms>
#include <QDebug>

EventList::EventList(QObject *parent) : QObject(parent)
{
}

void EventList::clearList()
{
//    qDebug() << "Loop: Clearing event list";
    Q_EMIT preItemReset();
    events.clear();
    Q_EMIT postItemReset();
//    qDebug() << "Loop: Cleared event list";
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

    Q_EMIT preItemReset();
    std::vector<LoopProjectFile::FaultEvent> faultEvents;
    LoopProjectFile::GetFaultEvents(name.toStdString(),faultEvents,false);
    if (faultEvents.size()) {
        for (auto it=faultEvents.begin();it!=faultEvents.end();it++) {
            std::shared_ptr<LoopProjectFile::FaultEvent> event = std::make_shared<LoopProjectFile::FaultEvent>(std::move(*it));
            events.append(event);
        }
    }
    std::vector<LoopProjectFile::FoldEvent> foldEvents;
    LoopProjectFile::GetFoldEvents(name.toStdString(),foldEvents,false);
    if (foldEvents.size()) {
        for (auto it=foldEvents.begin();it!=foldEvents.end();it++) {
            std::shared_ptr<LoopProjectFile::FoldEvent> event = std::make_shared<LoopProjectFile::FoldEvent>(std::move(*it));
            events.append(event);
        }
    }
    std::vector<LoopProjectFile::FoliationEvent> foliationEvents;
    LoopProjectFile::GetFoliationEvents(name.toStdString(),foliationEvents,false);
    if (foliationEvents.size()) {
        for (auto it=foliationEvents.begin();it!=foliationEvents.end();it++) {
            std::shared_ptr<LoopProjectFile::FoliationEvent> event = std::make_shared<LoopProjectFile::FoliationEvent>(std::move(*it));
            events.append(event);
        }
    }
    std::vector<LoopProjectFile::DiscontinuityEvent> discontinuityEvents;
    LoopProjectFile::GetDiscontinuityEvents(name.toStdString(),discontinuityEvents,false);
    if (discontinuityEvents.size()) {
        for (auto it=discontinuityEvents.begin();it!=discontinuityEvents.end();it++) {
            std::shared_ptr<LoopProjectFile::DiscontinuityEvent> event = std::make_shared<LoopProjectFile::DiscontinuityEvent>(std::move(*it));
            events.append(event);
        }
    }
    std::vector<LoopProjectFile::StratigraphicLayer> stratigraphicLayers;
    LoopProjectFile::GetStratigraphicLayers(name.toStdString(),stratigraphicLayers,false);
    if (stratigraphicLayers.size()) {
        for (auto it=stratigraphicLayers.begin();it!=stratigraphicLayers.end();it++) {
            std::shared_ptr<LoopProjectFile::StratigraphicLayer> layer = std::make_shared<LoopProjectFile::StratigraphicLayer>(std::move(*it));
            events.append(layer);
        }
    }
    Q_EMIT postItemReset();
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
        if ((*it)->type == LoopProjectFile::FAULTEVENT) {
            faultEvents.push_back(*(LoopProjectFile::FaultEvent*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::FOLDEVENT) {
            foldEvents.push_back(*(LoopProjectFile::FoldEvent*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::FOLIATIONEVENT) {
            foliationEvents.push_back(*(LoopProjectFile::FoliationEvent*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::DISCONTINUITYEVENT) {
            discontinuityEvents.push_back(*(LoopProjectFile::DiscontinuityEvent*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::STRATIGRAPHICLAYER) {
            stratigraphicLayers.push_back(*(LoopProjectFile::StratigraphicLayer*)(it->get()));
        } else {
            std::cout << "UNIDENTIFIED type for event " << (*it)->name << std::endl;
        }
    }
    if (faultEvents.size()) LoopProjectFile::SetFaultEvents(name.toStdString(),faultEvents,false);
    if (foldEvents.size()) LoopProjectFile::SetFoldEvents(name.toStdString(),foldEvents,false);
    if (foliationEvents.size()) LoopProjectFile::SetFoliationEvents(name.toStdString(),foliationEvents,false);
    if (discontinuityEvents.size()) LoopProjectFile::SetDiscontinuityEvents(name.toStdString(),discontinuityEvents,false);
    if (stratigraphicLayers.size()) LoopProjectFile::SetStratigraphicLayers(name.toStdString(),stratigraphicLayers,false);

    return result;
}

bool EventList::setEventAt(int index, const LoopProjectFile::Event& event)
{
    if (index < 0 || index >= events.size())
        return false;

    (*events[index]) = event;
    return true;
}

void EventList::sort()
{
    std::sort(events.begin(),events.end(),order);
    std::list<double> minAges;
    std::list<double> maxAges;
    int rank = 0;
    for (auto event= events.begin();event!=events.end();event++) {
        if (!(*event)->enabled) continue;
        if (minAges.size() == 0) {
            (*event)->rank = 0;
        } else {
            while (minAges.size() && (*event)->minAge > maxAges.back()) {
                minAges.pop_back(); maxAges.pop_back(); rank--;
            }
            (*event)->rank = rank;
        }
        minAges.push_back((*event)->minAge);
        maxAges.push_back((*event)->maxAge);
        rank++;
    }

    if (L3DViewer::instance()) {
        L3DViewer* viewer =  L3DViewer::instance();
        int count = 0;
        float stratigraphicDepth = 0;
        for (auto it=events.begin(); it!=events.end();it++) {
            if ((*it)->type == 4  && (*it)->enabled) {
                LoopProjectFile::StratigraphicLayer* event = static_cast<LoopProjectFile::StratigraphicLayer*>(it->get());
                stratigraphicDepth -= float(event->thickness);
                viewer->setColourStep(count,stratigraphicDepth);
                viewer->setColourOption(count,QVector3D(
                        ((int)(unsigned char)event->colour1Red)/256.0f,
                        ((int)(unsigned char)event->colour1Green)/256.0f,
                        ((int)(unsigned char)event->colour1Blue)/256.0f
                        ));
                count++;
                if (count>=100) break;
            }
        }
    }
}

bool EventList::appendItem(int eventID, QString name, QString supergroup, double minAge, double maxAge, LoopProjectFile::EventType type, int rank, bool isActive)
{
    std::shared_ptr<LoopProjectFile::Event> event = std::make_shared<LoopProjectFile::Event>();
    strncpy(event->name,name.toStdString().c_str(),30);
    strncpy(event->supergroup,supergroup.toStdString().c_str(),30);
    event->eventId = eventID;
    event->minAge = minAge;
    event->maxAge = maxAge;
    event->type = type;
    event->enabled = isActive;
    event->rank = rank;

    Q_EMIT preItemAppended(events.size(),1);
    events.append(event);
    Q_EMIT postItemAppended();
    return 1;
}

bool EventList::removeItem(int index)
{
    if (index < 0 || index >= events.size()) return false;

    Q_EMIT preItemRemoved(index);
    events.removeAt(index);
    Q_EMIT postItemRemoved();
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
    // (TODO) Fix bug divisor errors to 0
    // divisor should not ever equal 0, perhaps overflow?
    if (divisor == 0) return 0;
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
    while (startIndex < events.size() && !events[startIndex]->enabled) {
        startIndex++;
    }
    int endIndex = startIndex+1;
    while (startIndex < events.size() && events[startIndex]->enabled) {
        endIndex = startIndex+1;
        if (endIndex == events.size() || !events[endIndex]->enabled) break;
        double minAge = events[startIndex]->minAge;
        double maxAge = events[startIndex]->maxAge;
        int maxRank = 1;
        while (endIndex < events.size() && events[endIndex]->minAge <= maxAge && events[endIndex]->enabled) {
            if (events[endIndex]->maxAge > maxAge) maxAge = events[endIndex]->maxAge;
            if (events[endIndex]->rank > maxRank) maxRank = events[endIndex]->rank;
            endIndex++;
        }

        std::vector<int> restrictionElem1;
        std::vector<int> restrictionElem2;
        // Calculate Restrictions of sub-list
        for (int i=startIndex;i<endIndex;i++) {
            for (int j=i+1;j<endIndex;j++) {
                if (events[j]->minAge > events[i]->maxAge) {
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



/* ******************* EventRelationshipList area ****************** */
EventRelationshipList::EventRelationshipList(QObject *parent) : QObject(parent)
{
}


void EventRelationshipList::clearList()
{
    Q_EMIT preRelationshipReset();
    relationships.clear();
    Q_EMIT postRelationshipReset();
}

int EventRelationshipList::loadFromFile(QString filename)
{
    int result = 0;
    while (relationships.size()) removeRelationship(0);

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

    Q_EMIT preRelationshipReset();
    std::vector<LoopProjectFile::EventRelationship> eventRelationships;
    LoopProjectFile::GetEventRelationships(name.toStdString(),eventRelationships,false);
    if (eventRelationships.size()) {
        for (auto it=eventRelationships.begin();it!=eventRelationships.end();it++) {
            std::shared_ptr<LoopProjectFile::EventRelationship> relationship = std::make_shared<LoopProjectFile::EventRelationship>(std::move(*it));
            relationships.append(relationship);
        }
    }
    Q_EMIT postRelationshipReset();

    return result;
}

int EventRelationshipList::saveToFile(QString filename)
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

    std::vector<LoopProjectFile::EventRelationship> eventRelationships;
    auto eventRelationshipList = getRelationships();
    for (auto it=eventRelationshipList.begin();it!=eventRelationshipList.end();it++) {
        eventRelationships.push_back(*(LoopProjectFile::EventRelationship*)(it->get()));
    }
    if (eventRelationships.size()) LoopProjectFile::SetEventRelationships(name.toStdString(),eventRelationships,false);

    return result;
}

bool EventRelationshipList::appendRelationship(int eventID1, int eventID2, bool bidirectional)
{
    std::shared_ptr<LoopProjectFile::EventRelationship> relationship = std::make_shared<LoopProjectFile::EventRelationship>();
    relationship->eventId1 = eventID1;
    relationship->eventId2 = eventID2;
    relationship->bidirectional = bidirectional;

    Q_EMIT preRelationshipAppended(relationships.size(),1);
    relationships.append(relationship);
    Q_EMIT postRelationshipAppended();
    return 1;
}

bool EventRelationshipList::removeRelationship(int index)
{
    if (index < 0 || index >= relationships.size()) return false;

    Q_EMIT preRelationshipRemoved(index);
    relationships.removeAt(index);
    Q_EMIT postRelationshipRemoved();
    return true;
}
