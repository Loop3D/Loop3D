
#include "isosurface.h"
#include "structuralmodel.h"
#include "projectmanagement.h"

#include <iostream>
#include <QtQuick/qquickwindow.h>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLExtraFunctions>
#include <QtGui/QOpenGLContext>
#include <QtDebug>
#include <QtMath>

Isosurface::Isosurface()
    : m_isovalue(0)
    , m_renderer(nullptr)
{
    m_viewDistance = 20000.0f;
    m_viewAngle = QQuaternion::fromAxisAndAngle(-1.0f,0.0f,0.0f,qDegreesToRadians(90.0f));
    connect(this, &QQuickItem::windowChanged, this, &Isosurface::handleWindowChanged);
}

void Isosurface::setIsovalue(qreal isovalue)
{
    if (abs(isovalue - m_isovalue) < 0.00001)
        return;
    m_isovalue = isovalue;
    Q_EMIT isovalueChanged();
    if (window())
        window()->update();
}

void Isosurface::updateViewAngle(float horizontal, float vertical)
{
    m_viewAngle = m_viewAngle * QQuaternion::fromAxisAndAngle(0.0f,1.0f,0.0f,(horizontal)/2.0f) * QQuaternion::fromAxisAndAngle(1.0f,0.0,0.0f,(vertical)/2.0f);
    if (window()) window()->update();
}

void Isosurface::setViewDistance(float distance)
{
    m_viewDistance = distance;
    viewDistanceChanged();
    if (window()) window()->update();
}

void Isosurface::updateViewDistance(float angle)
{
    m_viewDistance *= 1.0f+angle/10.0f;
    viewDistanceChanged();
    if (window()) window()->update();
}

void Isosurface::resetViewAngle()
{
    m_viewAngle = QQuaternion::fromAxisAndAngle(-1.0f,0.0f,0.0f,90.0f);
    if (window()) window()->update();
}

void Isosurface::handleWindowChanged(QQuickWindow *win)
{
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing, this, &Isosurface::sync, Qt::DirectConnection);
        connect(win, &QQuickWindow::sceneGraphInvalidated, this, &Isosurface::cleanup, Qt::DirectConnection);

        win->setClearBeforeRendering(false);
    }
}

void Isosurface::cleanup()
{
    if (m_renderer) {
        delete m_renderer;
        m_renderer = nullptr;
    }
}

void Isosurface::sync()
{
    if (!m_renderer) {
        m_renderer = new IsosurfaceRenderer();
        connect(window(), &QQuickWindow::beforeRendering, m_renderer, &IsosurfaceRenderer::paint, Qt::DirectConnection);
    }
    if (parent()) {
        QPointF pt = this->mapToScene(QPointF(0,0));
        m_renderer->setViewportSize(QSize(static_cast<int>(width()),static_cast<int>(height())));
        m_renderer->setViewportLocation(QPointF(pt.x(),window()->height() - height() - pt.y()));
    }
    m_renderer->setIsovalue(m_isovalue);
    m_renderer->setViewAngle(m_viewAngle);
    m_renderer->setViewDistance(m_viewDistance);
    m_renderer->setWindow(window());
}

void IsosurfaceRenderer::paint()
{
    if (!m_program) {
        initializeOpenGLFunctions();

        QString vertexShaderCode =
                "layout (location = 0) in vec4 vertex;\n"
                "layout (location = 1) in vec4 texCoord;\n"
                "uniform sampler3D textureUnit0;\n" // 3D texture of values
                "uniform float xmin;\n"
                "uniform float ymin;\n"
                "uniform float zmin;\n"
                "uniform int xsize;\n"
                "uniform int ysize;\n"
                "uniform int zsize;\n"
                "uniform float xstepsize;\n"
                "uniform float ystepsize;\n"
                "uniform float zstepsize;\n"
                "out vec2 coords;\n"
                "out vec3 color;\n"
                "out float instanceID;\n"
                "out float vsubcubeTetraID;\n"
                "uniform float isovalue;"
                "out vec3 position;\n"
                "uniform mat4 modelViewProjectionMatrix;\n"
                "\n"
                "struct Tetra\n"
                "{\n"
                "   vec4 val;\n"
                "   vec3 geometryPoint[4];\n"
                "};\n"
                "\n"
                "vec3 interp(vec3 pt1, float val1, vec3 pt2, float val2, float target)\n"
                "{\n"
                "    vec3 res = pt1 + ((target-val1)/(val2-val1) * (pt2 - pt1));\n"
                "    if (abs(val2-val1) < 0.001)\n"
                "        return pt1;\n"
                "    else\n"
                "        return pt1 + ((target-val1)/(val2-val1) * (pt2 - pt1));\n"
                "}\n"
                "\n"
                "Tetra getTetra(int subcubeTetraId, ivec3 referenceIndex)\n"
                "{\n"
                "    ivec4 tetraReference[5] = {ivec4(0,1,2,4),\n"
                "                               ivec4(3,1,2,7),\n"
                "                               ivec4(5,1,4,7),\n"
                "                               ivec4(6,2,4,7),\n"
                "                               ivec4(1,2,4,7)};\n"
                "    ivec3 cubeOffsets[8] = {ivec3(0,0,0),\n"
                "                            ivec3(1,0,0),\n"
                "                            ivec3(0,1,0),\n"
                "                            ivec3(1,1,0),\n"
                "                            ivec3(0,0,1),\n"
                "                            ivec3(1,0,1),\n"
                "                            ivec3(0,1,1),\n"
                "                            ivec3(1,1,1)};\n"
                "    Tetra res;\n"
                "    ivec3 indexToPoint[4];\n"
                "    for (int i=0;i<4;i++) {\n"
                "        indexToPoint[i] = referenceIndex + cubeOffsets[tetraReference[subcubeTetraId][i]];\n"
                "        res.geometryPoint[i] = vec3(xmin + indexToPoint[i].x * xstepsize,\n"
                "                                    ymin + indexToPoint[i].y * ystepsize,\n"
                "                                    zmin + indexToPoint[i].z * zstepsize);\n"
                "        res.val[i] = texelFetch(textureUnit0,indexToPoint[i].xyz,0).x;\n"
                "    }\n"
                "    return res;\n"
                "}\n"
                "\n"
                "void main() {\n"
                "    instanceID = gl_InstanceID;\n"
                "\n"
                "    int subcubeTetraId = int(instanceID) % 5;\n"
                "    vsubcubeTetraID = subcubeTetraId;\n"
                "    int gridIndex = int(floor(instanceID / 5));\n"
                "    ivec3 gridPos = ivec3(gridIndex % (xsize-1), int(floor(gridIndex/(xsize-1))) % (ysize-1), int(floor(gridIndex/(xsize-1)/(ysize-1))) % (zsize-1));\n"
                "    if (gridPos.x >= xsize - 1 || gridPos.y >= ysize - 1 || gridPos.z >= zsize - 1) return;"
                "    Tetra tetra = getTetra(subcubeTetraId, gridPos);\n"
                "\n"
                "\n"
                "    bool above0 = tetra.val[0] > isovalue;\n"
                "    bool above1 = tetra.val[1] > isovalue;\n"
                "    bool above2 = tetra.val[2] > isovalue;\n"
                "    bool above3 = tetra.val[3] > isovalue;\n"
                "    int sum = int(above0) + int(above1) + int(above2) + int(above3);\n"
                "    if (sum == 0 || sum == 4) {"
                "        gl_Position = vec4(0.0f);\n"
                "        return;\n"
                "    }\n"
                "\n"
                "\n"
                "    vec3 newVertex;\n"
                "    if (sum == 1 || sum == 3) {\n"
                "        if (vertex.x == 3 || vertex.x == 4 || vertex.x == 5) {\n"
                "            gl_Position = vec4(0.0f);\n"
                "            return;\n"
                "        }\n"
                "           // Work out odd one out\n"
                "        int odd = int(above0 == above1) * 2 + int(above0 == above2);\n"
                "           // Work out line to interpolate (start and end)\n"
                "        int start = int(vertex.y);\n"
                "        int end = odd;\n"
                "        if (end == start) end = 2;\n"
                "           // Interp along line (start -> end)\n"
                "        newVertex = interp(tetra.geometryPoint[start], tetra.val[start],    \n"
                "                           tetra.geometryPoint[  end], tetra.val[  end], isovalue);\n"
//                "        color = vec3((isovalue-tetra.val[start])/(tetra.val[end]-tetra.val[start]));\n"
                "        color = vec3(0.0,0.0,0.5);\n"
                "\n"
                "\n"
                "    } else if (sum == 2) {\n"
                "        int option = int(above0 == above3) * 2 + int(above0 == above2);\n"
                "        int start = int(vertex.z);\n"
                "        ivec4 offsetReference[3] = {ivec4(3,2,0,1),\n"
                "                                    ivec4(1,2,3,0),\n"
                "                                    ivec4(1,3,0,2)};\n"
                "        ivec4 ttt = offsetReference[option];\n"
                "        int end = ttt[int(vertex.z)];\n"
                "        newVertex = interp(tetra.geometryPoint[start], tetra.val[start],    \n"
                "                           tetra.geometryPoint[  end], tetra.val[  end], isovalue);\n"
                "        color = vec3(0.0,0.5,0.0);\n"
//                "        color = vec3((isovalue-tetra.val[start])/(tetra.val[end]-tetra.val[start]));\n"
//                "        color = vec3(offsetReference[option]/3.0);"
                "    }\n"
                "\n"
                "\n"
                "    coords = texCoord.xy;\n"
                "    position = newVertex / 10000.0f;\n"
                "    gl_Position = modelViewProjectionMatrix * vec4(newVertex.xyz,1.0);\n"
                "}\n";
        QString fragmentShaderCode =
                "in vec2 coords;\n"
                "in float instanceID;\n"
                "in float vsubcubeTetraID;\n"
                "in vec3 position;\n"
                "in vec3 color;\n"
                "out vec4 FragColour;\n"
                "uniform float isovalue;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    FragColour = vec4(color,1.0);\n"
//                "    FragColour = vec4(0.0,0.5,0.5,1.0);\n"
                "}\n";
        if (QOpenGLContext::currentContext()->isOpenGLES()) {
            vertexShaderCode.prepend("#version 300 es\n");
            fragmentShaderCode.prepend("#version 300 es\n");
        } else {
            vertexShaderCode.prepend("#version 430\n");
            fragmentShaderCode.prepend("#version 430\n");
        }
        m_program = new QOpenGLShaderProgram();
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderCode);
        m_program->addCacheableShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderCode);

        m_program->bindAttributeLocation("vertex", 0);
        m_program->bindAttributeLocation("texCoord", 1);
        if (!m_program->link()) qFatal("Main OpenGL shader program won't link");


        m_vao = new QOpenGLVertexArrayObject;
        if (!m_vao->create()) qFatal("Failed to create VAO");
        m_vao->bind();
        // Allocate array buffers here
        m_positionsBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        if (!m_positionsBuffer->create()) qFatal("Failed to create position buffer");
        m_positionsBuffer->bind();
        m_positionsBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        float values[] = {
            0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f,
            2.0f, 3.0f, 2.0f,
            3.0f, 0.0f, 1.0f,
            4.0f, 0.0f, 2.0f,
            5.0f, 0.0f, 3.0f
        };
        m_positionsBuffer->allocate(values,18*sizeof(float));
        m_program->enableAttributeArray("vertex");
        m_program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);

        m_texCoordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        if (!m_texCoordBuffer->create()) qFatal("Failed to create texCoord buffer");
        m_texCoordBuffer->bind();
        m_texCoordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        float texCoords[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f
        };
        m_texCoordBuffer->allocate(texCoords,12*sizeof(float));
        m_program->enableAttributeArray("texCoord");
        m_program->setAttributeBuffer("texCoord", GL_FLOAT, 0, 2);
        m_vao->release();
    }

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT);


    // Load textures from structural model
    StructuralModel* stModel = ProjectManagement::instance()->getStModel();

    m_program->bind();

    if (stModel->getDataChanged()) {
        stModel->loadTextures();
    }
    stModel->bindTextures();

    m_program->setUniformValue("isovalue", static_cast<float>(m_isovalue));
    m_program->setUniformValue("textureUnit0",0);
    m_program->setUniformValue("colourScale",10.0f);
    m_program->setUniformValue("xmin",stModel->m_xmin);
    m_program->setUniformValue("ymin",stModel->m_ymin);
    m_program->setUniformValue("zmin",stModel->m_zmin);
    m_program->setUniformValue("xmid",(stModel->m_xmin + stModel->m_xmax) / 2.0f);
    m_program->setUniformValue("ymid",(stModel->m_ymin + stModel->m_ymax) / 2.0f);
    m_program->setUniformValue("zmid",(stModel->m_zmin + stModel->m_zmax) / 2.0f);
    m_program->setUniformValue("xsize",stModel->getWidth());
    m_program->setUniformValue("ysize",stModel->getHeight());
    m_program->setUniformValue("zsize",stModel->getDepth());
    m_program->setUniformValue("xstepsize",(stModel->m_xmax-stModel->m_xmin)/static_cast<float>(stModel->getWidth()-1));
    m_program->setUniformValue("ystepsize",(stModel->m_ymax-stModel->m_ymin)/static_cast<float>(stModel->getHeight()-1));
    m_program->setUniformValue("zstepsize",(stModel->m_zmax-stModel->m_zmin)/static_cast<float>(stModel->getDepth()-1));

    m_program->setUniformValue("xmin",-4000.0f);
    m_program->setUniformValue("ymin",-4000.0f);
    m_program->setUniformValue("zmin",-4000.0f);

    QMatrix4x4 modelView;
    QMatrix4x4 modelMatrix;
    QMatrix4x4 viewMatrix;
    QMatrix4x4 projectionMatrix;
    float aspectRatio = (m_viewportSize.height() == 0 ? 4.0f/3.0f : m_viewportSize.width() / m_viewportSize.height());
    projectionMatrix.perspective(60.0f, aspectRatio, 1.0f, 50000.0f);
    viewMatrix.translate(0.0f,0.0f,-m_viewDistance);
    viewMatrix.rotate(m_viewAngle);
    modelView.scale(1.0f,1.0f,1.0f);

    const QMatrix4x4 modelViewMatrix = viewMatrix * modelMatrix;
    const QMatrix4x4 modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;

    m_program->setUniformValue("modelViewMatrix", modelViewMatrix);
    m_program->setUniformValue("normalMatrix", modelViewMatrix.normalMatrix());
    m_program->setUniformValue("projectionMatrix", projectionMatrix);
    m_program->setUniformValue("modelViewProjectionMatrix", modelViewProjectionMatrix);

    glViewport(static_cast<int>(m_viewportLoc.x()), static_cast<int>(m_viewportLoc.y()), m_viewportSize.width(), m_viewportSize.height());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_vao->bind();
    QOpenGLExtraFunctions* funcs = QOpenGLContext::currentContext()->extraFunctions();



    funcs->glDrawArraysInstanced(GL_TRIANGLES,0,6,(stModel->getWidth()-1)*(stModel->getHeight()-1)*(stModel->getDepth()-1)*5);



    m_vao->release();
    m_program->release();
    m_window->resetOpenGLState();
}


