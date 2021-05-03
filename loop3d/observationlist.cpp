#include "observationlist.h"
#include "projectmanagement.h"
#include "3dviewer.h"
#include <QtAlgorithms>
#include <QDebug>
#include <QtMath>

ObservationList::ObservationList(QObject *parent) : QObject(parent)
{
    m_observationsData = nullptr;
    m_dataChanged = true;
    observationsTexture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    m_sharedTexture = new Qt3DRender::QSharedGLTexture();
}

ObservationList::~ObservationList()
{
    observations.clear();
    if (m_observationsData) free(m_observationsData);
    if (observationsTexture) delete observationsTexture;
    if (m_sharedTexture) delete m_sharedTexture;

}

void ObservationList::clearList()
{
//    qDebug() << "Loop: Clearing observation list";
    preItemReset();
    observations.clear();
    postItemReset();
//    qDebug() << "Loop: Cleared observation list";
}

int ObservationList::loadFromFile(QString filename)
{
    int result = 0;
    while (observations.size()) removeItem(0);

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

    preItemReset();
    std::vector<LoopProjectFile::FaultObservation> faultObservations;
    LoopProjectFile::GetFaultObservations(name.toStdString(),faultObservations,false);
    if (faultObservations.size()) {
        for (auto it=faultObservations.begin();it!=faultObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::FaultObservation> observation = std::make_shared<LoopProjectFile::FaultObservation>(std::move(*it));
            observation->type = LoopProjectFile::FAULTOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::FoldObservation> foldObservations;
    LoopProjectFile::GetFoldObservations(name.toStdString(),foldObservations,false);
    if (foldObservations.size()) {
        for (auto it=foldObservations.begin();it!=foldObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::FoldObservation> observation = std::make_shared<LoopProjectFile::FoldObservation>(std::move(*it));
            observation->type = LoopProjectFile::FOLDOBSERVATION;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::FoliationObservation> foliationObservations;
    LoopProjectFile::GetFoliationObservations(name.toStdString(),foliationObservations,false);
    if (foliationObservations.size()) {
        for (auto it=foliationObservations.begin();it!=foliationObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::FoliationObservation> observation = std::make_shared<LoopProjectFile::FoliationObservation>(std::move(*it));
            observation->type = LoopProjectFile::FOLIATIONOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::DiscontinuityObservation> discontinuityObservations;
    LoopProjectFile::GetDiscontinuityObservations(name.toStdString(),discontinuityObservations,false);
    if (discontinuityObservations.size()) {
        for (auto it=discontinuityObservations.begin();it!=discontinuityObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::DiscontinuityObservation> observation = std::make_shared<LoopProjectFile::DiscontinuityObservation>(std::move(*it));
            observation->type = LoopProjectFile::DISCONTINUITYOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    std::vector<LoopProjectFile::StratigraphicObservation> stratigraphicObservations;
    LoopProjectFile::GetStratigraphicObservations(name.toStdString(),stratigraphicObservations,false);
    if (stratigraphicObservations.size()) {
        for (auto it=stratigraphicObservations.begin();it!=stratigraphicObservations.end();it++) {
            std::shared_ptr<LoopProjectFile::StratigraphicObservation> observation = std::make_shared<LoopProjectFile::StratigraphicObservation>(std::move(*it));
            observation->type = LoopProjectFile::STRATIGRAPHICOBSERVATION;
            while (observation->dipdir > 360.0) observation->dipdir -=  360.0;
            observations.append(observation);
        }
    }
    postItemReset();
    loadData();
    return result;
}

int ObservationList::saveToFile(QString filename)
{
    int result = 0;
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

    std::vector<LoopProjectFile::FaultObservation> faultObservations;
    std::vector<LoopProjectFile::FoldObservation> foldObservations;
    std::vector<LoopProjectFile::FoliationObservation> foliationObservations;
    std::vector<LoopProjectFile::DiscontinuityObservation> discontinuityObservations;
    std::vector<LoopProjectFile::StratigraphicObservation> stratigraphicObservations;
    auto observationList = getObservations();
    for (auto it=observationList.begin();it!=observationList.end();it++) {
        // Load data into appropriate structure
        if ((*it)->type == LoopProjectFile::FAULTOBSERVATION) {
            faultObservations.push_back(*(LoopProjectFile::FaultObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::FOLDOBSERVATION) {
            foldObservations.push_back(*(LoopProjectFile::FoldObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::FOLIATIONOBSERVATION) {
            foliationObservations.push_back(*(LoopProjectFile::FoliationObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::DISCONTINUITYOBSERVATION) {
            discontinuityObservations.push_back(*(LoopProjectFile::DiscontinuityObservation*)(it->get()));
        } else if ((*it)->type == LoopProjectFile::STRATIGRAPHICOBSERVATION) {
            stratigraphicObservations.push_back(*(LoopProjectFile::StratigraphicObservation*)(it->get()));
        } else {
            std::cout << "UNIDENTIFIED type for observation of event " << (*it)->eventId << std::endl;
        }
    }
    if (faultObservations.size()) LoopProjectFile::SetFaultObservations(name.toStdString(),faultObservations,false);
    if (foldObservations.size()) LoopProjectFile::SetFoldObservations(name.toStdString(),foldObservations,false);
    if (foliationObservations.size()) LoopProjectFile::SetFoliationObservations(name.toStdString(),foliationObservations,false);
    if (discontinuityObservations.size()) LoopProjectFile::SetDiscontinuityObservations(name.toStdString(),discontinuityObservations,false);
    if (stratigraphicObservations.size()) LoopProjectFile::SetStratigraphicObservations(name.toStdString(),stratigraphicObservations,false);

    return result;
}

bool ObservationList::setObservationAt(int index, const LoopProjectFile::Observation& observation)
{
    if (index < 0 || index >= observations.size())
        return false;

    (*observations[index]) = observation;
    loadData();
    return true;
}

bool ObservationList::appendItem(int eventId, double easting, double northing, double altitude, LoopProjectFile::ObservationType type)
{
    std::shared_ptr<LoopProjectFile::Observation> observation = std::make_shared<LoopProjectFile::Observation>();
    observation->eventId = eventId;
    observation->easting = easting;
    observation->northing = northing;
    observation->altitude = altitude;
    observation->type = type;

    preItemAppended(observations.size(),1);
    observations.append(observation);
    postItemAppended();
    loadData();
    return 1;
}

bool ObservationList::removeItem(int index)
{
    if (index < 0 || index >= observations.size()) return false;

    preItemRemoved(index);
    observations.removeAt(index);
    postItemRemoved();
    return true;
}

void ObservationList::loadData()
{
    dataMutex.lock();
    {
        // m_observationsData is effectively a two dimensional array of floats
        // Each row contains easting,northing,altitude for the first RGB components
        // and then type,dipDir,dip for the second RGB
        // ie. [[obs[0].easting, obs[0].northing, obs[0].altitude] [obs[1].easting, obs[1].northing, obs[1].altitude] ...
        //      [obs[0].type,    obs[0].dipDir,   obs[0].dip     ] [obs[1].type,    obs[1].dipDir,   obs[1].dip     ] ... ]
        //
        unsigned long long textureWidth = 2048;
        if (m_observationsData) free(m_observationsData);
        double xmin = 0;
        double ymin = 0;
        double zmin = 0;
        L3DViewer *viewer = L3DViewer::instance();
        if (viewer != nullptr) {
            xmin = viewer->m_structureXMin;
            ymin = viewer->m_structureYMin;
            zmin = viewer->m_structureZMin;
        }
        m_observationsData = static_cast<float*>(malloc(sizeof(float)*textureWidth*2*3));
        if (!m_observationsData) {
            qDebug() << "Failed to allocate memory for value texture";
        } else {
            unsigned long long count = 0;
            for (auto it=observations.begin();it !=observations.end();it++) {
                unsigned long long posLoc = count*3;
                unsigned long long dataLoc = (textureWidth+count)*3;
                m_observationsData[posLoc+0] = static_cast<float>(it->get()->easting - xmin);
                m_observationsData[posLoc+1] = static_cast<float>(it->get()->northing - ymin);
                m_observationsData[posLoc+2] = static_cast<float>(it->get()->altitude - zmin);
                m_observationsData[dataLoc+0] = static_cast<float>(it->get()->type);
                if (it->get()->type == LoopProjectFile::FAULTOBSERVATION) {
                    LoopProjectFile::FaultObservation* obs = (LoopProjectFile::FaultObservation*)(it->get());
                    m_observationsData[dataLoc+1] = static_cast<float>(obs->dipdir*M_PI/180.0);
                    m_observationsData[dataLoc+2] = static_cast<float>(obs->dip*M_PI/180.0);
                } else if (it->get()->type == LoopProjectFile::FOLIATIONOBSERVATION) {
                    LoopProjectFile::FoliationObservation* obs = (LoopProjectFile::FoliationObservation*)(it->get());
                    m_observationsData[dataLoc+1] = static_cast<float>(obs->dipdir*M_PI/180.0);
                    m_observationsData[dataLoc+2] = static_cast<float>(obs->dip*M_PI/180.0);
                } else if (it->get()->type == LoopProjectFile::DISCONTINUITYOBSERVATION) {
                    LoopProjectFile::DiscontinuityObservation* obs = (LoopProjectFile::DiscontinuityObservation*)(it->get());
                    m_observationsData[dataLoc+1] = static_cast<float>(obs->dipdir*M_PI/180.0);
                    m_observationsData[dataLoc+2] = static_cast<float>(obs->dip*M_PI/180.0);
                } else if (it->get()->type == LoopProjectFile::STRATIGRAPHICOBSERVATION) {
                    LoopProjectFile::StratigraphicObservation* obs = (LoopProjectFile::StratigraphicObservation*)(it->get());
                    m_observationsData[dataLoc+1] = static_cast<float>(obs->dipdir*M_PI/180.0);
                    m_observationsData[dataLoc+2] = static_cast<float>(obs->dip*M_PI/180.0);
                }
                ++count;
                if (count >= textureWidth) break;
            }
        }
        m_dataChanged = true;
    }
    dataMutex.unlock();
}

int ObservationList::loadTextures()
{
    if (m_dataChanged) {
//        qDebug() << "Loop: Setting up observation texture for display";
        ProjectManagement* project =  ProjectManagement::instance();
        if (project->getQmlQuickView()->openglContext() == nullptr) return 0;
        dataMutex.lock();
        {
            // QOpenGLTexture style
            if (observationsTexture->isCreated()) {
                observationsTexture->destroy();
                observationsTexture->create();
            }
            observationsTexture->setAutoMipMapGenerationEnabled(false);
            observationsTexture->setFormat(QOpenGLTexture::RGB32F);
            observationsTexture->setSize(2048,2);
            observationsTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
            observationsTexture->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
            observationsTexture->allocateStorage(QOpenGLTexture::RGB,QOpenGLTexture::Float32);
            observationsTexture->setData(QOpenGLTexture::RGB,QOpenGLTexture::Float32,static_cast<const void*>(m_observationsData));
            m_sharedTexture->setTextureId(static_cast<int>(observationsTexture->textureId()));

            project->m_numObservations = observations.size();
            project->numObservationsChanged();
            m_dataChanged = false;
        }
        dataMutex.unlock();
        return 1;
    }
    return 0;
}
