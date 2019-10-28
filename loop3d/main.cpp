#include <QGuiApplication>
#include <QQmlContext>
#include <QtQuick/QQuickView>

#include <pybind11/embed.h>

#include "isosurface.h"

#include "pythontext.h"
#include "utmconverter.h"
#include "projectmanagement.h"
#include "datasourcemodel.h"
#include "datasourcelist.h"

namespace py = pybind11;

void setupOpenGLVersion(void)
{
    QSurfaceFormat fmt;
    if (fmt.renderableType() == QSurfaceFormat::OpenGLES) {
        fmt.setVersion(3,0);
        fmt.setDepthBufferSize(24);
    } else {
        fmt.setMajorVersion(4);
        fmt.setMinorVersion(3);
        fmt.setDepthBufferSize(24);
        fmt.setProfile(QSurfaceFormat::CoreProfile);
        fmt.setDepthBufferSize(24);
        fmt.setStencilBufferSize(8);
        fmt.setSamples(4);
    }
    QSurfaceFormat::setDefaultFormat(fmt);
}

int main(int argc, char *argv[])
{
    qDebug() << "Starting pybind11 interpreter";
    try {
        py::initialize_interpreter();
        qDebug() << "Interpreter loaded, trying a python file";

        py::eval_file("..\\setup.py");
        qDebug() << "Python interpreter and setup environment file successfully ran";
    } catch (std::exception& e) {
        qDebug() << e.what();
        qDebug() << "Python file DID NOT successfully ran\n";
    }

    setupOpenGLVersion();
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    app.setOrganizationName("Loop3d");
    app.setApplicationName("Loop3d");
    app.setOrganizationDomain("loop3d.org");

    DataSourceList dataSourceList;
    qmlRegisterType<Isosurface>("OpenGLUnderQML", 1, 0, "Isosurface");

    ProjectManagement* project = ProjectManagement::instance();
    project->m_minLatitude = -32.5;
    project->m_maxLatitude = -31.5;
    project->m_minLongitude = 115.5;
    project->m_maxLongitude = 116.5;
    qmlRegisterUncreatableType<ProjectManagement>("loop3d.projectmanagement",1,0,"Project",
                                               QStringLiteral("ProjectManagement shoudl not be created in QML"));

    qmlRegisterType<PythonText>("loop3d.pythontext",1,0,"PythonText");

    qmlRegisterType<LL>("loop3d.utmconverter",1,0,"LL");
    qmlRegisterType<UTM>("loop3d.utmconverter",1,0,"UTM");
    qmlRegisterType<LLExtents>("loop3d.utmconverter",1,0,"LLExtents");
    qmlRegisterType<UTMExtents>("loop3d.utmconverter",1,0,"UTMExtents");

    qmlRegisterType<DataSourceModel>("loop3d.datasourcemodel",1,0,"DataSourceModel");
    qmlRegisterUncreatableType<DataSourceList>("loop3d.datasourcemodel",1,0,"DataSourceList",
                                               QStringLiteral("DataSourceList should not be created in QML"));

    QQuickView view;
    view.rootContext()->setContextProperty(QStringLiteral("dataSourceList"), &dataSourceList);
    view.rootContext()->setContextProperty(QStringLiteral("project"), project);
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    int res = app.exec();
    py::finalize_interpreter();
    return res;
}
