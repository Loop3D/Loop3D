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
    qDebug() << "Trying to load python source file " << m_filename;
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
        qDebug() << "Trying to load file " << name;
        auto locals = py::dict("loopFilename"_a = name.toStdString().c_str());
        locals["xsteps"] = xsteps;
        locals["ysteps"] = ysteps;
        locals["zsteps"] = zsteps;
        py::exec(code.toStdString().c_str(),py::globals(),locals);

        if (useResult) {
            // Get results back from algorithm (numpy arrays work but don't go past boundaries)
            py::array_t<float> result = locals["result"].cast<py::array_t<float> >();
            if (ProjectManagement::instance()) {
                ProjectManagement* proj = ProjectManagement::instance();
                ProjectManagement::instance()->getStModel()->loadData(result,
                            static_cast<float>(proj->m_minEasting), static_cast<float>(proj->m_maxEasting), xsteps,
                            static_cast<float>(proj->m_minNorthing), static_cast<float>(proj->m_maxNorthing), ysteps,
                            static_cast<float>(proj->m_minDepth),static_cast<float>(proj->m_maxDepth),zsteps);
            } else {
                qFatal("No global project pointer (NEED TO FIX THIS NOW!)");
            }
        }
    } catch (std::exception& e) {
        qDebug() << e.what();
        qDebug() << "Failed to run python code";
    }
}
