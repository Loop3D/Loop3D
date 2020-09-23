#ifndef OBSERVATIONLIST_H
#define OBSERVATIONLIST_H

#include <QAbstractListModel>
#include <QVector>
//#include <netcdf>

#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"

class ObservationList : public QObject {
    Q_OBJECT
    public:
        ObservationList(QObject* parent=nullptr);
        ~ObservationList() { observations.clear(); }

        int loadFromFile(QString filename);
        int saveToFile(QString filename);
        bool setObservationAt(int index, const LoopProjectFile::Observation& observation);
        QVector<std::shared_ptr<LoopProjectFile::Observation>> getObservations() const { return observations; }

    Q_SIGNALS:
        void preItemAppended(int start, int count);
        void postItemAppended();
        void preItemRemoved(int index);
        void postItemRemoved();

    public Q_SLOTS:
        bool appendItem(int eventID, double easting, double northing, double altitude, LoopProjectFile::ObservationType type);
        bool removeItem(int index);

    private:
        QVector<std::shared_ptr<LoopProjectFile::Observation>> observations;
};

#endif // ObservationLIST_H
