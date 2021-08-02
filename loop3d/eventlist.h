#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <QAbstractListModel>
#include <QVector>
//#include <netcdf>

#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"

struct PermutationBlock {
    double minAge;
    double maxAge;
    unsigned long long permutations;
    int maxRank;
    PermutationBlock(double min, double max, unsigned long long perm, int rank) {
        minAge = min;
        maxAge = max;
        permutations = perm;
        maxRank = rank;
    }
};

class EventList : public QObject {
    Q_OBJECT
    public:
        EventList(QObject* parent=nullptr);
        ~EventList() override { events.clear(); pBlocks.clear(); }

        void clearList();
        int loadFromFile(QString filename);
        int saveToFile(QString filename);
        bool setEventAt(int index, const LoopProjectFile::Event& event);
        QVector<std::shared_ptr<LoopProjectFile::Event>> getEvents() const { return events; }
        QVector<PermutationBlock> getPermutationBlocks() const { return pBlocks; }
        Q_INVOKABLE void sort(void);
        Q_INVOKABLE unsigned long long calcPermutations(void);

    Q_SIGNALS:
        void preItemAppended(int start, int count);
        void postItemAppended();
        void preItemRemoved(int index);
        void postItemRemoved();
        void preItemReset();
        void postItemReset();

    public Q_SLOTS:
        bool appendItem(int eventID, QString name, QString supergroup, double minAge, double maxAge, LoopProjectFile::EventType type, int rank=0, bool isActive=true);
        bool removeItem(int index);

    private:
        QVector<std::shared_ptr<LoopProjectFile::Event>> events;
        QVector<PermutationBlock> pBlocks;
        static bool order(const std::shared_ptr<LoopProjectFile::Event> a, const std::shared_ptr<LoopProjectFile::Event> b) {
            if (a->enabled == b->enabled) {
                if (abs(a->minAge - b->minAge) < 0.0001) {
                    if (abs(a->maxAge - b->maxAge) < 0.0001) return a->eventId < b->eventId;
                    else return a->maxAge < b->maxAge;
                } else return a->minAge < b->minAge;
            } else if (a->enabled) return true;
            return false;
        }
        unsigned long long factorial(unsigned int n);
        unsigned long long ApproxPerm(unsigned int numElements, unsigned long long numRestrictions);
        unsigned long long numPermutations(std::vector<int> elements, std::vector<int> currentPerm, std::vector<int>& restrictionElem1, std::vector<int>& restrictionElem2);
};

class EventRelationshipList : public QObject {
    Q_OBJECT
    public:
        EventRelationshipList(QObject* parent=nullptr);
        ~EventRelationshipList() override { relationships.clear(); }

        void clearList();
        int loadFromFile(QString filename);
        int saveToFile(QString filename);
        QVector<std::shared_ptr<LoopProjectFile::EventRelationship>> getRelationships() const { return relationships; }

    Q_SIGNALS:
        void preRelationshipAppended(int start, int count);
        void postRelationshipAppended();
        void preRelationshipRemoved(int index);
        void postRelationshipRemoved();
        void preRelationshipReset();
        void postRelationshipReset();

    public Q_SLOTS:
        bool appendRelationship(int eventID1, int eventID2, bool bidirectional);
        bool removeRelationship(int index);

    private:
        QVector<std::shared_ptr<LoopProjectFile::EventRelationship>> relationships;
};

#endif // EVENTLIST_H
