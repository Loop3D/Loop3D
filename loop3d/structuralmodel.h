#ifndef STRUCTURALMODEL_H
#define STRUCTURALMODEL_H

#include <vector>
#include <pybind11/numpy.h>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QMutex>
#include <QTexture>
#include "LoopProjectFileUtils.h"
#include "LoopProjectFile.h"


class StructuralModel : protected QOpenGLFunctions
{
    public:
        StructuralModel();
        ~StructuralModel();

        int getWidth() { return static_cast<int>(m_width); }
        int getHeight() { return static_cast<int>(m_height); }
        int getDepth() { return static_cast<int>(m_depth); }
        unsigned int getWidthUI() { return m_width; }
        unsigned int getHeightUI() { return m_height; }
        unsigned int getDepthUI() { return m_depth; }
        bool getDataChanged() { return dataChanged; }
        Qt3DRender::QSharedGLTexture* getStructuralData(void) { return m_sharedTexture; }

        void loadData(pybind11::array_t<float> values_in,
                                       float xmin, float xmax, int xsteps,
                                       float ymin, float ymax, int ysteps,
                                       float zmin, float zmax, int zsteps);

        int saveToFile(QString filename);
        int loadFromFile(QString filename);

        QMutex dataMutex;
        float m_xmin, m_xmax, m_ymin, m_ymax, m_zmin, m_zmax;
        float m_valmin, m_valmax;
    private:
        void createBasicTestStructure(unsigned int size);
        void updateStructureDataInViewer(void);
        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_depth;
        unsigned int m_totalPoints;
        unsigned int m_totalTetra;
        std::vector<float> m_values;

        float* m_valueData;

        bool modelCreated;
        bool dataChanged;

       // Graphics Thread
    public:
        int loadTextures();
    private:
        QOpenGLTexture *valueTexture;
        Qt3DRender::QSharedGLTexture *m_sharedTexture;
        QOpenGLContext *openGLContext;
};

#endif // STRUCTURALMODEL_H
