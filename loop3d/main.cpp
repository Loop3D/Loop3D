#include <QGuiApplication>
#include <QQmlContext>
#include <QtQuick/QQuickView>
#include <QDir>

#include <pybind11/embed.h>

#include "3dviewer.h"
#include "pythontext.h"
#include "utmconverter.h"
#include "projectmanagement.h"
#include "datasourcemodel.h"
#include "datasourcelist.h"
#include "eventmodel.h"
#include "eventlist.h"
#include "observationmodel.h"
#include "observationlist.h"
#include "m2lconfig.h"
#include "lsconfig.h"

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

// Default message handler to be called to bypass all other warnings.
static const QtMessageHandler QT_DEFAULT_MESSAGE_HANDLER = qInstallMessageHandler(nullptr);
// custom Message Handler to remove spam messages
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString & msg);

int main(int argc, char *argv[])
{
    try {
        pybind11::initialize_interpreter();
        pybind11::eval_file(".\\setupPython.py");
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        std::cout << "Python file setupPython.py DID NOT run successfully" << std::endl;
    }

    setupOpenGLVersion();
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon("loop.ico"));
    qInstallMessageHandler(customMessageHandler);

    app.setOrganizationName("Loop");
    app.setApplicationName("Loop");
    app.setOrganizationDomain("loop3d.org");

    L3DViewer* viewer = L3DViewer::instance();

    ProjectManagement* project = ProjectManagement::instance();
    project->m_minLatitude = -23.009;
    project->m_maxLatitude = -21.992;
    project->m_minLongitude = 117.001;
    project->m_maxLongitude = 118.002;

    qmlRegisterUncreatableType<ProjectManagement>("loop3d.projectmanagement",1,0,"Project",
                                               QStringLiteral("ProjectManagement should not be created in QML"));
    qmlRegisterUncreatableType<L3DViewer>("loop3d.projectmanagement",1,0,"L3DViewer",
                                               QStringLiteral("3DViewer should not be created in QML"));
    qmlRegisterUncreatableType<DataSourceList>("loop3d.datasourcemodel",1,0,"DataSourceList",
                                               QStringLiteral("DataSourceList should not be created in QML"));
    qmlRegisterUncreatableType<EventList>("loop3d.eventmodel",1,0,"EventList",
                                               QStringLiteral("EventList should not be created in QML"));
    qmlRegisterUncreatableType<EventRelationshipList>("loop3d.eventmodel",1,0,"EventRelationshipList",
                                               QStringLiteral("EventRelationshipList should not be created in QML"));
    qmlRegisterUncreatableType<ObservationList>("loop3d.observationmodel",1,0,"ObservationList",
                                               QStringLiteral("ObservationList should not be created in QML"));
    qmlRegisterUncreatableType<M2lConfig>("loop3d.m2lconfig",1,0,"M2lConfig",
                                               QStringLiteral("M2lConfig should not be created in QML"));
    qmlRegisterUncreatableType<LSConfig>("loop3d.lsconfig",1,0,"LSConfig",
                                               QStringLiteral("LSConfig should not be created in QML"));

    qmlRegisterType<PythonText>("loop3d.pythontext",1,0,"PythonText");
    qmlRegisterType<LL>("loop3d.utmconverter",1,0,"LL");
    qmlRegisterType<UTM>("loop3d.utmconverter",1,0,"UTM");
    qmlRegisterType<LLExtents>("loop3d.utmconverter",1,0,"LLExtents");
    qmlRegisterType<UTMExtents>("loop3d.utmconverter",1,0,"UTMExtents");
    qmlRegisterType<DataSourceModel>("loop3d.datasourcemodel",1,0,"DataSourceModel");
    qmlRegisterType<EventModel>("loop3d.eventmodel",1,0,"EventModel");
    qmlRegisterType<EventRelationshipModel>("loop3d.eventmodel",1,0,"EventRelationshipModel");
    qmlRegisterType<ObservationModel>("loop3d.observationmodel",1,0,"ObservationModel");
    qmlRegisterType<M2lConfig>("loop3d.m2lconfig",1,0,"M2lConfig");
    qmlRegisterType<LSConfig>("loop3d.lsconfig",1,0,"LSConfig");

    QQuickView view;
    project->setQmlQuickView(&view);
    view.rootContext()->setContextProperty(QStringLiteral("dataSourceList"), project->getDataSourceList());
    view.rootContext()->setContextProperty(QStringLiteral("eventList"), project->getEventList());
    view.rootContext()->setContextProperty(QStringLiteral("eventRelationshipList"), project->getEventRelationshipList());
    view.rootContext()->setContextProperty(QStringLiteral("observationList"), project->getObservationList());
    view.rootContext()->setContextProperty(QStringLiteral("m2lConfig"), project->getM2lConfig());
    view.rootContext()->setContextProperty(QStringLiteral("lsConfig"), project->getLSConfig());
    view.rootContext()->setContextProperty(QStringLiteral("project"), project);
    view.rootContext()->setContextProperty(QStringLiteral("viewer"), viewer);
    view.rootContext()->setContextProperty(QStringLiteral("workingDir"), QString(QDir::currentPath()));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();
    QObject::connect(view.rootContext()->engine(),SIGNAL(quit()),qApp,SLOT(quit()));
    QObject::connect(project,SIGNAL(finishedMap2Loop()),view.rootObject(),SLOT(extractDataCompleted()));
    QObject::connect(project,SIGNAL(finishedGeologyModel()),view.rootObject(),SLOT(geologyModelCompleted()));

    std::cout << "Starting Loop GUI ..." << std::endl;
    int res = app.exec();
    pybind11::finalize_interpreter();
    return res;
}

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString & msg)
{
    switch (type) {
        case QtWarningMsg:
            if (msg.contains("QML Image: Cannot open:")); // GUI will often try to open invalid images
            else if (msg.contains("QGeoTileProviderOsm: Tileserver disabled at ")); // Spam from Qml Map
            else if (msg.contains("QGeoTileFetcherOsm: all providers resolved"));   // Spam from Qml Map
            else if (msg.contains("QWindowsWindow::setGeometry: Unable to set geometry"));   // Resize issue on map resize
            else {
                (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg); // bypass and display all other warnings
            }
            break;
        default:    // Call the default handler.
            (*QT_DEFAULT_MESSAGE_HANDLER)(type, context, msg);
            break;
    }
}

