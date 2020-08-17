#include "3dviewer.h"
#include "projectmanagement.h"
#include <QMatrix3x3>
#include <Qt3DCore>
#include <QtMath>

L3DViewer* L3DViewer::m_instance = nullptr;

void L3DViewer::setCameraOffsetCP(QVector3D val)
{
    m_cameraOffsetCP = val;
    cameraOffsetCPChanged();
    QMatrix4x4 m;
    m.rotate(-90.0f,1.0,0.0,0.0);
    m.rotate(m_cameraOffsetCP.y(),1.0,0.0,0.0);
    m.rotate(m_cameraOffsetCP.x(),0.0,0.0,1.0);
    setCameraPosition(m_lookatPosition + QVector3D(0.0,0.0,m_cameraOffsetCP.z()) * m);
}

void L3DViewer::zoom(float factor)
{
    float newDist = m_cameraOffsetCP.z() * (1.0f + factor*0.05f);
    if (newDist < 1.0f) newDist = 1.0f;
    m_cameraOffsetCP.setZ(newDist);
    setCameraOffsetCP(m_cameraOffsetCP);
}

void L3DViewer::pan(float horizontalFactor, float verticalFactor)
{
    QVector3D offset(-horizontalFactor*-cos(qDegreesToRadians(m_cameraOffsetCP.x())),
                     -horizontalFactor*sin(qDegreesToRadians(m_cameraOffsetCP.x())),
                     -verticalFactor);
    float multiplicationFactor = m_cameraOffsetCP.z() / 1000.0f;
    setLookAtPosition(m_lookatPosition + offset*multiplicationFactor);
    setCameraOffsetCP(m_cameraOffsetCP);
}

void L3DViewer::rotate(float horizontalFactor, float verticalFactor)
{
    float newHorRot = m_cameraOffsetCP.x() - horizontalFactor;
    float newVerRot = m_cameraOffsetCP.y() - verticalFactor / 2.0f;
    if (newVerRot > 89.0f) newVerRot = 89.0f;
    if (newVerRot < -89.0f) newVerRot = -89.0f;
    m_cameraOffsetCP.setX(newHorRot);
    m_cameraOffsetCP.setY(newVerRot);
    setCameraOffsetCP(m_cameraOffsetCP);
}

void L3DViewer::resetView()
{
    setLookAtPosition(QVector3D((m_structureXMax-m_structureXMin)/2.0f,(m_structureYMax-m_structureYMin)/2.0f,(m_structureZMax-m_structureZMin)/2.0f));
    setCameraOffsetCP(QVector3D(0.0,10.0,(m_structureYMax-m_structureYMin)*1.5f));
}

void L3DViewer::resetCrossSection()
{
    m_csPositionX = (m_structureXMax-m_structureXMin)/2.0f;
    m_csPositionY = (m_structureYMax-m_structureYMin)/2.0f;
    m_csPositionZ = (m_structureZMax-m_structureZMin)/2.0f;
    m_csOrientationPsi = 0.0f;
    m_csOrientationTheta = 0.0f;
}

void L3DViewer::handleKeyEvent(int key, int modifiers)
{
    if (key == Qt::Key::Key_A || key == Qt::Key::Key_Left)       pan( 10.0f,  0.0f);
    else if (key == Qt::Key::Key_W || key == Qt::Key::Key_Up)    pan(  0.0f, 10.0f);
    else if (key == Qt::Key::Key_S || key == Qt::Key::Key_Down)  pan(  0.0f,-10.0f);
    else if (key == Qt::Key::Key_D || key == Qt::Key::Key_Right) pan(-10.0f,  0.0f);
    else if (modifiers & Qt::KeypadModifier) {
        if (key == Qt::Key::Key_4)      pan( 10.0f,  0.0f);
        else if (key == Qt::Key::Key_8) pan(  0.0f, 10.0f);
        else if (key == Qt::Key::Key_2) pan(  0.0f,-10.0f);
        else if (key == Qt::Key::Key_6) pan(-10.0f,  0.0f);
    }
}

void L3DViewer::allStructureChanged()
{
    structureXMinChanged();
    structureXMaxChanged();
    structureYMinChanged();
    structureYMaxChanged();
    structureZMinChanged();
    structureZMaxChanged();
    structureXSizeChanged();
    structureYSizeChanged();
    structureZSizeChanged();
    structureXStepSizeChanged();
    structureYStepSizeChanged();
    structureZStepSizeChanged();
    structureNumberTetraPerIsosurfaceChanged();
    csPositionXChanged();
    csPositionYChanged();
    csPositionZChanged();
    csOrientationPsiChanged();
    csOrientationThetaChanged();
    lookatPositionChanged();
    cameraOffsetCPChanged();
    cameraPositionChanged();
}

L3DViewer::L3DViewer()
{
    m_invertedView = false;
    m_minScalarValue = 0.0;
    m_maxScalarValue = 1.0;
    m_isovalue = 0.0;
    m_numIsosurfaces = 1;
    m_miscToggle1 = true;
    m_miscToggle2 = false;
    m_miscToggle3 = false;
    m_miscToggle4 = false;
    m_miscToggle5 = false;
    m_structureXMin = 0.0f;
    m_structureXMax = 1.0f;
    m_structureYMin = 0.0f;
    m_structureYMax = 1.0f;
    m_structureZMin = 0.0f;
    m_structureZMax = 1.0f;
    m_structureXSize = 51;
    m_structureYSize = 51;
    m_structureZSize = 51;
    m_structureXStepSize = (m_structureXMax-m_structureXMin)/m_structureXSize;
    m_structureYStepSize = (m_structureYMax-m_structureYMin)/m_structureYSize;
    m_structureZStepSize = (m_structureZMax-m_structureZMin)/m_structureZSize;
    m_structureNumberTetraPerIsosurface = (m_structureXSize-1)*(m_structureYSize-1)*(m_structureZSize-1) * 5;
    resetView();
    resetCrossSection();
}
