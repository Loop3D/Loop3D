#ifndef EVENTLIST_H
#define EVENTLIST_H

#include <QAbstractListModel>
#include <QVector>
#include <netcdf>

struct EventItem {
        QString name;
        bool isActive;
        float minAge;
        float maxAge;
        int eventID;
        int rank;
        QString type;
};

class EventList : public QObject {
    Q_OBJECT
    public:
        EventList(QObject* parent=nullptr);
        ~EventList() { events.clear(); }

        int loadFromFile(QString filename);
        bool setEventAt(int index, const EventItem& event);
        QVector<EventItem> getEvents() const { return events; }
        Q_INVOKABLE void sort(void);

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
        static bool order(const EventItem& a, const EventItem& b) {
            if (a.isActive == b.isActive) {
                if (abs(a.minAge - b.minAge) < 0.0001f) {
                    if (abs(a.maxAge - b.maxAge) < 0.0001f) return a.eventID < b.eventID;
                    else return a.maxAge < b.maxAge;
                } else return a.minAge < b.minAge;
            } else if (a.isActive) return true;
            return false;
        }
};

#endif // EVENTLIST_H
