#ifndef L3DVIEWER_H
#define L3DVIEWER_H


#include <QObject>
#include <QtQuick/QQuickItem>
#include <QVector3D>

class L3DViewer : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY (bool invertedView MEMBER m_invertedView NOTIFY invertedViewChanged)
    Q_PROPERTY (double minScalarValue MEMBER m_minScalarValue NOTIFY minScalarValueChanged)
    Q_PROPERTY (double maxScalarValue MEMBER m_maxScalarValue NOTIFY maxScalarValueChanged)
    Q_PROPERTY (double numIsosurfaces MEMBER m_numIsosurfaces NOTIFY numIsosurfacesChanged)
    Q_PROPERTY (QList<float> isovalues MEMBER m_isovalues NOTIFY isovaluesChanged)
    Q_PROPERTY (int miscToggle1 MEMBER m_miscToggle1 NOTIFY miscToggle1Changed)
    Q_PROPERTY (int miscToggle2 MEMBER m_miscToggle2 NOTIFY miscToggle2Changed)
    Q_PROPERTY (int miscToggle3 MEMBER m_miscToggle3 NOTIFY miscToggle3Changed)
    Q_PROPERTY (int miscToggle4 MEMBER m_miscToggle4 NOTIFY miscToggle4Changed)
    Q_PROPERTY (int miscToggle5 MEMBER m_miscToggle5 NOTIFY miscToggle5Changed)
    Q_PROPERTY (int miscToggle6 MEMBER m_miscToggle6 NOTIFY miscToggle6Changed)
    Q_PROPERTY (QVector3D cameraPosition MEMBER m_cameraPosition WRITE setCameraPosition NOTIFY cameraPositionChanged)
    Q_PROPERTY (QVector3D cameraOffsetCP MEMBER m_cameraOffsetCP WRITE setCameraOffsetCP NOTIFY cameraOffsetCPChanged)
    Q_PROPERTY (QVector3D lookatPosition MEMBER m_lookatPosition WRITE setLookAtPosition NOTIFY lookatPositionChanged)
    Q_PROPERTY (float structureXMin MEMBER m_structureXMin NOTIFY structureXMinChanged)
    Q_PROPERTY (float structureXMax MEMBER m_structureXMax NOTIFY structureXMaxChanged)
    Q_PROPERTY (float structureYMin MEMBER m_structureYMin NOTIFY structureYMinChanged)
    Q_PROPERTY (float structureYMax MEMBER m_structureYMax NOTIFY structureYMaxChanged)
    Q_PROPERTY (float structureZMin MEMBER m_structureZMin NOTIFY structureZMinChanged)
    Q_PROPERTY (float structureZMax MEMBER m_structureZMax NOTIFY structureZMaxChanged)
    Q_PROPERTY (float structureXMid MEMBER m_structureXMid NOTIFY structureXMidChanged)
    Q_PROPERTY (float structureYMid MEMBER m_structureYMid NOTIFY structureYMidChanged)
    Q_PROPERTY (float structureZMid MEMBER m_structureZMid NOTIFY structureZMidChanged)
    Q_PROPERTY (float structureXStepSize MEMBER m_structureXStepSize NOTIFY structureXStepSizeChanged)
    Q_PROPERTY (float structureYStepSize MEMBER m_structureYStepSize NOTIFY structureYStepSizeChanged)
    Q_PROPERTY (float structureZStepSize MEMBER m_structureZStepSize NOTIFY structureZStepSizeChanged)
    Q_PROPERTY (unsigned int structureXSize MEMBER m_structureXSize NOTIFY structureXSizeChanged)
    Q_PROPERTY (unsigned int structureYSize MEMBER m_structureYSize NOTIFY structureYSizeChanged)
    Q_PROPERTY (unsigned int structureZSize MEMBER m_structureZSize NOTIFY structureZSizeChanged)
    Q_PROPERTY (unsigned int structureNumberTetraPerIsosurface MEMBER m_structureNumberTetraPerIsosurface NOTIFY structureNumberTetraPerIsosurfaceChanged)
    Q_PROPERTY (QList<float> colourSteps MEMBER m_colourSteps NOTIFY colourStepsChanged)
    Q_PROPERTY (QList<float> colourRedOptions MEMBER m_colourRedOptions NOTIFY colourRedOptionsChanged)
    Q_PROPERTY (QList<float> colourGreenOptions MEMBER m_colourGreenOptions NOTIFY colourGreenOptionsChanged)
    Q_PROPERTY (QList<float> colourBlueOptions MEMBER m_colourBlueOptions NOTIFY colourBlueOptionsChanged)

    Q_PROPERTY (float csPositionX MEMBER m_csPositionX NOTIFY csPositionXChanged)
    Q_PROPERTY (float csPositionY MEMBER m_csPositionY NOTIFY csPositionYChanged)
    Q_PROPERTY (float csPositionZ MEMBER m_csPositionZ NOTIFY csPositionZChanged)
    Q_PROPERTY (float csOrientationPsi MEMBER m_csOrientationPsi NOTIFY csOrientationPsiChanged)
    Q_PROPERTY (float csOrientationTheta MEMBER m_csOrientationTheta NOTIFY csOrientationThetaChanged)
Q_SIGNALS:
    void invertedViewChanged();
    void minScalarValueChanged();
    void maxScalarValueChanged();
    void miscToggle1Changed();
    void miscToggle2Changed();
    void miscToggle3Changed();
    void miscToggle4Changed();
    void miscToggle5Changed();
    void miscToggle6Changed();
    void lightPositionChanged();
    void cameraPositionChanged();
    void cameraOffsetCPChanged();
    void lookatPositionChanged();
    void numIsosurfacesChanged();
    void isovaluesChanged();
    void structureXMinChanged();
    void structureXMaxChanged();
    void structureYMinChanged();
    void structureYMaxChanged();
    void structureZMinChanged();
    void structureZMaxChanged();
    void structureXMidChanged();
    void structureYMidChanged();
    void structureZMidChanged();
    void structureXStepSizeChanged();
    void structureYStepSizeChanged();
    void structureZStepSizeChanged();
    void structureXSizeChanged();
    void structureYSizeChanged();
    void structureZSizeChanged();
    void structureNumberTetraPerIsosurfaceChanged();
    void colourStepsChanged();
    void colourRedOptionsChanged();
    void colourGreenOptionsChanged();
    void colourBlueOptionsChanged();

    void csPositionXChanged();
    void csPositionYChanged();
    void csPositionZChanged();
    void csOrientationPsiChanged();
    void csOrientationThetaChanged();

public Q_SLOTS:
    void setCameraPosition(QVector3D val) { m_cameraPosition = val; cameraPositionChanged(); }
    void setCameraOffsetCP(QVector3D val);
    void setLookAtPosition(QVector3D val) { m_lookatPosition = val; lookatPositionChanged(); }
    void zoom(float factor);
    void pan(float horizontalFactor, float verticalFactor);
    void rotate(float horizontalFactor, float verticalFactor);
    void resetView(void);
    void resetColours(void);
    void resetCrossSection(void);
    void handleKeyEvent(int key, int modifiers);

public:
    Q_INVOKABLE bool setIsovalue(int index, float value);
    Q_INVOKABLE bool setColourStep(int index, float value);
    Q_INVOKABLE bool setColourOption(int index, QVector3D value);
    Q_INVOKABLE bool setColourRedOption(int index, float value);
    Q_INVOKABLE bool setColourGreenOption(int index, float value);
    Q_INVOKABLE bool setColourBlueOption(int index, float value);
    static L3DViewer* instance() {
        if (!m_instance) m_instance = new L3DViewer;
        return m_instance;
    }
    void allStructureChanged();

    bool m_invertedView;
    float m_minScalarValue;
    float m_maxScalarValue;
    int m_numIsosurfaces;
    QList<float> m_isovalues;
    int m_miscToggle1;
    int m_miscToggle2;
    int m_miscToggle3;
    int m_miscToggle4;
    int m_miscToggle5;
    int m_miscToggle6;
    QVector3D m_cameraPosition;
    QVector3D m_cameraOffsetCP;
    QVector3D m_lookatPosition;

    float m_structureXMin;
    float m_structureXMax;
    float m_structureYMin;
    float m_structureYMax;
    float m_structureZMin;
    float m_structureZMax;
    float m_structureXMid;
    float m_structureYMid;
    float m_structureZMid;
    float m_structureXStepSize;
    float m_structureYStepSize;
    float m_structureZStepSize;
    unsigned int m_structureXSize;
    unsigned int m_structureYSize;
    unsigned int m_structureZSize;
    unsigned int m_structureNumberTetraPerIsosurface;
    QList<float> m_colourSteps;
    QList<float> m_colourRedOptions;
    QList<float> m_colourGreenOptions;
    QList<float> m_colourBlueOptions;

    float m_csPositionX;
    float m_csPositionY;
    float m_csPositionZ;
    float m_csOrientationPsi;
    float m_csOrientationTheta;

private:
    L3DViewer();
    static L3DViewer* m_instance;
};

#endif // 3DVIEWER_H
