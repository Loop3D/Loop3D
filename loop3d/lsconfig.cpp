#include "lsconfig.h"
#include "LoopProjectFile.h"

LSConfig::LSConfig()
{
    reset();
}

void LSConfig::reset(void)
{
    m_foliationInterpolator = 0;
    m_foliationNumElements = 100000;
    m_foliationBuffer = 0.8;
    m_foliationSolver = 0;
    m_foliationDamp = 1;

    m_faultInterpolator = 0;
    m_faultNumElements = 10000;
    m_faultDataRegion = 0.3;
    m_faultSolver = 0;
    m_faultCpw = 10;
    m_faultNpw = 10;
}

int LSConfig::saveToFile(QString filename)
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

    LoopProjectFile::StructuralModelsConfiguration config;
    std::string foliationInterpolator = m_foliationInterpolator == 0 ? "PLI" : "something else";
    strncpy(config.foliationInterpolator,foliationInterpolator.c_str(),30);
    config.foliationNumElements = m_foliationNumElements;
    config.foliationBuffer = m_foliationBuffer;
    std::string foliationSolver = m_foliationSolver == 0 ? "pyamg" : (m_foliationSolver == 1 ? "cg" : "lu");
    strncpy(config.foliationSolver,foliationSolver.c_str(),30);
    config.foliationDamp = m_foliationDamp;

    std::string faultInterpolator = m_faultInterpolator == 0 ? "FDI" : "something else";
    strncpy(config.faultInterpolator,faultInterpolator.c_str(),30);
    config.faultNumElements = m_faultNumElements;
    config.faultDataRegion = m_faultDataRegion;
    std::string faultSolver = m_faultSolver == 0 ? "pyamg" : (m_faultSolver == 1 ? "cg" : "lu");
    strncpy(config.faultSolver,faultSolver.c_str(),30);
    config.faultCpw = m_faultCpw;
    config.faultNpw = m_faultNpw;

    LoopProjectFileResponse resp = LoopProjectFile::SetStructuralModelsConfiguration(name.toStdString(),config,false);
    return resp.errorCode;
}

int LSConfig::loadFromFile(QString filename)
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
    int result = 0;
    LoopProjectFile::StructuralModelsConfiguration config;
    LoopProjectFileResponse resp = LoopProjectFile::GetStructuralModelsConfiguration(name.toStdString(),config,false);

    if (resp.errorCode == 0) {
        m_foliationInterpolator = strncmp(config.foliationInterpolator,"PLI",3) == 0 ? 0 : 1;
        m_foliationNumElements = config.foliationNumElements;
        m_foliationBuffer = config.foliationBuffer;
        m_foliationSolver = strncmp(config.foliationSolver,"pyamg",5) == 0 ? 0
                          : strncmp(config.foliationSolver,"cg",2) == 0 ? 1
                          : 2;
        m_foliationDamp = config.foliationDamp;

        m_faultInterpolator = strncmp(config.faultInterpolator,"FDI",3) == 0 ? 0 : 1;
        m_faultNumElements = config.faultNumElements;
        m_faultDataRegion = config.faultDataRegion;
        m_faultSolver = strncmp(config.faultSolver,"pyamg",5) == 0 ? 0
                          : strncmp(config.faultSolver,"cg",2) == 0 ? 1
                          : 2;

        m_faultCpw = config.faultCpw;
        m_faultNpw = config.faultNpw;

        foliationInterpolatorChanged();
        foliationNumElementsChanged();
        foliationBufferChanged();
        foliationSolverChanged();
        foliationDampChanged();

        faultInterpolatorChanged();
        faultNumElementsChanged();
        faultDataRegionChanged();
        faultSolverChanged();
        faultCpwChanged();
        faultNpwChanged();
    } else result = resp.errorCode;
    return result;
}
