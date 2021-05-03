#ifndef OBSERVATIONLIST_H
#define OBSERVATIONLIST_H

#include <QVector>
//#include <netcdf>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QTexture>
#include <QMutex>

#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"

class ObservationList : public QObject {
    Q_OBJECT
    public:
        ObservationList(QObject* parent=nullptr);
        ~ObservationList();

        void clearList();
        int loadFromFile(QString filename);
        int saveToFile(QString filename);
        bool setObservationAt(int index, const LoopProjectFile::Observation& observation);
        QVector<std::shared_ptr<LoopProjectFile::Observation>> getObservations() const { return observations; }
        Qt3DRender::QSharedGLTexture* getObservationsTexture(void) { return m_sharedTexture; }
        int loadTextures();

    Q_SIGNALS:
        void preItemAppended(int start, int count);
        void postItemAppended();
        void preItemRemoved(int index);
        void postItemRemoved();
        void preItemReset();
        void postItemReset();

    public Q_SLOTS:
        bool appendItem(int eventID, double easting, double northing, double altitude, LoopProjectFile::ObservationType type);
        bool removeItem(int index);

    private:
        QVector<std::shared_ptr<LoopProjectFile::Observation>> observations;

        void loadData();
        // Manage observation data into texture for display
        QMutex dataMutex;
        bool m_dataChanged;
        float* m_observationsData;
        QOpenGLTexture *observationsTexture;
        Qt3DRender::QSharedGLTexture *m_sharedTexture;
};

#endif // ObservationLIST_H
