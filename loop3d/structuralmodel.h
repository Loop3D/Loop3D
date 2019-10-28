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

        int getWidth() { return static_cast<int>(width); }
        int getHeight() { return static_cast<int>(height); }
        int getDepth() { return static_cast<int>(depth); }
        unsigned int getWidthUI() { return width; }
        unsigned int getHeightUI() { return height; }
        unsigned int getDepthUI() { return depth; }
        bool getDataChanged() { return dataChanged; }

        void loadData(pybind11::array_t<float> values_in,
                                       float xmin, float xmax, int xsteps,
                                       float ymin, float ymax, int ysteps,
                                       float zmin, float zmax, int zsteps);

        QMutex dataMutex;
        float m_xmin, m_xmax, m_ymin, m_ymax, m_zmin, m_zmax;
    private:
        void createBasicTestStructure(unsigned int size);
        unsigned int width;
        unsigned int height;
        unsigned int depth;
        std::vector<float> values;

        float* valueData;

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
