#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <QAbstractListModel>
#include <QVector>
//#include <netcdf>

#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"

struct EventItem {
        QString name;
        bool isActive;
        float minAge;
        float maxAge;
        int eventID;
        int rank;
        QString type;
};

struct PermutationBlock {
    float minAge;
    float maxAge;
    unsigned long long permutations;
    int maxRank;
    PermutationBlock(float min, float max, unsigned long long perm, int rank) {
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
        ~EventList() { events.clear(); pBlocks.clear(); }

        int loadFromFile(QString filename);
        int saveToFile(QString filename);
        bool setEventAt(int index, const EventItem& event);
        QVector<EventItem> getEvents() const { return events; }
        QVector<PermutationBlock> getPermutationBlocks() const { return pBlocks; }
        Q_INVOKABLE void sort(void);
        Q_INVOKABLE unsigned long long calcPermutations(void);

    Q_SIGNALS:
        void preItemAppended();
        void postItemAppended();
        void preItemRemoved(int index);
        void postItemRemoved();

    public Q_SLOTS:
        bool appendItem(int eventID, QString name, float minAge, float maxAge, QString type, int rank=0, bool isActive=true);
        bool removeItem(int index);

    private:
        QVector<EventItem> events;
        QVector<PermutationBlock> pBlocks;
        static bool order(const EventItem& a, const EventItem& b) {
            if (a.isActive == b.isActive) {
                if (abs(a.minAge - b.minAge) < 0.0001f) {
                    if (abs(a.maxAge - b.maxAge) < 0.0001f) return a.eventID < b.eventID;
                    else return a.maxAge < b.maxAge;
                } else return a.minAge < b.minAge;
            } else if (a.isActive) return true;
            return false;
        }
        unsigned long long factorial(unsigned int n);
        unsigned long long ApproxPerm(unsigned int numElements, unsigned long long numRestrictions);
        unsigned long long numPermutations(std::vector<int> elements, std::vector<int> currentPerm, std::vector<int>& restrictionElem1, std::vector<int>& restrictionElem2);
};

#endif // EVENTLIST_H
