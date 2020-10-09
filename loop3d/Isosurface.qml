import QtQuick 2.14
import QtQuick.Scene3D 2.14
import Qt3D.Core 2.14
import Qt3D.Render 2.14
import Qt3D.Extras 2.14
import Qt3D.Input 2.14
import loop3d.observationmodel 1.0

Scene3DView {
    id: view1
    scene3D: mainScene3D
    width: parent.width
    height: parent.height

    Entity {
        id: scene
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
                    clearColor: "#dddddd"
                    buffersToClear: ClearBuffers.ColorDepthBuffer
                    frustumCulling: false
                    Viewport {
                        id: viewport
                        normalizedRect: Qt.rect(0,0,1,1)
                    }
                }
            },
            InputSettings  { eventSource: dataViewTab }
        ]

        Entity {
            id: backDrop
            components: [
                GeometryRenderer {
                    geometry: PlaneGeometry {
                        width: 2
                        height: 2
                        resolution: Qt.size(2,2)
                    }
                },
                DiffuseSpecularMaterial {
                    ambient: "#dddddd"
                    diffuse: "#dddddd"
                    specular: "#dddddd"
                },
                Transform { translation: Qt.vector3d(0,0,0) }
            ]

        }

        Entity {
            id: gridded3DEntity
            enabled: bar.currentIndex == 4
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
                        Parameter { name: "xmax" ; value: viewer.structureXMax },
                        Parameter { name: "ymax" ; value: viewer.structureYMax },
                        Parameter { name: "zmax" ; value: viewer.structureZMax },
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
                        Parameter { name: "invertedView" ; value: viewer.invertedView },
                        Parameter { name: "isovalues[0]" ; value: viewer.isovalues }
                    ]
                    effect: Effect {
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
                }
            ]
        }
        Entity {
            id: boundingGrid
            enabled: bar.currentIndex == 4
            components: [
                GeometryRenderer {
                    geometry: PlaneGeometry {
                        width: 10000
                        height: 10000
                        resolution: Qt.size(2,2)
                    }
                    instanceCount: 24
                },
//                Transform { translation: Qt.vector3d(0,0,1) },
                Material {
                    parameters: [
                        Parameter { name: "colour"; value: Qt.vector3d(0,0,0) },
                        Parameter { name: "xmin" ; value: 0.0 },
                        Parameter { name: "ymin" ; value: 0.0 },
                        Parameter { name: "zmin" ; value: 0.0 },
                        Parameter { name: "xmax" ; value: viewer.structureXMax - viewer.structureXMin },
                        Parameter { name: "ymax" ; value: viewer.structureYMax - viewer.structureYMin },
                        Parameter { name: "zmax" ; value: viewer.structureZMax - viewer.structureZMin }
                    ]
                    effect: Effect {
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
                                            vertexShaderCode: loadSource("qrc:/shaders/grid.vert")
                                            fragmentShaderCode: loadSource("qrc:/shaders/grid.frag")
                                        }
                                        renderStates: [ CullFace { mode: CullFace.NoCulling } ]
                                    }
                                ]
                            }
                        ]
                    }
                }
            ]
        }
        Entity {
            id: crossSectionEntity
            enabled: bar.currentIndex == 4 && viewer.miscToggle3
            property real gridSizeX: (viewer.structureXMax - viewer.structureXMin)
            property real gridSizeY: (viewer.structureXMax - viewer.structureXMin)

            components: [
                PlaneMesh {
                    id: crossSectionMesh
                    geometry: PlaneGeometry {
                        width: crossSectionEntity.gridSizeX
                        height: crossSectionEntity.gridSizeY
                        resolution: Qt.size(2,2)
                    }
                },
                Material {
                    id: crossSectionMaterial
                    parameters: [
                        Parameter { name: "color"; value: "black" },
                        Parameter { name: "xmin" ; value: viewer.structureXMin },
                        Parameter { name: "ymin" ; value: viewer.structureYMin },
                        Parameter { name: "zmin" ; value: viewer.structureZMin },
                        Parameter { name: "xmax" ; value: viewer.structureXMax },
                        Parameter { name: "ymax" ; value: viewer.structureYMax },
                        Parameter { name: "zmax" ; value: viewer.structureZMax },
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
                        Parameter { name: "invertedView" ; value: viewer.invertedView },
                        Parameter { name: "isovalue[0]" ; value: viewer.isovalues }
                    ]
                    effect: Effect {
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
                                            vertexShaderCode: loadSource("qrc:/shaders/crossSection.vert")
                                            fragmentShaderCode: loadSource("qrc:/shaders/crossSection.frag")
                                        }
                                        renderStates: [
                                            CullFace { mode: CullFace.NoCulling },
//                                            DepthTest { depthFunction: DepthTest.LessOrEqual },
//                                            NoDepthMask {},
                                            BlendEquation { blendFunction: BlendEquation.Add },
                                            BlendEquationArguments {
                                                sourceRgb: BlendEquationArguments.SourceAlpha
                                                destinationRgb: BlendEquationArguments.OneMinusSourceAlpha
                                                sourceAlpha: BlendEquationArguments.SourceAlpha
                                                destinationAlpha: BlendEquationArguments.OneMinusSourceAlpha
                                            }
                                        ]
                                    }
                                ]
                            }
                        ]
                    }
                },
                Transform {
                    property real userAngle: 0.0
                    matrix: {
                        var m = Qt.matrix4x4();
                        m.rotate(180.0, Qt.vector3d(0, 0, 1))
                        m.translate(Qt.vector3d(-(viewer.csPositionX-viewer.structureXMin + crossSectionEntity.gridSizeX * 0.0),
                                                -(viewer.csPositionY-viewer.structureYMin + crossSectionEntity.gridSizeY * 0.0),
                                                 (viewer.csPositionZ-viewer.structureZMin + crossSectionEntity.gridSizeX * 0.0)));
                        m.rotate(viewer.csOrientationPsi, Qt.vector3d(0, 0, 1))
                        m.rotate(viewer.csOrientationTheta, Qt.vector3d(1, 0, 0))

                        return m;
                    }
                }
            ]
        }
        Entity {
            Text2DEntity {
                color: "red"
                height: 20
                width: 200
                text: viewer.structureXMax
                font.family: "Helvetica"
                components: [
                    Transform {
                        matrix: {
                            var m = Qt.matrix4x4();
                            m.translate(Qt.vector3d(viewer.structureXMax-viewer.structureXMin,0.0,0));
                            m.scale(50.0);
                            m.rotate(-90.0,Qt.vector3d(0,0,1))
                            return m;
                        }
                    }
                ]
            }
        }
        Entity {
            Text2DEntity {
                color: "red"
                height: 20
                width: 200
                text: viewer.structureXMin
                font.family: "Helvetica"
                components: [
                    Transform {
                        matrix: {
                            var m = Qt.matrix4x4();
                            m.translate(Qt.vector3d(-500.0,0.0,0));
                            m.scale(50.0);
                            m.rotate(-90.0,Qt.vector3d(0,0,1))
                            return m;
                        }
                    }
                ]
            }
        }
        Entity {
            Text2DEntity {
                color: "blue"
                height: 20
                width: 200
                text: viewer.structureYMax
                font.family: "Helvetica"
                components: [
                    Transform {
                        matrix: {
                            var m = Qt.matrix4x4();
                            m.translate(Qt.vector3d(-500.0,viewer.structureYMax-viewer.structureYMin,0));
                            m.scale(50.0);
                            return m;
                        }
                    }
                ]
            }
        }
        Entity {
            Text2DEntity {
                color: "blue"
                height: 20
                width: 200
                text: viewer.structureYMin
                font.family: "Helvetica"
                components: [
                    Transform {
                        matrix: {
                            var m = Qt.matrix4x4();
                            m.translate(Qt.vector3d(-500.0,0.0,0));
                            m.scale(50.0);
                            return m;
                        }
                    }
                ]
            }
        }
        Entity {
            components: [
                SphereMesh { radius: 50 },
                Transform { translation: Qt.vector3d(viewer.structureXMax-viewer.structureXMin,
                                                     viewer.structureYMax-viewer.structureYMin,
                                                     viewer.structureZMax-viewer.structureZMin) },
                DiffuseSpecularMaterial {
                    ambient: "#555555"
                    diffuse: "#ee0000"
                    specular: "#00ee00"
                }
            ]
        }
        NodeInstantiator {
            id: observations
            enabled: bar.currentIndex == 4
            model: observationsModel
            delegate: Entity {
                id: observationEntity
                property real elemDip: dip
                property real elemDipDir: dipDir
                components: [
                    Mesh {
                        id: up_arrow
                        source: "qrc:/3dassets/Up_arrow_v1.obj"
                    },
                    Transform {
                        scale: 50.0
                         // First rotation is the dip angle and the next is the dip direction
                        rotation: fromAxesAndAngles(Qt.vector3d(1.0,0.0,0.0),(viewer.invertedView?dip:180.0-elemDip),Qt.vector3d(0.0,0.0,1.0),180.0-(elemDipDir))
                        translation: (viewer.invertedView ?
                           Qt.vector3d(easting-viewer.structureXMin,northing-viewer.structureYMin,viewer.structureZMax-altitude)
                          :
                           Qt.vector3d(easting-viewer.structureXMin,northing-viewer.structureYMin,altitude-viewer.structureZMin)
                          )
                    },
                    DiffuseSpecularMaterial {
                        ambient: "#ff9955"
                        diffuse: "#222222"
                        specular: "#ff0000"
                    }
                ]
            }
        }
    }
}
