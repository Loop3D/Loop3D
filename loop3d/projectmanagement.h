#ifndef PROJECTMANAGEMENT_H
#define PROJECTMANAGEMENT_H

#include <QObject>
#include "utmconverter.h"
#include "structuralmodel.h"

class ProjectManagement : public QObject
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
    Q_PROPERTY (int spacingX MEMBER m_spacingX NOTIFY spacingXChanged)
    Q_PROPERTY (int spacingY MEMBER m_spacingY NOTIFY spacingYChanged)
    Q_PROPERTY (int spacingZ MEMBER m_spacingZ NOTIFY spacingZChanged)
    Q_PROPERTY (double minDepth MEMBER m_minDepth NOTIFY minDepthChanged)
    Q_PROPERTY (double maxDepth MEMBER m_maxDepth NOTIFY maxDepthChanged)

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

    public:
        Q_INVOKABLE void clearProject(void);
        Q_INVOKABLE int saveProject(QString filename = "");
        Q_INVOKABLE int loadProject(QString filename = "");
        Q_INVOKABLE void updateGeodeticLimits(double minLatitude, double maxLatitude, double minLongitude, double maxLongitude);
        Q_INVOKABLE void updateUTMLimits(double minNorthing, double maxNorthing, double minEasting, double maxEasting, int zone, int northSouth);
        Q_INVOKABLE void checkGeodeticLimits(void);
        Q_INVOKABLE void checkUTMLimits(void);
        Q_INVOKABLE void convertGeodeticToUTM(void);
        Q_INVOKABLE void convertUTMToGeodetic(void);

        Q_INVOKABLE void downloadData(QString url = "", QString datatype = "netCDF");


        static ProjectManagement* instance() {
            if (!m_instance)
                m_instance = new ProjectManagement;
                return m_instance;
        }
        StructuralModel* getStModel() { return &stModel; }

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
    private:
        ProjectManagement();
        static ProjectManagement* m_instance;

        double m_mapCentreLatitude;
        double m_mapCentreLongitude;
        int m_utmZone;
        int m_utmNorthSouth;
        QString m_utmNorthSouthStr;
        int m_spacingX;
        int m_spacingY;
        int m_spacingZ;
        QString m_filename;

        StructuralModel stModel;
};

#endif // PROJECTMANAGEMENT_H
