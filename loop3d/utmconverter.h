#ifndef UTMCONVERTER_H
#define UTMCONVERTER_H

#include <math.h>
#include <QObject>
#include <QQmlEngine>
#include <QGeoCoordinate>

class UTM;
class UTMExtents;

class LL : public QObject {
    Q_OBJECT

    Q_PROPERTY (double latitude MEMBER m_latitude)
    Q_PROPERTY (double longitude MEMBER m_longitude)

    public:
        explicit LL(QObject* parent = nullptr);
        LL(double lat, double lon);
        virtual ~LL() {}

        Q_INVOKABLE void fromUTM(UTM* utm, int in_zone = 0, int in_ns = -1);

        double m_latitude;
        double m_longitude;
    private:
};

class UTM : public QObject {
    Q_OBJECT

    Q_PROPERTY (double northing MEMBER m_northing)
    Q_PROPERTY (double easting MEMBER m_easting)
    Q_PROPERTY (int zone MEMBER m_zone)
    Q_PROPERTY (int north MEMBER m_north)
    Q_PROPERTY (QString northStr MEMBER m_northStr)

    public:
        explicit UTM(QObject* parent = nullptr);
        UTM(double northing, double easting, int zone, int north);
        virtual ~UTM() {}

        Q_INVOKABLE void fromLL(LL*, int in_zone = 0, int in_ns = -1);

        double m_easting;
        double m_northing;
        int m_zone;
        int m_north;
        QString m_northStr;
    private:
};

class LLExtents : public QObject {
    Q_OBJECT

    Q_PROPERTY (double minLatitude MEMBER m_minLatitude NOTIFY minLatitudeChanged)
    Q_PROPERTY (double minLongitude MEMBER m_minLongitude NOTIFY minLongitudeChanged)
    Q_PROPERTY (double maxLatitude MEMBER m_maxLatitude NOTIFY maxLatitudeChanged)
    Q_PROPERTY (double maxLongitude MEMBER m_maxLongitude NOTIFY maxLongitudeChanged)

    Q_SIGNALS:
        void minLatitudeChanged();
        void minLongitudeChanged();
        void maxLatitudeChanged();
        void maxLongitudeChanged();

    public:
        explicit LLExtents(QObject* parent = nullptr);
        LLExtents(double minlat, double maxlat, double minlon, double maxlon);
        virtual ~LLExtents() {}

        Q_INVOKABLE void fromUTM(UTMExtents* utm, int in_zone = 0, int in_ns = -1);

        double m_minLatitude;
        double m_minLongitude;
        double m_maxLatitude;
        double m_maxLongitude;
};

class UTMExtents: public QObject {
    Q_OBJECT

    Q_PROPERTY (double minNorthing MEMBER m_minNorthing NOTIFY minNorthingChanged)
    Q_PROPERTY (double minEasting MEMBER m_minEasting NOTIFY minEastingChanged)
    Q_PROPERTY (double maxNorthing MEMBER m_maxNorthing NOTIFY maxNorthingChanged)
    Q_PROPERTY (double maxEasting MEMBER m_maxEasting NOTIFY maxEastingChanged)
    Q_PROPERTY (int zone MEMBER m_zone NOTIFY zoneChanged)
    Q_PROPERTY (int north MEMBER m_north NOTIFY northChanged)
    Q_PROPERTY (QString northStr MEMBER m_northStr NOTIFY northStrChanged)

    Q_SIGNALS:
        void minNorthingChanged();
        void minEastingChanged();
        void maxNorthingChanged();
        void maxEastingChanged();
        void zoneChanged();
        void northChanged();
        void northStrChanged();

    private Q_SLOTS:
        void recalcNorthStr(void);

    public:
        explicit UTMExtents(QObject* parent = nullptr);
        UTMExtents(double minnorthing, double maxnorthing, double mineasting, double maxeasting, int zone, int north);
        virtual ~UTMExtents() {}

        Q_INVOKABLE void fromLL(LLExtents* ll, int in_zone = 0, int in_ns = -1);

        double m_minNorthing;
        double m_minEasting;
        double m_maxNorthing;
        double m_maxEasting;
        int m_zone;
        int m_north;
        QString m_northStr;
};

#endif // UTMCONVERTER_H
