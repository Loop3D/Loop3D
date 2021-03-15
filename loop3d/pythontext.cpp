#include "pythontext.h"
#include "projectmanagement.h"
#include <iostream>
#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <QDebug>
#include <QFile>
#include <QTextStream>

namespace py = pybind11;
using namespace py::literals;

PythonText::PythonText(QObject* parent)
    : QObject(parent),
      pythonHighlighter(nullptr)
{
    m_code = "";
    m_threadRunning = false;
    m_threadRunningStage = "";
}

void PythonText::setMainTextEdit(QQuickTextDocument* text)
{
    pythonHighlighter = new PythonHighlighter(text->textDocument());
    pythonHighlighter->rehighlight();
}

void PythonText::setFilename(QString in)
{
    m_filename = in;
    QString content;
    qDebug() << "Loop: Loading python source file " << m_filename;
    if (m_filename != "") {
        try {
            QFile file(m_filename);

            if (file.open(QIODevice::ReadOnly)) {
                QString line;
                QTextStream t(&file);
                do {
                    line = t.readLine();
                    content += line + '\n';
                } while (!line.isNull());
                file.close();
            } else {
                qDebug() << "Failed to load file (" << m_filename << ")";
            }
        } catch (std::exception e) {
            qDebug() << e.what();
            qDebug() << "Failed to load file (" << m_filename << ")";
        }
    }
    m_pythonCode = content;
    Q_EMIT filenameChanged();
    Q_EMIT pythonCodeChanged();
}

void PythonText::run(QString code, QString loopFilename, QString loopStage)
{
    QStringList list;
    QString name;
    // Find last '/' of the first set of '/'s as in file:/// or url:///
#ifdef _WIN32
    list = loopFilename.split(QRegExp("///"));
    name = (list.length() > 1 ? list[1] : list[0]);
#elif __linux__
    list = loopFilename.split(QRegExp("///"));
    name = "/" + (list.length() > 1 ? list[1] : list[0]);
#endif

    int xsteps = 50;
    int ysteps = 50;
    int zsteps = 50;
    std::string structureUrl = "";
    std::string geologyUrl = "";
    std::string mindepUrl = "";
    std::string faultUrl = "";
    std::string foldUrl = "";
    std::string metadataUrl = "";
    qDebug() << "Loop: Loading python script file named: " << name;
    m_locals = py::dict("loopFilename"_a = name.toStdString().c_str());
    if (ProjectManagement::instance()) {
        ProjectManagement* proj = ProjectManagement::instance();
        xsteps = static_cast<int>((proj->m_maxEasting - proj->m_minEasting) / proj->m_spacingX)+1;
        ysteps = static_cast<int>((proj->m_maxNorthing - proj->m_minNorthing) / proj->m_spacingY)+1;
        zsteps = static_cast<int>((proj->m_topExtent - proj->m_botExtent) / proj->m_spacingZ)+1;
        if (loopStage == "DataCollection") {
            structureUrl = replaceKeywords(proj->m_structureUrl);
            geologyUrl = replaceKeywords(proj->m_geologyUrl);
            mindepUrl = replaceKeywords(proj->m_mindepUrl);
            faultUrl = replaceKeywords(proj->m_faultUrl);
            foldUrl = replaceKeywords(proj->m_foldUrl);
            metadataUrl = replaceKeywords(proj->m_metadataUrl);
            qDebug() << "Running Map2Loop Python Script";
            qDebug() << "structure = " << structureUrl.substr(0,structureUrl.find_first_of('?')).c_str();
            qDebug() << "geology   = " << geologyUrl.substr(0,geologyUrl.find_first_of('?')).c_str();
            qDebug() << "mindep    = " << mindepUrl.substr(0,mindepUrl.find_first_of('?')).c_str();
            qDebug() << "fault     = " << faultUrl.substr(0,faultUrl.find_first_of('?')).c_str();
            qDebug() << "fold      = " << foldUrl.substr(0,foldUrl.find_first_of('?')).c_str();
            qDebug() << "meta      = " << metadataUrl.substr(0,metadataUrl.find_first_of('?')).c_str();
            pybind11::dict m2lFiles;
            m2lFiles["structure_file"] = structureUrl;
            m2lFiles["geology_file"] = geologyUrl;
            m2lFiles["mindep_file"] = mindepUrl;
            m2lFiles["fault_file"] = faultUrl;
            m2lFiles["fold_file"] = foldUrl;
            m2lFiles["metadata"] = metadataUrl;
            m_locals["m2lFiles"] = m2lFiles;

            M2lConfig* m2lconf = proj->getM2lConfig();
            pybind11::dict m2lParams;
            m2lParams["orientation_decimate"] = m2lconf->m_orientationDecimate;
            m2lParams["contact_decimate"] = m2lconf->m_contactDecimate;
            m2lParams["intrusion_mode"] = m2lconf->m_intrusionMode;
            m2lParams["interpolation_spacing"] = m2lconf->m_interpolationSpacing;
            m2lParams["misorientation"] = m2lconf->m_misorientation;
            m2lParams["interpolation_scheme"] = m2lconf->m_interpolationScheme.toStdString();
            m2lParams["fault_decimate"] = m2lconf->m_faultDecimate;
            m2lParams["min_fault_length"] = m2lconf->m_minFaultLength;
            m2lParams["fault_dip"] = m2lconf->m_faultDip;
            m2lParams["pluton_dip"] = m2lconf->m_plutonDip;
            m2lParams["pluton_form"] = m2lconf->m_plutonForm.toStdString();
            m2lParams["dist_buffer"] = m2lconf->m_distBuffer;
            m2lParams["contact_dip"] = m2lconf->m_contactDip;
            m2lParams["contact_orientation_decimate"] = m2lconf->m_contactOrientationDecimate;
            m2lParams["null_scheme"] = m2lconf->m_nullScheme.toStdString();
            m2lParams["thickness_buffer"] = m2lconf->m_thicknessBuffer;
            m2lParams["max_thickness_allowed"] = m2lconf->m_maxThicknessAllowed;
            m2lParams["fold_decimate"] = m2lconf->m_foldDecimate;
            m2lParams["fat_step"] = m2lconf->m_fatStep;
            m2lParams["close_dip"] = m2lconf->m_closeDip;
            m2lParams["use_interpolations"] = m2lconf->m_useInterpolations ? "True" : "False";
            m2lParams["use_fat"] = m2lconf->m_useFat ? "True" : "False";
            m_locals["m2lParams"] = m2lParams;
            m_locals["m2lQuietMode"] = m2lconf->m_quietMode == 0 ? "all" : (m2lconf->m_quietMode == 1 ? "no-fugures" : "None");

        } else if (loopStage == "GeologyModel") {
            qDebug() << "Running Loop Structural Python Script";
            m_locals["useLavavu"] = proj->m_useLavavu ? true : false;
        }
        std::string m2lDataDirName = proj->m_filename.toStdString();
        m2lDataDirName = m2lDataDirName.substr(m2lDataDirName.find_last_of('/')+1) + "_m2l_data";
//        qDebug() << "m2lDataDir = " << m2lDataDirName.c_str();
        m_locals["m2lDataDir"] = m2lDataDirName;
    }
    m_locals["xsteps"] = xsteps;
    m_locals["ysteps"] = ysteps;
    m_locals["zsteps"] = zsteps;

    m_code = code;
    if (m_threadRunning == 0) {
        m_threadRunningStage = loopStage;
        if (ProjectManagement::instance()) {
            ProjectManagement* proj = ProjectManagement::instance();
            proj->m_pythonInProgress = 0.1;
            proj->pythonInProgressChanged();
        }
        try {
            py::exec(m_pythonCode.toStdString().c_str(),py::globals(),m_locals);
        } catch (std::exception& e) {
            qDebug() << e.what();
            qDebug() << "Failed to run python code";
        }
        if (!m_threadRunning) {
            m_threadRunning = true;
            PythonTextWorkerThread *workerThread = new PythonTextWorkerThread;
            connect(workerThread, &PythonTextWorkerThread::finished, this, &PythonText::postCode);
            connect(workerThread, &PythonTextWorkerThread::finished, workerThread, &QObject::deleteLater);
            workerThread->start(QThread::NormalPriority);
        }
    }
}

void PythonText::postCode()
{
    m_threadRunning = false;
    if (ProjectManagement::instance()) {
        ProjectManagement* proj = ProjectManagement::instance();
        proj->reloadProject();
        auto globals = py::globals();
        if (globals.contains("errors")) {
            std::cout << globals["errors"].cast<std::string>() << std::endl;
            proj->m_pythonErrors = QString(globals["errors"].cast<std::string>().c_str());
            proj->pythonErrorsChanged();
        }
        if (m_threadRunningStage == "DataCollection")  proj->finishedMap2Loop();
        else if (m_threadRunningStage == "GeologyModel") proj->finishedGeologyModel();

        m_threadRunningStage = "";
    } else {
        qFatal("No global project pointer (NEED TO FIX THIS NOW!)");
    }
}

std::string PythonText::replaceKeywords(std::string incoming)
{
    if (ProjectManagement::instance()) {
        ProjectManagement* proj = ProjectManagement::instance();
        // MGA zone 28348 - 28358 for zones S48 to S58
        // UTM zones 32401 - 32460 for N01 to N60 and 32501 - 32560 for S01 to S60
        std::string epsgMGA = std::to_string(28300 + proj->m_utmZone);
        std::string wpsgUTM = std::to_string(32400 + (proj->m_utmNorthSouth ? 100 : 0) + proj->m_utmZone);
        std::string bboxstr = std::to_string(proj->m_minEasting) + "," + std::to_string(proj->m_minNorthing)
                + "," + std::to_string(proj->m_maxEasting) + "," + std::to_string(proj->m_maxNorthing) + ",EPSG:" + epsgMGA;

        if (incoming.find("$(MINLAT)") != std::string::npos)   incoming.replace(incoming.find("$(MINLAT)"),9,std::to_string(proj->m_minLatitude));
        if (incoming.find("$(MAXLAT)") != std::string::npos)   incoming.replace(incoming.find("$(MAXLAT)"),9,std::to_string(proj->m_maxLatitude));
        if (incoming.find("$(MINLONG)") != std::string::npos)  incoming.replace(incoming.find("$(MINLONG)"),10,std::to_string(proj->m_minLongitude));
        if (incoming.find("$(MAXLONG)") != std::string::npos)  incoming.replace(incoming.find("$(MAXLONG)"),10,std::to_string(proj->m_maxLongitude));
        if (incoming.find("$(MAXNORTH)") != std::string::npos) incoming.replace(incoming.find("$(MAXNORTH)"),11,std::to_string(proj->m_maxNorthing));
        if (incoming.find("$(MINNORTH)") != std::string::npos) incoming.replace(incoming.find("$(MINNORTH)"),11,std::to_string(proj->m_minNorthing));
        if (incoming.find("$(MINEAST)") != std::string::npos)  incoming.replace(incoming.find("$(MINEAST)"),10,std::to_string(proj->m_minEasting));
        if (incoming.find("$(MAXEAST)") != std::string::npos)  incoming.replace(incoming.find("$(MAXEAST)"),10,std::to_string(proj->m_maxEasting));
        if (incoming.find("$(BBOXSTR)") != std::string::npos)  incoming.replace(incoming.find("$(BBOXSTR)"),10,bboxstr);
        if (incoming.find("$(EPSG)") != std::string::npos)     incoming.replace(incoming.find("$(EPSG)"),7,epsgMGA);
    }
    return incoming;
}

void PythonTextWorkerThread::run()
{
    bool running = true;
    while (running) {
        py::exec("import time\ntime.sleep(0.02)\n");
        if (ProjectManagement::instance()) {
            ProjectManagement* proj = ProjectManagement::instance();
            auto globals = py::globals();
            if (proj->m_pythonInProgress <= 0) {
                running = false;
            }
            if (proj->m_pythonInProgress >= 666) {
                if (globals.contains("errors")) {
                    std::cout << globals["errors"].cast<std::string>() << std::endl;
                    proj->m_pythonErrors = QString(globals["errors"].cast<std::string>().c_str());
                    proj->pythonErrorsChanged();
                }
                running = false;
            }
            if (globals.contains("currentProgress") && running) {
                double progress = globals["currentProgress"].cast<double>() / 100.0;
                proj->m_pythonInProgress = progress;
                proj->pythonInProgressChanged();
            }
            if (globals.contains("currentProgress") && running) {
                std::string progressText = globals["currentProgressText"].cast<std::string>();
                proj->m_pythonProgressText = progressText.c_str();
                proj->pythonProgressTextChanged();
                proj->m_pythonProgressTextLineCount = std::count(progressText.begin(),progressText.end(),'\n') + 1;
                proj->pythonProgressTextLineCountChanged();
            }
        }
        msleep(2);
    }
}
