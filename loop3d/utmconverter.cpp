#include "utmconverter.h"
#include <math.h>
#include <iostream>

// contant values from https://en.wikipedia.org/wiki/Universal_Transverse_Mercator_coordinate_system
constexpr static const double rad = 6378137.0;
constexpr static const double f = 1.0 / 298.257223563;
constexpr static const double n = f / (2 - f);
constexpr static const double E0 = 500000.0;
constexpr static const double k0 = 0.9996;
constexpr static const double A = rad / (1 + n) * (1 + (n*n)/4 + (n*n*n*n)/64);
constexpr static const double a[] = { n/2 - 2/3*n*n + 5/16*n*n*n,
                                      13/48*n*n - 3/5*n*n*n,
                                      61/240*n*n*n };
constexpr static const double b[] = { n/2 - 2/3*n*n + 37/96*n*n*n,
                                      n*n/48 + n*n*n/15,
                                      17/480*n*n*n };
constexpr static const double d[] = { 2*n - 2/3*n*n - 2*n*n*n,
                                      7/3*n*n - 8/5*n*n*n,
                                      56/15*n*n*n };
constexpr static const double pi = 3.1415926535897;
constexpr static const double deg2rad = pi/180.0;

LL::LL(QObject* parent) : QObject(parent), m_latitude(0), m_longitude(0) {}
LL::LL(double lat, double lon) { m_latitude = lat; m_longitude = lon; }

void LL::fromUTM(UTM* utm, int in_zone, int in_ns)
{
    int zone = (in_zone == 0 ? utm->m_zone : in_zone);
    bool ns = (in_ns == -1 ? utm->m_north : in_ns);
    double N0 = (ns?0.0:10000000.0);
    double lambda0 = zone * 6 - 183;
    double eps = (utm->m_northing - N0)/(k0*A);
    double eta = (utm->m_easting  - E0)/(k0*A);
    double eps1 = eps - (b[0]*sin(2*eps)*cosh(2*eta) + b[1]*sin(4*eps)*cosh(4*eta) + b[2]*sin(6*eps)*cosh(6*eta));
    double eta1 = eta - (b[0]*cos(2*eps)*sinh(2*eta) + b[1]*cos(4*eps)*sinh(4*eta) + b[2]*cos(6*eps)*sinh(6*eta));
    double x = asin(sin(eps1)/cosh(eta1));
    m_latitude = (x + d[0]*sin(2*x) + d[1]*sin(4*x) + d[2]*sin(6*x)) / deg2rad;
    m_longitude = (lambda0 + atan(sinh(eta1)/cos(eps1)) / deg2rad);
}

UTM::UTM(QObject* parent) : QObject(parent), m_easting(0), m_northing(0), m_zone(0), m_north(-1), m_northStr("-")  {}

UTM::UTM(double northing, double easting, int zone, int north)
{
    m_northing = northing;
    m_easting = easting;
    m_zone = zone;
    m_north = north;
    m_northStr = (m_north == -1 ? "-" : (m_north == 0 ? "S" : "N"));
}

void UTM::fromLL(LL* ll, int in_zone, int in_ns)
{
    if (in_zone == 0) m_zone = static_cast<int>((ll->m_longitude+180) / 6)+1;
    else m_zone = in_zone;
    if (in_ns == -1) m_north = ll->m_latitude > 0;
    else m_north = in_ns;
    m_northStr = (m_north == -1 ? "-" : (m_north == 0 ? "S" : "N"));
    double N0 = (m_north ? 0 : 10000000);
    double lambda0 = m_zone * 6 - 183;
    double t = sinh(atanh(sin(ll->m_latitude*deg2rad)) - 2*sqrt(n)/(1+n)*atanh(2*sqrt(n)/(1+n)*sin(ll->m_latitude*deg2rad)));
    double eps = atan(t/cos((ll->m_longitude - lambda0)*deg2rad));
    double eta = atanh(sin((ll->m_longitude - lambda0)*deg2rad)/sqrt(1+t*t));
    m_easting  = E0 + k0 * A * (eta + a[0]*cos(2*eps)*sinh(2*eta) + a[1]*cos(4*eps)*sinh(4*eta) + a[2]*cos(6*eps)*sinh(6*eta));
    m_northing = N0 + k0 * A * (eps + a[0]*sin(2*eps)*cosh(2*eta) + a[1]*sin(4*eps)*cosh(4*eta) + a[2]*sin(6*eps)*cosh(6*eta));
}

LLExtents::LLExtents(QObject* parent) : QObject(parent) {}

LLExtents::LLExtents(double minlat, double maxlat, double minlon, double maxlon) {
    m_minLatitude = minlat;
    m_maxLatitude = maxlat;
    m_minLongitude = minlon;
    m_maxLongitude = maxlon;
}

void LLExtents::fromUTM(UTMExtents* utm, int in_zone, int in_ns)
{
    if (in_zone == 0) in_zone = utm->m_zone;
    if (in_ns == -1) in_ns = utm->m_north;
    UTM northBorder(utm->m_maxNorthing, (utm->m_minEasting + utm->m_maxEasting) / 2.0, in_zone, in_ns);
    UTM southBorder(utm->m_minNorthing, (utm->m_minEasting + utm->m_maxEasting) / 2.0, in_zone, in_ns);
    UTM eastBorder((utm->m_minNorthing + utm->m_maxNorthing) / 2.0, utm->m_maxEasting, in_zone, in_ns);
    UTM westBorder((utm->m_minNorthing + utm->m_maxNorthing) / 2.0, utm->m_minEasting, in_zone, in_ns);
    LL llNorthBorder;
    LL llSouthBorder;
    LL llEastBorder;
    LL llWestBorder;
    llNorthBorder.fromUTM(&northBorder);
    llSouthBorder.fromUTM(&southBorder);
    llEastBorder.fromUTM(&eastBorder);
    llWestBorder.fromUTM(&westBorder);
    m_minLatitude = llSouthBorder.m_latitude;
    m_maxLatitude = llNorthBorder.m_latitude;
    m_minLongitude = llWestBorder.m_longitude;
    m_maxLongitude = llEastBorder.m_longitude;
}

UTMExtents::UTMExtents(QObject* parent) : QObject(parent)
{
    m_northStr = "-";
    connect(this,SIGNAL(northChanged()),this,SLOT(recalcNorthStr()));
}

UTMExtents::UTMExtents(double minnorthing, double maxnorthing, double mineasting, double maxeasting, int zone, int north) {
    m_minNorthing = minnorthing;
    m_maxNorthing = maxnorthing;
    m_minEasting = mineasting;
    m_maxEasting = maxeasting;
    m_zone = zone;
    m_north = north;
    m_northStr = (m_north == -1 ? "-" : (m_north == 0 ? "S" : "N"));
}

void UTMExtents::fromLL(LLExtents* ll, int in_zone, int in_ns)
{
    LL northBorder(ll->m_maxLatitude, (ll->m_minLongitude + ll->m_maxLongitude)/2.0);
    LL southBorder(ll->m_minLatitude, (ll->m_minLongitude + ll->m_maxLongitude)/2.0);
    LL eastBorder((ll->m_minLatitude + ll->m_maxLatitude) / 2.0, ll->m_maxLongitude);
    LL westBorder((ll->m_minLatitude + ll->m_maxLatitude) / 2.0, ll->m_minLongitude);
    UTM utmNorthBorder;
    UTM utmSouthBorder;
    UTM utmEastBorder;
    UTM utmWestBorder;
    utmNorthBorder.fromLL(&northBorder, in_zone, in_ns);
    if (in_zone == 0) in_zone = utmNorthBorder.m_zone;
    if (in_ns == -1) in_ns = utmNorthBorder.m_north;
    utmSouthBorder.fromLL(&southBorder, in_zone, in_ns);
    utmEastBorder.fromLL(&eastBorder, in_zone, in_ns);
    utmWestBorder.fromLL(&westBorder, in_zone, in_ns);
    m_minNorthing = utmSouthBorder.m_northing;
    m_maxNorthing = utmNorthBorder.m_northing;
    m_minEasting  = utmWestBorder.m_easting;
    m_maxEasting  = utmEastBorder.m_easting;
    m_zone = in_zone;
    m_north = in_ns;
    m_northStr = (m_north == -1 ? "-" : (m_north == 0 ? "S" : "N"));
    northStrChanged();
}

void UTMExtents::recalcNorthStr(void)
{
    m_northStr = (m_north == -1 ? "-" : (m_north == 0 ? "S" : "N"));
    northStrChanged();
}


