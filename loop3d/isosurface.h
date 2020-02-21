/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef ISOSURFACE_H
#define ISOSURFACE_H

#include <QtQuick/QQuickItem>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLTexture>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtGui/qopenglbuffer.h>


class IsosurfaceRenderer : public QObject, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    IsosurfaceRenderer() : m_numOfIsosurfaces(1), m_program(nullptr), m_miscToggle1(0), m_miscToggle2(0), m_miscToggle3(0),
                            m_miscToggle4(0), m_miscToggle5(0){ }
    ~IsosurfaceRenderer() { delete m_program; }

    void setIsovalues(float* isovalues) { for (int i=0;i<6;i++) m_isovalues[i] = isovalues[i]; }
    void setNumberOfIsosurfaces(int num) { m_numOfIsosurfaces = num; }
    void setViewportSize(const QSize &size) { m_viewportSize = size; }
    void setViewportLocation(const QPointF &loc) { m_viewportLoc = loc; }
    void setWindow(QQuickWindow *window) { m_window = window; }
    void setViewAngle(QQuaternion angle) { m_viewAngle = angle; }
    void setViewDistance(float dist) { m_viewDistance = dist; }
    void setIsovalueMinMax(float min, float max) { m_isovalueMin = min; m_isovalueMax = max; }
    void setCameraPosition(float northing, float easting, float depth);
    void setMiscToggles(int one, int two, int three, int four, int five);

public Q_SLOTS:
    void paint();

private:
    QSize m_viewportSize;
    QPointF m_viewportLoc;
    int m_numOfIsosurfaces;
    float m_isovalueMin, m_isovalueMax;
    float m_isovalues[6];
    QOpenGLShaderProgram *m_program;
    QQuickWindow *m_window;
    QOpenGLTexture* m_texture;
    QOpenGLBuffer* m_positionsBuffer;
    QOpenGLBuffer* m_texCoordBuffer;
    QOpenGLVertexArrayObject* m_vao;
    QQuaternion m_viewAngle;
    float m_viewDistance;
    float m_camNorthing, m_camEasting, m_camDepth;
    int m_miscToggle1;
    int m_miscToggle2;
    int m_miscToggle3;
    int m_miscToggle4;
    int m_miscToggle5;
};

class Isosurface : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(float viewDistance READ viewDistance WRITE setViewDistance NOTIFY viewDistanceChanged)
    Q_PROPERTY(float isovalueMin READ isovalueMin WRITE setIsovalueMin NOTIFY isovalueMinChanged)
    Q_PROPERTY(float isovalueMax READ isovalueMax WRITE setIsovalueMax NOTIFY isovalueMaxChanged)
public:
    Isosurface();

    float viewDistance() const { return m_viewDistance; }
    void setViewDistance(float distance);
    float isovalueMin(void) { return m_isovalueMin; }
    void setIsovalueMin(float min) { m_isovalueMin = min; }
    float isovalueMax(void) { return m_isovalueMax; }
    void setIsovalueMax(float max) { m_isovalueMax = max; }

    Q_INVOKABLE void updateViewAngle(float horizontal, float vertical);
    Q_INVOKABLE void updateViewDistance(float angle);
    Q_INVOKABLE void resetViewAngle();
    Q_INVOKABLE void updateNumberOfIsosurfaces(int num);
    Q_INVOKABLE bool updateIsovalue(int index, float isovalue);
    Q_INVOKABLE void updateCameraPosition(bool up, bool down, bool left, bool right, bool forward, bool back);
    Q_INVOKABLE void updateMiscToggle1(int val);
    Q_INVOKABLE void updateMiscToggle2(int val);
    Q_INVOKABLE void updateMiscToggle3(int val);
    Q_INVOKABLE void updateMiscToggle4(int val);
    Q_INVOKABLE void updateMiscToggle5(int val);

Q_SIGNALS:
    void isovaluesChanged();
    void viewDistanceChanged();
    void isovalueMinChanged();
    void isovalueMaxChanged();

public Q_SLOTS:
    void sync();
    void cleanup();

private Q_SLOTS:
    void handleWindowChanged(QQuickWindow *win);

private:
    float m_isovalueMin;
    float m_isovalueMax;
    float m_isovalues[6];
    int m_numOfIsosurfaces;
    IsosurfaceRenderer *m_renderer;
    QQuaternion m_viewAngle;
    float m_viewDistance;
    float m_camNorthing, m_camEasting, m_camDepth;
    int m_miscToggle1, m_miscToggle2, m_miscToggle3, m_miscToggle4, m_miscToggle5;
};

#endif // ISOSURFACE_H
