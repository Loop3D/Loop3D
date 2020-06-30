import QtQuick 2.14
import QtQuick.Scene3D 2.14
import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Extras 2.14
import Qt3D.Input 2.14

Scene3DView {
    id: view1
    scene3D: mainScene3D
    width: parent.width
    height: parent.height

    Entity {
        id: scene
        property bool clearColor: true

        Camera {
            id: camera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 60
            nearPlane : 1.0
            farPlane : 500000.0
            position: viewer.cameraPosition
            upVector: Qt.vector3d( 0.0, 0.0, 1.0 )
            viewCenter: viewer.lookatPosition
        }

        FirstPersonCameraController { camera: camera }

        components: [
            RenderSettings {
                activeFrameGraph: ForwardRenderer {
                    camera: camera
                    clearColor: "transparent"
                    buffersToClear: scene.clearColor ? ClearBuffers.ColorDepthBuffer : ClearBuffers.DepthBuffer
                    frustumCulling: false
                }
            }
            ,
            InputSettings  { eventSource: dataViewTab }
        ]
        Entity {
            id: sphereEntity
            enabled: bar.currentIndex == 2
            components: [
                GeometryRenderer {
                    id: meshInstance
                    geometry: PlaneGeometry {
                        width: 10000
                        height: 10000
                        resolution: Qt.size(2,3)
                    }
                    instanceCount: viewer.structureNumberTetraPerIsosurface * viewer.numIsosurfaces
                },
                Material {
                    id: isosurfaceMaterial
                    parameters: [
                        Parameter { name: "xmin" ; value: viewer.structureXMin },
                        Parameter { name: "ymin" ; value: viewer.structureYMin },
                        Parameter { name: "zmin" ; value: viewer.structureZMin },
                        Parameter { name: "xsize" ; value: viewer.structureXSize },
                        Parameter { name: "ysize" ; value: viewer.structureYSize },
                        Parameter { name: "zsize" ; value: viewer.structureZSize },
                        Parameter { name: "xstepsize" ; value: viewer.structureXStepSize },
                        Parameter { name: "ystepsize" ; value: viewer.structureYStepSize },
                        Parameter { name: "zstepsize" ; value: viewer.structureZStepSize },
                        Parameter { name: "valmin" ; value: viewer.minScalarValue },
                        Parameter { name: "valmax" ; value: viewer.maxScalarValue },
                        Parameter { name: "miscToggle1" ; value: viewer.miscToggle1 },
                        Parameter { name: "miscToggle2" ; value: viewer.miscToggle2 },
                        Parameter { name: "miscToggle3" ; value: viewer.miscToggle3 },
                        Parameter { name: "miscToggle4" ; value: viewer.miscToggle4 },
                        Parameter { name: "miscToggle5" ; value: viewer.miscToggle5 },
                        Parameter { name: "numTetraPerIso" ; value: viewer.structureNumberTetraPerIsosurface },
                        Parameter { name: "lightPos" ; value: Qt.vector3d(0.0,0.0,500000.0) },
                        Parameter { name: "scalarField" ; value: project.getStructuralModelData() },
                        Parameter { name: "isovalue1" ; value: viewer.isovalue },
                        Parameter { name: "isovalue2" ; value: viewer.isovalue2 },
                        Parameter { name: "isovalue3" ; value: viewer.isovalue3 },
                        Parameter { name: "isovalue4" ; value: viewer.isovalue4 },
                        Parameter { name: "isovalue5" ; value: viewer.isovalue5 },
                        Parameter { name: "isovalue6" ; value: viewer.isovalue6 },
                        Parameter { name: "isovalue7" ; value: viewer.isovalue7 }
                    ]
                    effect: Effect {
                        id: effect
                        techniques: [
                            Technique {
                                filterKeys: FilterKey { name: "renderingStyle"; value: "forward" }
                                graphicsApiFilter {
                                    api: GraphicsApiFilter.OpenGL
                                    profile: GraphicsApiFilter.CoreProfile
                                    majorVersion: 4
                                    minorVersion: 3
                                }
                                renderPasses: [
                                    RenderPass {
                                        shaderProgram: ShaderProgram {
                                            vertexShaderCode: loadSource("qrc:/shaders/isosurface.vert")
                                            fragmentShaderCode: loadSource("qrc:/shaders/isosurface.frag")
                                        }
                                        renderStates: [ CullFace { mode: CullFace.NoCulling } ]
                                    }
                                ]
                            }
                        ]
                    }
                },
                Transform {
                    id: sphereTransform
                    property real userAngle: 0.0
                    matrix: {
                        var m = Qt.matrix4x4();
                        m.rotate(userAngle, Qt.vector3d(0, 1, 1))
                        m.translate(Qt.vector3d(20, 0, 0));
                        return m;
                    }
                }
            ]
        }
    }
}
