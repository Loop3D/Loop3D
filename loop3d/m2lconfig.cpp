#include "m2lconfig.h"
#include "LoopProjectFile.h"
#include "LoopProjectFileUtils.h"

M2lConfig::M2lConfig()
{
    reset();
}

void M2lConfig::reset(void) {
    m_quietMode = 0;
    m_deposits = "Fe,Cu,Au,NONE";
    m_dtb = "";
    m_orientationDecimate = 0;
    m_contactDecimate = 5;
    m_intrusionMode = 0;
    m_interpolationSpacing = 500;
    m_misorientation = 30;
    m_interpolationScheme = "scipy_rbf";
    m_faultDecimate = 5;
    m_minFaultLength = 5000;
    m_faultDip = 90;
    m_plutonDip = 45;
    m_plutonForm = "domes";
    m_distBuffer = 10;
    m_contactDip = -999;
    m_contactOrientationDecimate = 5;
    m_nullScheme = "null";
    m_thicknessBuffer = 5000;
    m_maxThicknessAllowed = 10000;
    m_foldDecimate = 5;
    m_fatStep = 750;
    m_closeDip = -999;
    m_useInterpolations = false;
    m_useFat = true;
}

int M2lConfig::saveToFile(QString filename)
{
    // Find last '/' of the first set of '/'s as in file:/// or url:///
    QStringList list;
    QString name;

#ifdef _WIN32
    list = filename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = filename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    LoopProjectFile::DataCollectionConfiguration config;
    config.quietMode = m_quietMode;
    strncpy_s(config.deposits,m_deposits.toStdString().c_str(),30);
    strncpy_s(config.dtb,m_dtb.toStdString().c_str(),30);
    config.orientationDecimate = m_orientationDecimate;
    config.contactDecimate = m_contactDecimate;
    config.intrusionMode = m_intrusionMode;
    config.interpolationSpacing = m_interpolationSpacing;
    config.misorientation = m_misorientation;
    strncpy_s(config.interpolationScheme,m_interpolationScheme.toStdString().c_str(),30);
    config.faultDecimate = m_faultDecimate;
    config.minFaultLength = m_minFaultLength;
    config.faultDip = m_faultDip;
    config.plutonDip = m_plutonDip;
    strncpy_s(config.plutonForm,m_plutonForm.toStdString().c_str(),30);
    config.distBuffer = m_distBuffer;
    config.contactDip = m_contactDip;
    config.contactOrientationDecimate = m_contactOrientationDecimate;
    strncpy_s(config.nullScheme,m_nullScheme.toStdString().c_str(),30);
    config.thicknessBuffer = m_thicknessBuffer;
    config.maxThicknessAllowed = m_maxThicknessAllowed;
    config.foldDecimate = m_foldDecimate;
    config.fatStep = m_fatStep;
    config.closeDip = m_closeDip;
    config.useInterpolations = m_useInterpolations;
    config.useFat = m_useFat;
    LoopProjectFileResponse resp = LoopProjectFile::SetDataCollectionConfiguration(name.toStdString(),config,false);

    return resp.errorCode;
}

int M2lConfig::loadFromFile(QString filename)
{
    int result = 0;

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
    LoopProjectFile::DataCollectionConfiguration config;
    LoopProjectFileResponse resp = LoopProjectFile::GetDataCollectionConfiguration(name.toStdString(),config,false);

    if (resp.errorCode == 0) {
        m_quietMode = config.quietMode;
        m_orientationDecimate = config.orientationDecimate;
        m_contactDecimate = config.contactDecimate;
        m_intrusionMode = config.intrusionMode;
        m_interpolationSpacing = config.interpolationSpacing;
        m_misorientation = config.misorientation;
        m_interpolationScheme = config.interpolationScheme;
        m_faultDecimate = config.faultDecimate;
        m_minFaultLength = config.minFaultLength;
        m_faultDip = config.faultDip;
        m_plutonDip = config.plutonDip;
        m_plutonForm = config.plutonForm;
        m_distBuffer = config.distBuffer;
        m_contactDip = config.contactDip;
        m_contactOrientationDecimate = config.contactOrientationDecimate;
        m_nullScheme = config.nullScheme;
        m_thicknessBuffer = config.thicknessBuffer;
        m_maxThicknessAllowed = config.maxThicknessAllowed;
        m_foldDecimate = config.foldDecimate;
        m_fatStep = config.fatStep;
        m_closeDip = config.closeDip;
        m_useInterpolations = config.useInterpolations;
        m_useFat = config.useFat;

        quietModeChanged();
        orientationDecimateChanged();
        contactDecimateChanged();
        intrusionModeChanged();
        interpolationSpacingChanged();
        misorientationChanged();
        interpolationSchemeChanged();
        faultDecimateChanged();
        minFaultLengthChanged();
        faultDipChanged();
        plutonDipChanged();
        plutonFormChanged();
        distBufferChanged();
        contactDipChanged();
        contactOrientationDecimateChanged();
        nullSchemeChanged();
        thicknessBufferChanged();
        maxThicknessAllowedChanged();
        foldDecimateChanged();
        fatStepChanged();
        closeDipChanged();
        useInterpolationsChanged();
        useFatChanged();
    } else result = resp.errorCode;

    return result;
}
