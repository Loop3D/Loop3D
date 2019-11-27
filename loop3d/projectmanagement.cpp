#include "projectmanagement.h"
#include <iostream>
#include <QtDebug>

#include <exception>
#include <netcdf>


ProjectManagement* ProjectManagement::m_instance = nullptr;

ProjectManagement::ProjectManagement():
    m_minDepth(1000),
    m_maxDepth(9000),
    m_utmZone(0),
    m_utmNorthSouth(-1),
    m_spacingX(100),
    m_spacingY(100),
    m_spacingZ(100),
  m_filename("")
{
}

void ProjectManagement::clearProject(void)
{
    m_utmZone = 0;
    m_utmNorthSouth = 0;
    m_utmNorthSouthStr = "-";
    m_minLatitude = 0;
    m_maxLatitude = 0;
    m_minLongitude = 0;
    m_maxLongitude = 0;
    m_minNorthing = 0;
    m_maxNorthing = 0;
    m_minEasting = 0;
    m_maxEasting = 0;
    m_filename = "";
    m_mapCentreLatitude = 0;
    m_mapCentreLongitude = 0;
    m_minDepth = 1000.0;
    m_maxDepth = 9000.0;
    m_spacingX = 100;
    m_spacingY = 100;
    m_spacingZ = 100;
    minLatitudeChanged(); maxLatitudeChanged(); minLongitudeChanged(); maxLongitudeChanged();
    minNorthingChanged(); maxNorthingChanged(); minEastingChanged(); maxEastingChanged();
    utmZoneChanged(); utmNorthSouthChanged(); utmNorthSouthStrChanged();
    spacingXChanged(); spacingYChanged(); spacingZChanged();
}

int ProjectManagement::saveProject(QString filename)
{
    if (filename == "") {
        filename = m_filename;
    }
    checkUTMLimits();

    try {
        // Find last '/' of the first set of '/'s as in file:/// or url:///
        QStringList list = filename.split(QRegExp("///"));
        QString name = (list.length() > 1 ? list[1] : list[0]);

        netCDF::NcFile dataFile(name.toStdString().c_str(), netCDF::NcFile::replace);

        // Save global information
        dataFile.putAtt("minLatitude",netCDF::ncDouble,m_minLatitude);
        dataFile.putAtt("maxLatitude",netCDF::ncDouble,m_maxLatitude);
        dataFile.putAtt("minLongitude",netCDF::ncDouble,m_minLongitude);
        dataFile.putAtt("maxLongitude",netCDF::ncDouble,m_maxLongitude);
        dataFile.putAtt("minNorthing",netCDF::ncDouble,m_minNorthing);
        dataFile.putAtt("maxNorthing",netCDF::ncDouble,m_maxNorthing);
        dataFile.putAtt("minEasting",netCDF::ncDouble,m_minEasting);
        dataFile.putAtt("maxEasting",netCDF::ncDouble,m_maxEasting);
        dataFile.putAtt("minDepth",netCDF::ncDouble,m_minDepth);
        dataFile.putAtt("maxDepth",netCDF::ncDouble,m_maxDepth);
        dataFile.putAtt("utmZone",netCDF::ncInt64,m_utmZone);
        dataFile.putAtt("utmNorthSouth",netCDF::ncInt64,m_utmNorthSouth);
        dataFile.putAtt("spacingX",netCDF::ncInt64,m_spacingX);
        dataFile.putAtt("spacingY",netCDF::ncInt64,m_spacingY);
        dataFile.putAtt("spacingZ",netCDF::ncInt64,m_spacingZ);

        dataFile.close();

        // Data file opened so save filename
        m_filename = filename;
        filenameChanged();

    } catch(netCDF::exceptions::NcException& e) {
        qFatal("%s", e.what());
        qFatal("Error creating file (%s)", filename.toStdString().c_str());
        return 1;
    }
    // Save structural data
    stModel.saveToFile(filename);
    return 0;
}

int ProjectManagement::loadProject(QString filename)
{
    if (filename == "") {
        return 1;
    }
    try {
        // Load variables to staging area to confirm all loaded in
        double minLatitude, maxLatitude, minLongitude, maxLongitude;
        double minNorthing, maxNorthing, minEasting, maxEasting;
        double minDepth, maxDepth;
        int64_t utmZone, utmNorthSouth;
        int64_t spacingX, spacingY, spacingZ;

        QStringList list = filename.split(QRegExp("///"));
        QString name = (list.length() > 1 ? list[1] : list[0]);

        netCDF::NcFile dataFile(name.toStdString().c_str(), netCDF::NcFile::read);

        dataFile.getAtt("minLatitude").getValues(&minLatitude);
        dataFile.getAtt("maxLatitude").getValues(&maxLatitude);
        dataFile.getAtt("minLongitude").getValues(&minLongitude);
        dataFile.getAtt("maxLongitude").getValues(&maxLongitude);
        dataFile.getAtt("minNorthing").getValues(&minNorthing);
        dataFile.getAtt("maxNorthing").getValues(&maxNorthing);
        dataFile.getAtt("minEasting").getValues(&minEasting);
        dataFile.getAtt("maxEasting").getValues(&maxEasting);
        dataFile.getAtt("utmZone").getValues(&utmZone);
        dataFile.getAtt("utmNorthSouth").getValues(&utmNorthSouth);
        dataFile.getAtt("minDepth").getValues(&minDepth);
        dataFile.getAtt("maxDepth").getValues(&maxDepth);
        dataFile.getAtt("spacingX").getValues(&spacingX);
        dataFile.getAtt("spacingY").getValues(&spacingY);
        dataFile.getAtt("spacingZ").getValues(&spacingZ);

        dataFile.close();

        // All loaded to set values into structure
        m_utmZone = static_cast<int>(utmZone);
        m_utmNorthSouth = static_cast<int>(utmNorthSouth);
        m_utmNorthSouthStr = (utmNorthSouth == -1 ? "-" : (utmNorthSouth == 1 ? "N" : "S"));
        m_minNorthing = minNorthing;
        m_maxNorthing = maxNorthing;
        m_minEasting = minEasting;
        m_maxEasting = maxEasting;
        m_minDepth = minDepth;
        m_maxDepth = maxDepth;
        m_spacingX = static_cast<int>(spacingX);
        m_spacingY = static_cast<int>(spacingY);
        m_spacingZ = static_cast<int>(spacingZ);
        updateGeodeticLimits(minLatitude,maxLatitude,minLongitude,maxLongitude);

        minDepthChanged(); maxDepthChanged();
        spacingXChanged(); spacingYChanged(); spacingZChanged();
        // Data file all working so save filename
        m_filename = filename;
        filenameChanged();
    } catch(netCDF::exceptions::NcException& e) {
        qFatal("%s", e.what());
        qFatal("Error loading file (%s)", filename.toStdString().c_str());
        return 1;
    }

    // Load structural data
    stModel.loadFromFile(filename);
    return 0;
}

void ProjectManagement::updateGeodeticLimits(double minLatitude, double maxLatitude, double minLongitude, double maxLongitude)
{
    m_minLatitude = minLatitude;
    m_maxLatitude = maxLatitude;
    m_minLongitude = minLongitude;
    m_maxLongitude = maxLongitude;
    checkGeodeticLimits();
    minLatitudeChanged(); maxLatitudeChanged(); minLongitudeChanged(); maxLongitudeChanged();
    convertGeodeticToUTM();
}

void ProjectManagement::convertGeodeticToUTM(void)
{
    UTMExtents utmExtents(0,0,0,0,0,0);
    LLExtents llExtents(m_minLatitude,m_maxLatitude,m_minLongitude, m_maxLongitude);
    utmExtents.fromLL(&llExtents,m_utmZone,m_utmNorthSouth);
    m_minNorthing = utmExtents.m_minNorthing;
    m_maxNorthing = utmExtents.m_maxNorthing;
    m_minEasting = utmExtents.m_minEasting;
    m_maxEasting = utmExtents.m_maxEasting;
    m_utmZone = utmExtents.m_zone;
    m_utmNorthSouth = utmExtents.m_north;
    m_utmNorthSouthStr = (m_utmNorthSouth == -1 ? "-" : (m_utmNorthSouth == 0 ? "S" : "N"));
    m_mapCentreLatitude = (m_minLatitude + m_maxLatitude )/2.0;
    m_mapCentreLongitude = (m_minLongitude + m_maxLongitude )/2.0;
    minNorthingChanged(); maxNorthingChanged(); minEastingChanged(); maxEastingChanged();
    utmZoneChanged(); utmNorthSouthChanged(); utmNorthSouthStrChanged();
}

void ProjectManagement::updateUTMLimits(double minNorthing, double maxNorthing, double minEasting, double maxEasting, int zone, int northSouth)
{
    m_minNorthing = minNorthing;
    m_maxNorthing = maxNorthing;
    m_minEasting = minEasting;
    m_maxEasting = maxEasting;
    m_utmZone = zone;
    m_utmNorthSouth = northSouth;
    m_utmNorthSouthStr = (m_utmNorthSouth == -1 ? "-" : (m_utmNorthSouth == 0 ? "S" : "N"));
    checkUTMLimits();
    minNorthingChanged(); maxNorthingChanged(); minEastingChanged(); maxEastingChanged();
    utmZoneChanged(); utmNorthSouthChanged(); utmNorthSouthStrChanged();
    convertUTMToGeodetic();

}

void ProjectManagement::convertUTMToGeodetic(void)
{
    LLExtents llExtents(0,0,0,0);
    UTMExtents utmExtents(m_minNorthing, m_maxNorthing, m_minEasting, m_maxEasting, m_utmZone, m_utmNorthSouth);
    llExtents.fromUTM(&utmExtents,m_utmZone,m_utmNorthSouth);
    m_minLatitude = llExtents.m_minLatitude;
    m_maxLatitude = llExtents.m_maxLatitude;
    m_minLongitude = llExtents.m_minLongitude;
    m_maxLongitude = llExtents.m_maxLongitude;
    m_mapCentreLatitude = (m_minLatitude + m_maxLatitude )/2.0;
    m_mapCentreLongitude = (m_minLongitude + m_maxLongitude )/2.0;
    minLatitudeChanged(); maxLatitudeChanged(); minLongitudeChanged(); maxLongitudeChanged();
}

void ProjectManagement::checkGeodeticLimits()
{
    if (m_minLatitude > m_maxLatitude) {
        double tmp = m_minLatitude;
        m_minLatitude = m_maxLatitude;
        m_maxLatitude = tmp;
        minLatitudeChanged(); maxLatitudeChanged();
    }
    if (m_minLongitude > m_maxLongitude) {
        double tmp = m_minLongitude;
        m_minLongitude = m_maxLongitude;
        m_maxLongitude = tmp;
        minLongitudeChanged(); maxLongitudeChanged();
    }
}

void ProjectManagement::checkUTMLimits()
{
    if (m_minNorthing > m_maxNorthing) {
        double tmp = m_minNorthing;
        m_minNorthing = m_maxNorthing;
        m_maxNorthing = tmp;
        minNorthingChanged(); maxNorthingChanged();
    }
    if (m_minEasting > m_maxEasting) {
        double tmp = m_minEasting;
        m_minEasting = m_maxEasting;
        m_maxEasting = tmp;
        minEastingChanged(); maxEastingChanged();
    }
}

void ProjectManagement::downloadData(QString url, QString datatype)
{
    if (datatype == "netCDF") {
        try {
            QStringList list = url.split(QRegExp("///"));
            QString name = (list.length() > 1 ? list[1] : list[0]);

            netCDF::NcFile dataFile(name.toStdString().c_str(), netCDF::NcFile::read);
            auto attrs = dataFile.getAtts();
            for (auto i : attrs) {
                std::cout << i.first << std::endl;
            }
        } catch (std::exception e) {
            qDebug() << e.what();
        }
    } else if (datatype == "") {

    }
}
