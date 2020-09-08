#ifndef PROJECTMANAGEMENT_H
#define PROJECTMANAGEMENT_H

#include <QObject>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include "utmconverter.h"
#include "structuralmodel.h"
#include "eventpermutation.h"
#include "eventlist.h"
#include "observationlist.h"

#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"

class ProjectManagement : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY (QString filename MEMBER m_filename NOTIFY filenameChanged)
    Q_PROPERTY (double minLatitude MEMBER m_minLatitude NOTIFY minLatitudeChanged)
    Q_PROPERTY (double maxLatitude MEMBER m_maxLatitude NOTIFY maxLatitudeChanged)
    Q_PROPERTY (double minLongitude MEMBER m_minLongitude NOTIFY minLongitudeChanged)
    Q_PROPERTY (double maxLongitude MEMBER m_maxLongitude NOTIFY maxLongitudeChanged)
    Q_PROPERTY (double mapCentreLatitude MEMBER m_mapCentreLatitude NOTIFY mapCentreLatitudeChanged)
    Q_PROPERTY (double mapCentreLongitude MEMBER m_mapCentreLongitude NOTIFY mapCentreLongitudeChanged)
    Q_PROPERTY (double minNorthing MEMBER m_minNorthing NOTIFY minNorthingChanged)
    Q_PROPERTY (double maxNorthing MEMBER m_maxNorthing NOTIFY maxNorthingChanged)
    Q_PROPERTY (double minEasting MEMBER m_minEasting NOTIFY minEastingChanged)
    Q_PROPERTY (double maxEasting MEMBER m_maxEasting NOTIFY maxEastingChanged)
    Q_PROPERTY (int utmZone MEMBER m_utmZone NOTIFY utmZoneChanged)
    Q_PROPERTY (int utmNorthSouth MEMBER m_utmNorthSouth NOTIFY utmNorthSouthChanged)
    Q_PROPERTY (QString utmNorthSouthStr MEMBER m_utmNorthSouthStr NOTIFY utmNorthSouthStrChanged)
    Q_PROPERTY (unsigned int spacingX MEMBER m_spacingX NOTIFY spacingXChanged)
    Q_PROPERTY (unsigned int spacingY MEMBER m_spacingY NOTIFY spacingYChanged)
    Q_PROPERTY (unsigned int spacingZ MEMBER m_spacingZ NOTIFY spacingZChanged)
    Q_PROPERTY (double minDepth MEMBER m_minDepth NOTIFY minDepthChanged)
    Q_PROPERTY (double maxDepth MEMBER m_maxDepth NOTIFY maxDepthChanged)
    Q_PROPERTY (bool inUtm MEMBER m_inUtm NOTIFY inUtmChanged)
    Q_PROPERTY (int mainIndex MEMBER m_mainIndex NOTIFY mainIndexChanged)
    Q_PROPERTY (bool extentsChanged MEMBER m_extentsChanged NOTIFY extentsChangedChanged)
    Q_PROPERTY (bool flowChoiceMade MEMBER m_flowChoiceMade NOTIFY flowChoiceMadeChanged)
    Q_PROPERTY (int flowChoice MEMBER m_flowChoice NOTIFY flowChoiceChanged)
    Q_PROPERTY (int loopStructuralFlowOption MEMBER m_loopStructuralFlowOption NOTIFY loopStructuralFlowOptionChanged)
    Q_PROPERTY (int sharedTextureId MEMBER m_sharedTextureId NOTIFY sharedTextureIdChanged)
    Q_PROPERTY (unsigned int xsize MEMBER m_xsize NOTIFY xsizeChanged)
    Q_PROPERTY (unsigned int ysize MEMBER m_ysize NOTIFY ysizeChanged)
    Q_PROPERTY (unsigned int zsize MEMBER m_zsize NOTIFY zsizeChanged)

    Q_SIGNALS:
        void filenameChanged();
        void minLatitudeChanged();
        void maxLatitudeChanged();
        void minLongitudeChanged();
        void maxLongitudeChanged();
        void mapCentreLatitudeChanged();
        void mapCentreLongitudeChanged();
        void minNorthingChanged();
        void maxNorthingChanged();
        void minEastingChanged();
        void maxEastingChanged();
        void utmZoneChanged();
        void utmNorthSouthChanged();
        void utmNorthSouthStrChanged();
        void spacingXChanged();
        void spacingYChanged();
        void spacingZChanged();
        void minDepthChanged();
        void maxDepthChanged();
        void inUtmChanged();
        void mainIndexChanged();
        void extentsChangedChanged();
        void flowChoiceMadeChanged();
        void flowChoiceChanged();
        void loopStructuralFlowOptionChanged();
        void sharedTextureIdChanged();
        void xsizeChanged();
        void ysizeChanged();
        void zsizeChanged();

    public:
        Q_INVOKABLE void clearProject(void);
        Q_INVOKABLE int saveProject(QString filename = "");
        Q_INVOKABLE int loadProject(QString filename = "");
        Q_INVOKABLE int reloadProject(void);
        Q_INVOKABLE void updateGeodeticLimits(double minLatitude, double maxLatitude, double minLongitude, double maxLongitude);
        Q_INVOKABLE void updateUTMLimits(double minNorthing, double maxNorthing, double minEasting, double maxEasting, int zone, int northSouth);
        Q_INVOKABLE void checkGeodeticLimits(void);
        Q_INVOKABLE void checkUTMLimits(void);
        Q_INVOKABLE void convertGeodeticToUTM(void);
        Q_INVOKABLE void convertUTMToGeodetic(void);

        Q_INVOKABLE Qt3DRender::QSharedGLTexture* getStructuralModelData(void);
        Q_INVOKABLE void downloadData(QString url = "", QString datatype = "netCDF");
        Q_INVOKABLE void incrementFlowChoice();
        Q_INVOKABLE void decrementFlowChoice();
        Q_INVOKABLE void loadTextures();

        static ProjectManagement* instance() {
            if (!m_instance) m_instance = new ProjectManagement;
            return m_instance;
        }
        StructuralModel* getStModel() { return &stModel; }
        EventList* getEventList() { return &eventList; }
        ObservationList* getObservationList() { return &observationList; }
        QQuickView* getQmlQuickView() { return m_qmlView; }
        void setQmlQuickView(QQuickView* view) {
            m_qmlView = view;
            connect(view,&QQuickWindow::beforeRendering,this, &ProjectManagement::loadTextures, Qt::DirectConnection);
        }

        double m_minLatitude;
        double m_maxLatitude;
        double m_minLongitude;
        double m_maxLongitude;
        double m_minNorthing;
        double m_maxNorthing;
        double m_minEasting;
        double m_maxEasting;
        double m_minDepth;
        double m_maxDepth;
        unsigned int m_spacingX;
        unsigned int m_spacingY;
        unsigned int m_spacingZ;
        unsigned int m_xsize;
        unsigned int m_ysize;
        unsigned int m_zsize;
        bool m_inUtm;
        int m_mainIndex;
        bool m_extentsChanged;
        bool m_flowChoiceMade;
        int m_flowChoice;
        int m_loopStructuralFlowOption;
        int m_sharedTextureId;
    private:
        ProjectManagement();
        static ProjectManagement* m_instance;

        double m_mapCentreLatitude;
        double m_mapCentreLongitude;
        int m_utmZone;
        int m_utmNorthSouth;
        QString m_utmNorthSouthStr;

        QString m_filename;
        QQuickView *m_qmlView;

        StructuralModel stModel;
        std::map<int,StructuralModel*> structuralModelMap;
        std::map<int,EventPermutation*> permutationMap;
        EventList eventList;
        ObservationList observationList;
};

#endif // PROJECTMANAGEMENT_H
