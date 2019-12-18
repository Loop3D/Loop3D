#include "structuralmodel.h"
#include "projectmanagement.h"
#include <limits>

#include <QDebug>
#include <netcdf>

#define TEST_GRID_SIZE 50

StructuralModel::StructuralModel()
{
    m_width  = 0;
    m_height = 0;
    m_depth  = 0;

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
    dataChanged = false;
    valueTexture = new QOpenGLTexture(QOpenGLTexture::Target3D);
    texturesValid = false;
    createBasicTestStructure(TEST_GRID_SIZE);
}

StructuralModel::~StructuralModel()
{
    if (m_valueData) free(m_valueData);
    if (valueTexture) delete valueTexture;
    texturesValid = false;

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
        m_width = static_cast<unsigned int>(xsteps)+1;
        m_height = static_cast<unsigned int>(ysteps)+1;
        m_depth = static_cast<unsigned int>(zsteps)+1;

        qDebug() << "Clearing data strutures";
        // Clear data structures
        m_values.clear();

        qDebug() << "Allocating memory for textures (" << m_width << "x" << m_height << "x" << m_depth << ")";
        // Allocate memory for textures
        if (m_valueData) free(m_valueData);
        m_valueData = static_cast<float*>(malloc(sizeof(float)*m_height*m_width*m_depth));
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
            texturesValid = false;
            dataChanged = true;
        }
    }
    dataMutex.unlock();
}

int StructuralModel::saveToFile(QString filename)
{
    int result = 0;
    dataMutex.lock();
    try {
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

        netCDF::NcFile dataFile(name.toStdString().c_str(), netCDF::NcFile::write);
        netCDF::NcGroup structuralModels = dataFile.getGroup("StructuralModels");
        netCDF::NcDim northing;
        netCDF::NcDim easting;
        netCDF::NcDim depth;
        netCDF::NcDim index;
        if (!structuralModels.isNull()) {
            northing = structuralModels.getDim("northing");
            easting = structuralModels.getDim("easting");
            depth = structuralModels.getDim("depth");
            index = structuralModels.getDim("index");
            if (northing.getSize() != m_width || easting.getSize() != m_height || depth.getSize() != m_depth) {
                dataFile.close();
                //throw std::exception("Non matching dimensions between file and structural data");
            }
        } else {
            structuralModels = dataFile.addGroup("StructuralModels");
            northing = structuralModels.addDim("northing",m_width);
            easting = structuralModels.addDim("easting",m_height);
            depth = structuralModels.addDim("depth",m_depth);
            index = structuralModels.addDim("index");
        }
        std::vector<netCDF::NcDim> dims;
        dims.push_back(northing);
        dims.push_back(easting);
        dims.push_back(depth);
        dims.push_back(index);
        netCDF::NcVar data = structuralModels.addVar("data",netCDF::ncFloat,dims);
        data.setCompression(true, true, 9);
        std::vector<size_t> start, count;
        start.push_back(0);
        start.push_back(0);
        start.push_back(0);
        start.push_back(0);
        count.push_back(m_width);
        count.push_back(m_height);
        count.push_back(m_depth);
        count.push_back(1);
        data.putVar(start,count,m_valueData);

        std::vector<netCDF::NcDim> indexDims;
        start.clear();
        start.push_back(0);
        count.clear();
        count.push_back(1);
        netCDF::NcVar minValData = structuralModels.addVar("minVal",netCDF::ncFloat,indexDims);
        minValData.putVar(start,count,&m_valmin);
        netCDF::NcVar maxValData = structuralModels.addVar("maxVal",netCDF::ncFloat,indexDims);
        maxValData.putVar(start,count,&m_valmax);
        dataFile.close();
    } catch(netCDF::exceptions::NcException& e) {
        qFatal("%s", e.what());
        qFatal("Error creating file (%s)", filename.toStdString().c_str());
        result = 1;
    } catch(std::exception& e) {
        qFatal("%s", e.what());
        qFatal("Error creating file (%s)", filename.toStdString().c_str());
        result = 1;
    }

    dataMutex.unlock();
    return result;
}

int StructuralModel::loadFromFile(QString filename)
{

    int result = 0;
    dataMutex.lock();
    try {
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

        netCDF::NcFile dataFile(name.toStdString().c_str(), netCDF::NcFile::read);

        netCDF::NcGroup structuralModels = dataFile.getGroup("StructuralModels");
        if (structuralModels.isNull()) {
            qDebug() << "No Structural Models Group in netCDF file";
        } else {
            netCDF::NcDim northing = structuralModels.getDim("northing");
            netCDF::NcDim easting = structuralModels.getDim("easting");
            netCDF::NcDim depth = structuralModels.getDim("depth");
            m_width = static_cast<unsigned int>(northing.getSize());
            m_height= static_cast<unsigned int>(easting.getSize());
            m_depth = static_cast<unsigned int>(depth.getSize());

            netCDF::NcDim index = structuralModels.getDim("index");
            netCDF::NcVar data = structuralModels.getVar("data");
            std::vector<size_t> start;
            start.push_back(0);
            start.push_back(0);
            start.push_back(0);
            start.push_back(0);
            std::vector<size_t> count;
            count.push_back(m_width);
            count.push_back(m_height);
            count.push_back(m_depth);
            count.push_back(1);
            if (m_valueData) free(m_valueData);
            m_valueData = static_cast<float*>(malloc(sizeof(float)*m_height*m_width*m_depth));
            data.getVar(start,count,m_valueData);

            std::vector<netCDF::NcDim> indexDims;
            start.clear();
            start.push_back(0);
            count.clear();
            count.push_back(1);
            netCDF::NcVar minValData = structuralModels.getVar("minVal");
            minValData.getVar(start,count,&m_valmin);
            netCDF::NcVar maxValData = structuralModels.getVar("maxVal");
            maxValData.getVar(start,count,&m_valmax);

            // Clean up
            dataFile.close();
            texturesValid = false;
            dataChanged = true;
        }
    } catch(netCDF::exceptions::NcException& e) {
        qFatal("%s", e.what());
        qFatal("Error creating file (%s)", filename.toStdString().c_str());
        result = 1;
    }
    dataMutex.unlock();
    return result;
}

void StructuralModel::createBasicTestStructure(unsigned int size)
{
    if (size < 1) size = 1;

    dataMutex.lock();
    {
        m_xmin = m_ymin = m_zmin = -4000.0f;
        m_xmax = m_ymax = m_zmax =  4000.0f;
        m_width = m_height = m_depth = size+1;

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
            texturesValid = false;
            dataChanged = true;
        }
    }
    dataMutex.unlock();
}

void StructuralModel::loadTextures()
{

    if (dataChanged) {
        qDebug() << "Setting up textures for displays";
        dataMutex.lock();
        {
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

            texturesValid = true;
            dataChanged = false;
        }
        dataMutex.unlock();
    }
}

void StructuralModel::bindTextures()
{
    if (texturesValid) {
        valueTexture->bind(0);
    }
}
