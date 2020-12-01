#include "structuralmodel.h"
#include "projectmanagement.h"
#include "3dviewer.h"
#include <limits>
#include <Qt3DRender/QTexture>
#include <QOffscreenSurface>
#include <QDebug>

#define TEST_GRID_SIZE 50

StructuralModel::StructuralModel()
{
    m_width  = 0;
    m_height = 0;
    m_depth  = 0;
    m_totalPoints = 0;
    m_totalTetra = 0;

    m_xmin = -1.0;
    m_xmax =  1.0;
    m_ymin = -1.0;
    m_ymax =  1.0;
    m_zmin = -1.0;
    m_zmax =  1.0;

    m_valmin = 0.0;
    m_valmax = 1.0;

    m_values.clear();
    m_valueData = nullptr;
    m_dataChanged = false;
    valueTexture = new QOpenGLTexture(QOpenGLTexture::Target3D);
    m_sharedTexture = new Qt3DRender::QSharedGLTexture();
    m_modelCreated = false;
    createBasicTestStructure(TEST_GRID_SIZE);
}

StructuralModel::~StructuralModel()
{
    if (m_valueData) free(m_valueData);
    if (valueTexture) delete valueTexture;
    if (m_sharedTexture) delete m_sharedTexture;

}

void StructuralModel::clearData()
{
    m_modelCreated = false;
    createBasicTestStructure(TEST_GRID_SIZE);
}

void StructuralModel::loadData(pybind11::array_t<float> values_in, float xmin, float xmax,  int xsteps, float ymin, float ymax, int ysteps, float zmin, float zmax, int zsteps)
{
    qDebug() << "Note: Max texture size is" << GL_MAX_TEXTURE_SIZE;
    qDebug() << "Note: Max 3D texture size is" << GL_MAX_3D_TEXTURE_SIZE;
    qDebug() << "Loading data from structual model," << values_in.size() << "values";

    dataMutex.lock();
    {
        m_xmin = xmin;
        m_xmax = xmax;
        m_ymin = ymin;
        m_ymax = ymax;
        m_zmin = zmin;
        m_zmax = zmax;
        m_valmin = std::numeric_limits<float>::max();
        m_valmax = -std::numeric_limits<float>::max();
        // Texture sizes
        m_width = static_cast<unsigned int>(xsteps);
        m_height = static_cast<unsigned int>(ysteps);
        m_depth = static_cast<unsigned int>(zsteps);
        m_totalPoints = m_width * m_height * m_depth;
        m_totalTetra = (m_width-1) * (m_height-1) * (m_depth-1) * 5;

        qDebug() << "Clearing data strutures";
        // Clear data structures
        m_values.clear();

        qDebug() << "Allocating memory for textures (" << m_width << "x" << m_height << "x" << m_depth << ")";
        // Allocate memory for textures
        if (m_valueData) free(m_valueData);
        m_valueData = static_cast<float*>(malloc(sizeof(float)*m_totalPoints));
        if (!m_valueData) {
            qDebug() << "Failed to allocate memory for value texture";
        } else {
            qDebug() << "Loading structures";
            // Load structures and textures
            for (unsigned int i=0;i<values_in.size();i++) {
                // Value Structure
                float val = values_in.at(i);
                m_values.push_back(val);
                m_valueData[i] = static_cast<float>(val);
                if (val < m_valmin) m_valmin = val;
                if (val > m_valmax) m_valmax = val;
            }
            qDebug() << "Loaded structures";
            m_dataChanged = true;
            m_modelCreated = true;
        }
    }
    dataMutex.unlock();
}

int StructuralModel::saveToFile(QString filename)
{
    int result = 0;
    if (!m_modelCreated) return result;

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

    dataMutex.lock();

    std::vector<float> data(m_valueData,m_valueData + m_width*m_height*m_depth);
    std::vector<int> dataShape;
    dataShape.push_back(m_width);
    dataShape.push_back(m_height);
    dataShape.push_back(m_depth);
    LoopProjectFileResponse resp = {0,""};
    resp = LoopProjectFile::SetStructuralModel(name.toStdString(),data,dataShape,0,true);
    if (resp.errorCode) {
        qDebug() << resp.errorMessage.c_str();
        return 1;
    }

    dataMutex.unlock();
    return result;
}

int StructuralModel::loadFromFile(QString filename)
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

    dataMutex.lock();
    {
        std::vector<float> data;
        std::vector<int> dataShape;
        LoopProjectFileResponse resp = {0,""};
        resp = LoopProjectFile::GetStructuralModel(name.toStdString(),data,dataShape,0,true);
        if (resp.errorCode) {
            qDebug() << resp.errorMessage.c_str();
        } else {
            m_width = dataShape[0];
            m_height = dataShape[1];
            m_depth = dataShape[2];
            m_totalPoints = m_width * m_height * m_depth;
            qDebug() << "Allocating memory for textures (" << m_width << "x" << m_height << "x" << m_depth << ")";
            m_totalTetra = (m_width-1) * (m_height-1) * (m_depth-1) * 5;
            if (m_valueData) free(m_valueData);
            m_valueData = static_cast<float*>(malloc(sizeof(float)*m_totalPoints));
            for (unsigned int i=0;i<data.size();i++) m_valueData[i] = data.data()[i];

            // Calculate Min and Max value
            m_valmin = std::numeric_limits<float>::max();
            m_valmax = std::numeric_limits<float>::min();
            for (unsigned int i=0;i<m_totalPoints;i++) {
                if (m_valueData[i] < m_valmin) m_valmin = m_valueData[i];
                if (m_valueData[i] > m_valmax) m_valmax = m_valueData[i];
            }
            m_dataChanged = true;
            m_modelCreated = true;
        }

        ProjectManagement* project =  ProjectManagement::instance();
        m_xmin = (float)project->m_minEasting;
        m_xmax = (float)project->m_maxEasting;
        m_ymin = (float)project->m_minNorthing;
        m_ymax = (float)project->m_maxNorthing;
        m_zmin = (float)project->m_botExtent;
        m_zmax = (float)project->m_topExtent;
        project->m_xsize = m_width;
        project->m_ysize = m_height;
        project->m_zsize = m_depth;
        project->xsizeChanged();
        project->ysizeChanged();
        project->zsizeChanged();

        updateStructureDataInViewer();
    }
    dataMutex.unlock();
    return result;
}

void StructuralModel::createBasicTestStructure(unsigned int size)
{
    if (size < 1) size = 1;
    m_modelCreated = false;
    dataMutex.lock();
    {
        m_xmin = m_ymin = m_zmin = 0.0f;
        m_xmax = m_ymax = m_zmax =  4000.0f;
        m_width = m_height = m_depth = size+1;
        m_totalPoints = m_width * m_height * m_depth;
        m_totalTetra = (m_width-1) * (m_height-1) * (m_depth-1) * 5;

        qDebug() << "Allocating memory for textures (" << m_width << "x" << m_height << "x" << m_depth << ")";
        // Allocate memory for textures
        if (m_valueData) free(m_valueData);
        m_valueData = static_cast<float*>(malloc(sizeof(float)*m_height*m_width*m_depth));
        if (!m_valueData) {
            qDebug() << "Failed to allocate memory for value texture";
        } else {
            m_values.clear();
            for (unsigned int i=0;i<getWidthUI();i++)
                for (unsigned int j=0;j<getHeightUI();j++)
                    for (unsigned int k=0;k<getDepthUI();k++) {
                        float val = static_cast<float>(i) / (static_cast<float>(j) + static_cast<float>(k));
                        if (val != val || isinf(val)) val = static_cast<float>(i);
                        m_values.push_back(val);
                        m_valueData[i*getHeightUI()*getDepthUI() + j*getDepthUI() + k] = val;
                    }
            m_dataChanged = true;
        }
        updateStructureDataInViewer();
    }
    dataMutex.unlock();
}

void StructuralModel::updateStructureDataInViewer()
{
    L3DViewer* viewer =  L3DViewer::instance();
    viewer->m_structureXMin = m_xmin;
    viewer->m_structureXMax = m_xmax;
    viewer->m_structureYMin = m_ymin;
    viewer->m_structureYMax = m_ymax;
    viewer->m_structureZMin = m_zmin;
    viewer->m_structureZMax = m_zmax;
    viewer->m_structureXSize = m_width;
    viewer->m_structureYSize = m_height;
    viewer->m_structureZSize = m_depth;
    viewer->m_structureXStepSize = (m_xmax - m_xmin) / m_width;
    viewer->m_structureYStepSize = (m_ymax - m_ymin) / m_height;
    viewer->m_structureZStepSize = (m_zmax - m_zmin) / m_depth;
//    viewer->m_structureXMid = (m_xmax - m_xmin) / 2.0f;
//    viewer->m_structureYMid = (m_ymax - m_ymin) / 2.0f;
//    viewer->m_structureZMid = (m_zmax - m_zmin) / 2.0f;
    viewer->m_structureNumberTetraPerIsosurface = m_totalTetra;
    viewer->allStructureChanged();
    viewer->m_minScalarValue = m_valmin;
    viewer->m_maxScalarValue = m_valmax;
    viewer->minScalarValueChanged();
    viewer->maxScalarValueChanged();
    viewer->resetView();
}

int StructuralModel::loadTextures()
{
    if (m_dataChanged) {
        qDebug() << "Setting up textures for displays";
        ProjectManagement* project =  ProjectManagement::instance();
        openGLContext = project->getQmlQuickView()->openglContext();
        if (openGLContext == nullptr) return 0;

        dataMutex.lock();
        {
            // QOpenGLTexture style
            if (valueTexture->isCreated()) {
                valueTexture->destroy();
                valueTexture->create();
            }
            valueTexture->setAutoMipMapGenerationEnabled(false);
            valueTexture->setFormat(QOpenGLTexture::R32F);
            valueTexture->setSize(getWidth(), getHeight(), getDepth());
            valueTexture->setWrapMode(QOpenGLTexture::ClampToEdge);
            valueTexture->setMinMagFilters(QOpenGLTexture::Nearest,QOpenGLTexture::Nearest);
            valueTexture->allocateStorage(QOpenGLTexture::Red,QOpenGLTexture::Float32);
            valueTexture->setData(QOpenGLTexture::Red,QOpenGLTexture::Float32,static_cast<const void*>(m_valueData));
            m_sharedTexture->setTextureId(static_cast<int>(valueTexture->textureId()));

            m_dataChanged = false;
        }
        dataMutex.unlock();
        return 1;
    }
    return 0;
}
