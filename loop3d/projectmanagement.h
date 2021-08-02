#ifndef PROJECTMANAGEMENT_H
#define PROJECTMANAGEMENT_H

#include <QObject>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>
#include "utmconverter.h"
#include "structuralmodel.h"
#include "eventpermutation.h"
#include "datasourcelist.h"
#include "eventlist.h"
#include "observationlist.h"
#include "m2lconfig.h"
#include "lsconfig.h"

#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"

class ProjectManagement : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY (bool quiting MEMBER m_quiting NOTIFY quitingChanged)
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
    Q_PROPERTY (double botExtent MEMBER m_botExtent NOTIFY botExtentChanged)
    Q_PROPERTY (double topExtent MEMBER m_topExtent NOTIFY topExtentChanged)
    Q_PROPERTY (bool inUtm MEMBER m_inUtm NOTIFY inUtmChanged)
    Q_PROPERTY (int mainIndex MEMBER m_mainIndex NOTIFY mainIndexChanged)
    Q_PROPERTY (bool extentsChanged MEMBER m_extentsChanged NOTIFY extentsChangedChanged)
    Q_PROPERTY (bool flowChoiceMade MEMBER m_flowChoiceMade NOTIFY flowChoiceMadeChanged)
    Q_PROPERTY (int flowChoice MEMBER m_flowChoice NOTIFY flowChoiceChanged)
    Q_PROPERTY (int loopStructuralFlowOption MEMBER m_loopStructuralFlowOption NOTIFY loopStructuralFlowOptionChanged)
    Q_PROPERTY (int sharedTextureId MEMBER m_sharedTextureId NOTIFY sharedTextureIdChanged)
    Q_PROPERTY (int observationsTextureId MEMBER m_observationsTextureId NOTIFY observationsTextureIdChanged)
    Q_PROPERTY (int numObservations MEMBER m_numObservations NOTIFY numObservationsChanged)
    Q_PROPERTY (unsigned int xsize MEMBER m_xsize NOTIFY xsizeChanged)
    Q_PROPERTY (unsigned int ysize MEMBER m_ysize NOTIFY ysizeChanged)
    Q_PROPERTY (unsigned int zsize MEMBER m_zsize NOTIFY zsizeChanged)
    Q_PROPERTY (bool lockedExtents MEMBER m_lockedExtents WRITE setLockedExtents NOTIFY lockedExtentsChanged)
    Q_PROPERTY (unsigned int activeState MEMBER m_activeState WRITE setActiveState NOTIFY activeStateChanged)
    Q_PROPERTY (QString activeStateName MEMBER m_activeStateName NOTIFY activeStateNameChanged)
    Q_PROPERTY (bool useLavavu MEMBER m_useLavavu WRITE setUseLavavu NOTIFY useLavavuChanged)
    Q_PROPERTY (QString pythonErrors MEMBER m_pythonErrors NOTIFY pythonErrorsChanged)
    Q_PROPERTY (double pythonInProgress MEMBER m_pythonInProgress NOTIFY pythonInProgressChanged)
    Q_PROPERTY (QString pythonProgressText MEMBER m_pythonProgressText NOTIFY pythonProgressTextChanged)
    Q_PROPERTY (int pythonProgressTextLineCount MEMBER m_pythonProgressTextLineCount NOTIFY pythonProgressTextLineCountChanged)
    Q_PROPERTY (unsigned long long totalPermutations MEMBER m_totalPermutations NOTIFY totalPermutationsChanged)

    Q_SIGNALS:
        void quitingChanged();
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
        void botExtentChanged();
        void topExtentChanged();
        void inUtmChanged();
        void mainIndexChanged();
        void extentsChangedChanged();
        void flowChoiceMadeChanged();
        void flowChoiceChanged();
        void loopStructuralFlowOptionChanged();
        void sharedTextureIdChanged();
        void observationsTextureIdChanged();
        void numObservationsChanged();
        void xsizeChanged();
        void ysizeChanged();
        void zsizeChanged();
        void lockedExtentsChanged();
        void activeStateChanged();
        void activeStateNameChanged();
        void useLavavuChanged();
        void pythonErrorsChanged();
        void pythonInProgressChanged();
        void pythonProgressTextChanged();
        void pythonProgressTextLineCountChanged();
        void totalPermutationsChanged();

        void finishedMap2Loop();
        void finishedLoopStructural();
        void finishedGeologyModel();

public Q_SLOTS:
        void setLockedExtents(bool val) { m_lockedExtents = val; Q_EMIT lockedExtentsChanged(); }
        void setActiveState(unsigned int val) { m_activeState = val; Q_EMIT activeStateChanged(); }
        bool setActiveState(QString state);
        void setUseLavavu(bool val) { m_useLavavu = val; Q_EMIT useLavavuChanged(); }

    public:
        Q_INVOKABLE void clearProject(bool clearExtents);
        Q_INVOKABLE void deleteProject(void);
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
        Q_INVOKABLE Qt3DRender::QSharedGLTexture* getObservationData(void);
        Q_INVOKABLE void downloadData(QString url = "", QString datatype = "netCDF");
        Q_INVOKABLE void incrementFlowChoice();
        Q_INVOKABLE void decrementFlowChoice();
        Q_INVOKABLE void loadTextures();
        Q_INVOKABLE bool hasFilename() { return m_filename != "";}

        static ProjectManagement* instance() {
            if (!m_instance) m_instance = new ProjectManagement;
            return m_instance;
        }
        StructuralModel* getStModel() { return &stModel; }
        EventList* getEventList() { return &eventList; }
        EventRelationshipList* getEventRelationshipList() { return &eventRelationshipList; }
        ObservationList* getObservationList() { return &observationList; }
        M2lConfig* getM2lConfig() { return &m2lConfig; }
        DataSourceList* getDataSourceList() { return &dataSourceList; }
        LSConfig* getLSConfig() { return &lsConfig; }
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
        int m_utmZone;
        int m_utmNorthSouth;
        double m_botExtent;
        double m_topExtent;
        unsigned int m_spacingX;
        unsigned int m_spacingY;
        unsigned int m_spacingZ;
        unsigned int m_xsize;
        unsigned int m_ysize;
        unsigned int m_zsize;
        bool m_lockedExtents;
        bool m_inUtm;
        int m_mainIndex;
        bool m_extentsChanged;
        bool m_flowChoiceMade;
        int m_flowChoice;
        int m_loopStructuralFlowOption;
        int m_sharedTextureId;
        int m_observationsTextureId;
        int m_numObservations;
        unsigned int m_activeState;
        QString m_activeStateName;
        bool m_useLavavu;

        // Currently project to store selected Urls
        // TODO: multiple urls that can be added as a list to map2loop
        std::string m_structureUrl;
        std::string m_geologyUrl;
        std::string m_faultUrl;
        std::string m_foldUrl;
        std::string m_mindepUrl;
        std::string m_metadataUrl;
        unsigned long long m_totalPermutations;

        QString m_pythonErrors;
        QString m_filename;
        double m_pythonInProgress;
        QString m_pythonProgressText;
        int m_pythonProgressTextLineCount;

        bool m_quiting;
    private:
        ProjectManagement();
        static ProjectManagement* m_instance;

        double m_mapCentreLatitude;
        double m_mapCentreLongitude;
        QString m_utmNorthSouthStr;

        QQuickView *m_qmlView;

        StructuralModel stModel;
        std::map<int,StructuralModel*> structuralModelMap;
        std::map<int,EventPermutation*> permutationMap;
        EventList eventList;
        EventRelationshipList eventRelationshipList;
        ObservationList observationList;
        M2lConfig m2lConfig;
        LSConfig lsConfig;
        DataSourceList dataSourceList;
};

#endif // PROJECTMANAGEMENT_H
