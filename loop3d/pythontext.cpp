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

void PythonText::run(QString code, QString loopFilename, bool useResult)
{
    QStringList list = loopFilename.split(QRegExp("///"));
    QString name = (list.length() > 1 ? list[1] : list[0]);
    try {
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
        auto locals = py::dict("loopFilename"_a = name.toStdString().c_str());
        if (ProjectManagement::instance()) {
            ProjectManagement* proj = ProjectManagement::instance();
            xsteps = static_cast<int>((proj->m_maxEasting - proj->m_minEasting) / proj->m_spacingX)+1;
            ysteps = static_cast<int>((proj->m_maxNorthing - proj->m_minNorthing) / proj->m_spacingY)+1;
            zsteps = static_cast<int>((proj->m_topExtent - proj->m_botExtent) / proj->m_spacingZ)+1;
            structureUrl = replaceKeywords(proj->m_structureUrl);
            geologyUrl = replaceKeywords(proj->m_geologyUrl);
            mindepUrl = replaceKeywords(proj->m_mindepUrl);
            faultUrl = replaceKeywords(proj->m_faultUrl);
            foldUrl = replaceKeywords(proj->m_foldUrl);
            metadataUrl = replaceKeywords(proj->m_metadataUrl);
            std::string m2lDataDirName = proj->m_filename.toStdString();
            m2lDataDirName = m2lDataDirName.substr(m2lDataDirName.find_last_of('/')+1) + "_m2l_data";
            qDebug() << "structure = " << structureUrl.substr(0,structureUrl.find_first_of('?')).c_str();
            qDebug() << "geology   = " << geologyUrl.substr(0,geologyUrl.find_first_of('?')).c_str();
            qDebug() << "mindep    = " << mindepUrl.substr(0,mindepUrl.find_first_of('?')).c_str();
            qDebug() << "fault     = " << faultUrl.substr(0,faultUrl.find_first_of('?')).c_str();
            qDebug() << "fold      = " << foldUrl.substr(0,foldUrl.find_first_of('?')).c_str();
            qDebug() << "meta      = " << metadataUrl.substr(0,metadataUrl.find_first_of('?')).c_str();
            locals["use_lavavu"] = proj->m_useLavavu ? true : false;
            qDebug() << "m2l_data_dir = " << m2lDataDirName.c_str();
            locals["m2l_data_dir"] = m2lDataDirName;
        }
        locals["xsteps"] = xsteps;
        locals["ysteps"] = ysteps;
        locals["zsteps"] = zsteps;
        locals["structure_url"] = structureUrl;
        locals["geology_url"] = geologyUrl;
        locals["mindep_url"] = mindepUrl;
        locals["fault_url"] = faultUrl;
        locals["fold_url"] = foldUrl;
        locals["metadata"] = metadataUrl;
        py::exec(code.toStdString().c_str(),py::globals(),locals);

        if (useResult) {
            // Get results back from algorithm (numpy arrays work but don't go past boundaries)
            py::array_t<float> result = locals["result"].cast<py::array_t<float> >();
            py::array_t<float> stepsizes = locals["stepsizes"].cast<py::array_t<float> >();
            xsteps = locals["xsteps"].cast<int>();
            ysteps = locals["ysteps"].cast<int>();
            zsteps = locals["zsteps"].cast<int>();
            qDebug() << stepsizes.at(0) << " " << stepsizes.at(1) << " " << stepsizes.at(2);
            qDebug() << xsteps << " " << ysteps << " " << zsteps;
            if (ProjectManagement::instance()) {
                ProjectManagement* proj = ProjectManagement::instance();
                if (proj->getStModel()) proj->getStModel()->loadData(result,
                            static_cast<float>(proj->m_minEasting), static_cast<float>(proj->m_maxEasting), xsteps,
                            static_cast<float>(proj->m_minNorthing), static_cast<float>(proj->m_maxNorthing), ysteps,
                            static_cast<float>(proj->m_topExtent),static_cast<float>(proj->m_botExtent),zsteps);
            } else {
                qFatal("No global project pointer (NEED TO FIX THIS NOW!)");
            }
        }
        if (locals.contains("errors")) {
            std::cout << locals["errors"].cast<std::string>() << std::endl;
            if (ProjectManagement::instance()) {
                ProjectManagement* proj = ProjectManagement::instance();
                proj->m_pythonErrors = QString(locals["errors"].cast<std::string>().c_str());
                proj->pythonErrorsChanged();
            }
        }
        qDebug() << "Loop: Finished python code exec in file " << name;
    } catch (std::exception& e) {
        qDebug() << e.what();
        qDebug() << "Failed to run python code";
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
