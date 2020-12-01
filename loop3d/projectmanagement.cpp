#include "projectmanagement.h"
#include <iostream>
#include <QtDebug>
#include <QtQuick/qquickwindow.h>

#include <exception>
#include <netcdf>
#include <sys/types.h>
#include <stdio.h>


ProjectManagement* ProjectManagement::m_instance = nullptr;

ProjectManagement::ProjectManagement():
    m_utmZone(0),
    m_utmNorthSouth(-1),
    m_botExtent(-12000),
    m_topExtent(1200),
    m_spacingX(1000),
    m_spacingY(1000),
    m_spacingZ(300),
    m_xsize(51),
    m_ysize(51),
    m_zsize(51),
    m_lockedExtents(false),
    m_inUtm(false),
    m_extentsChanged(true),
    m_flowChoiceMade(false),
    m_flowChoice(1),
    m_loopStructuralFlowOption(2),
    m_sharedTextureId(0),
    m_activeState(1),
    m_activeStateName("WA"),
    m_useLavavu(false),
    m_structureUrl(""),
    m_geologyUrl(""),
    m_faultUrl(""),
    m_foldUrl(""),
    m_mindepUrl(""),
    m_metadataUrl(""),
    m_filename("")
{

}

void ProjectManagement::clearProject(bool clearExtents)
{
    if (clearExtents) {
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
        m_botExtent = -12000.0;
        m_topExtent = 1200.0;
        m_spacingX = 1000;
        m_spacingY = 1000;
        m_spacingZ = 300;
        m_xsize = 51;
        m_ysize = 51;
        m_zsize = 51;
        m_extentsChanged = true;
        m_inUtm = false;
        m_flowChoiceMade = false;
        m_flowChoice = 1;
        m_loopStructuralFlowOption = 2;
        minLatitudeChanged(); maxLatitudeChanged(); minLongitudeChanged(); maxLongitudeChanged();
        minNorthingChanged(); maxNorthingChanged(); minEastingChanged(); maxEastingChanged();
        utmZoneChanged(); utmNorthSouthChanged(); utmNorthSouthStrChanged();
        spacingXChanged(); spacingYChanged(); spacingZChanged(); extentsChangedChanged();
        flowChoiceChanged(); flowChoiceMadeChanged(); loopStructuralFlowOptionChanged();
    }
    m_sharedTextureId = 0;
    eventList.clearList();
    observationList.clearList();
    stModel.clearData();
    m_lockedExtents = false;
    if (hasFilename()) saveProject(m_filename);
}

void ProjectManagement::deleteProject()
{
    QString filename = m_filename;
    QStringList list;
    QString name;
    // Find last '/' of the first set of '/'s as in file:/// or url:///
#ifdef _WIN32
    list = filename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = filename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    struct stat bf;
    // Check whether file exists
    bool loopFileExists = stat(name.toStdString().c_str(), &bf) == 0;

    if (loopFileExists) {
        qDebug() << "Deleting project file " << name;
        if (remove(name.toStdString().c_str()))
            qDebug() << "ERROR: Failed to delete project file " << name;
        else
            qDebug() << "Successfully deleted project file " << name;
    }
}

int ProjectManagement::saveProject(QString filename)
{
    if (filename == "") {
        filename = m_filename;
    }
    qDebug() << "Saving to file " << filename;
    checkUTMLimits();
//    int64_t inUtm = m_inUtm;

    QStringList list;
    QString name;
    // Find last '/' of the first set of '/'s as in file:/// or url:///
#ifdef _WIN32
    list = filename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = filename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    struct stat bf;
    // Check whether file exists
    bool loopFileExists = stat(name.toStdString().c_str(), &bf) == 0;

    LoopProjectFileResponse resp = {0,""};
    if (!loopFileExists) {
        resp = LoopProjectFile::CreateBasicFile(name.toStdString());
        if (resp.errorCode) {
            std::cout << resp.errorMessage << std::endl;
            return 1;
        }
    }
    LoopProjectFile::LoopExtents extents;
    extents.minLatitude = m_minLatitude;
    extents.maxLatitude = m_maxLatitude;
    extents.minLongitude = m_minLongitude;
    extents.maxLongitude = m_maxLongitude;
    extents.minEasting = m_minEasting;
    extents.maxEasting = m_maxEasting;
    extents.minNorthing = m_minNorthing;
    extents.maxNorthing = m_maxNorthing;
    extents.topDepth = m_topExtent;
    extents.bottomDepth = m_botExtent;
    extents.utmZone = m_utmZone;
    extents.utmNorthSouth = m_utmNorthSouth;
    extents.spacingX = m_spacingX;
    extents.spacingY = m_spacingY;
    extents.spacingZ = m_spacingZ;
    extents.workingFormat = m_inUtm;
    resp = LoopProjectFile::SetExtents(name.toStdString(),extents,true);
    if (resp.errorCode) {
        qDebug() << resp.errorMessage.c_str();
        return 1;
    }

    m_filename = filename;
    filenameChanged();

    stModel.saveToFile(filename);
    eventList.saveToFile(filename);
    observationList.saveToFile(filename);

    m_extentsChanged = false;
    m_lockedExtents = true;
    lockedExtentsChanged();
    return 0;
}

int ProjectManagement::loadProject(QString filename)
{
    if (filename == "") {
        return 1;
    }

    QStringList list;
    QString name;

#ifdef _WIN32
    list = filename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = filename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    struct stat bf;
    // Check whether file exists
    bool loopFileExists = stat(name.toStdString().c_str(), &bf) == 0;
    if (!loopFileExists) {
        std::cout << "Loop Project file " << name.toStdString() << " does not exist!" << std::endl;
        return 1;
    }

    LoopProjectFileResponse resp = {0,""};
    LoopProjectFile::LoopExtents extents;
    resp = LoopProjectFile::GetExtents(name.toStdString(),extents,true);
    if (resp.errorCode) {
        qDebug() << resp.errorMessage.c_str();
        return 1;
    }
    m_minLatitude = extents.minLatitude;
    m_maxLatitude = extents.maxLatitude;
    m_minLongitude = extents.minLongitude;
    m_maxLongitude = extents.maxLongitude;
    m_minEasting = extents.minEasting;
    m_maxEasting = extents.maxEasting;
    m_minNorthing = extents.minNorthing;
    m_maxNorthing = extents.maxNorthing;
    m_topExtent = extents.topDepth;
    m_botExtent = extents.bottomDepth;
    m_spacingX = static_cast<unsigned int>(extents.spacingX);
    m_spacingY = static_cast<unsigned int>(extents.spacingY);
    m_spacingZ = static_cast<unsigned int>(extents.spacingZ);
    m_utmZone = extents.utmZone;
    m_utmNorthSouth = extents.utmNorthSouth;
    m_inUtm = extents.workingFormat;

    updateGeodeticLimits(m_minLatitude,m_maxLatitude,m_minLongitude,m_maxLongitude);
    botExtentChanged(); topExtentChanged();
    spacingXChanged(); spacingYChanged(); spacingZChanged();
    inUtmChanged();
    // Data file all working so save filename
    m_filename = filename;
    filenameChanged();

    // Load structural data
    stModel.loadFromFile(filename);
    eventList.loadFromFile(filename);
    observationList.loadFromFile(filename);

    m_extentsChanged = false;
    m_lockedExtents = true;
    lockedExtentsChanged();
    return 0;
}

int ProjectManagement::reloadProject()
{
    return loadProject(m_filename);
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

void ProjectManagement::loadTextures()
{
    if (getStModel() && getStModel()->loadTextures()) sharedTextureIdChanged();
}

Qt3DRender::QSharedGLTexture *ProjectManagement::getStructuralModelData()
{
    if (getStModel()) return getStModel()->getStructuralData();
    else return nullptr;
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


// Not implemented
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

void ProjectManagement::incrementFlowChoice()
{
    m_loopStructuralFlowOption++;
    if (m_loopStructuralFlowOption > 3) m_loopStructuralFlowOption -= 4;
    loopStructuralFlowOptionChanged();
}

void ProjectManagement::decrementFlowChoice()
{
    m_loopStructuralFlowOption--;
    if (m_loopStructuralFlowOption < 0) m_loopStructuralFlowOption += 4;
    loopStructuralFlowOptionChanged();
}
