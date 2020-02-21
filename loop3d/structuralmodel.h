#ifndef STRUCTURALMODEL_H
#define STRUCTURALMODEL_H

#include <vector>
#include <pybind11/numpy.h>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QMutex>

struct Tetra {
    float nodeIndex[4];
};

struct Nodes {
    double point[3];
    Nodes(double x, double y, double z) { point[0] = x; point[1] = y; point[2] = z; }
};

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
        unsigned int m_width;
        unsigned int m_height;
        unsigned int m_depth;
        std::vector<float> m_values;

        float* m_valueData;

        bool modelCreated;
        bool dataChanged;

       // Graphics Thread
    public:
        void loadTextures();
        void bindTextures();
    private:
        QOpenGLTexture *valueTexture;
        bool texturesValid;
};

#endif // STRUCTURALMODEL_H
