#include "structuralmodel.h"
#include "projectmanagement.h"
#include <QDebug>

#define TEST_GRID_SIZE 50

StructuralModel::StructuralModel()
{
    width  = 0;
    height = 0;
    depth  = 0;

    m_xmin = -1.0;
    m_xmax =  1.0;
    m_ymin = -1.0;
    m_ymax =  1.0;
    m_zmin = -1.0;
    m_zmax =  1.0;

    m_valmin = 0.0;
    m_valmax = 1.0;

    values.clear();
    valueData = nullptr;
    dataChanged = false;
    valueTexture = new QOpenGLTexture(QOpenGLTexture::Target3D);
    texturesValid = false;
    createBasicTestStructure(TEST_GRID_SIZE);
}

StructuralModel::~StructuralModel()
{
    if (valueData) free(valueData);
    if (valueTexture) delete valueTexture;
    texturesValid = false;

}

void StructuralModel::loadData(pybind11::array_t<float> values_in, float xmin, float xmax,  int xsteps, float ymin, float ymax, int ysteps, float zmin, float zmax, int zsteps, float valmin, float valmax)
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
        m_valmin = valmin;
        m_valmax = valmax;
        // Texture sizes
        width = static_cast<unsigned int>(xsteps)+1;
        height = static_cast<unsigned int>(ysteps)+1;
        depth = static_cast<unsigned int>(zsteps)+1;

        qDebug() << "Clearing data strutures";
        // Clear data structures
        values.clear();

        qDebug() << "Allocating memory for textures (" << width << "x" << height << "x" << depth << ")";
        // Allocate memory for textures
        if (valueData) free(valueData);
        valueData = static_cast<float*>(malloc(sizeof(float)*height*width*depth));
        if (!valueData) {
            qDebug() << "Failed to allocate memory for value texture";
        } else {
            qDebug() << "Loading structures";
            // Load structures and textures
            for (unsigned int i=0;i<values_in.size();i++) {
                // Value Structure
                values.push_back(values_in.at(i));
                valueData[i] = static_cast<float>(values_in.at(i));
            }
            qDebug() << "Loaded structures";
            texturesValid = false;
            dataChanged = true;
        }
    }
    dataMutex.unlock();
}

void StructuralModel::createBasicTestStructure(unsigned int size)
{
    if (size < 1) size = 1;

    dataMutex.lock();
    {
        m_xmin = m_ymin = m_zmin = -4000.0f;
        m_xmax = m_ymax = m_zmax =  4000.0f;
        width = height = depth = size+1;

        qDebug() << "Allocating memory for textures (" << width << "x" << height << "x" << depth << ")";
        // Allocate memory for textures
        if (valueData) free(valueData);
        valueData = static_cast<float*>(malloc(sizeof(float)*height*width*depth));
        if (!valueData) {
            qDebug() << "Failed to allocate memory for value texture";
        } else {
            values.clear();
            for (unsigned int i=0;i<getWidthUI();i++)
                for (unsigned int j=0;j<getHeightUI();j++)
                    for (unsigned int k=0;k<getDepthUI();k++) {
                        float val = static_cast<float>(i) / (static_cast<float>(j) + static_cast<float>(k));
                        if (val != val || isinf(val)) val = static_cast<float>(i);
                        values.push_back(val);
                        valueData[i*getHeightUI()*getDepthUI() + j*getDepthUI() + k] = val;
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
            valueTexture->setData(QOpenGLTexture::Red,QOpenGLTexture::Float32,static_cast<const void*>(valueData));

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
