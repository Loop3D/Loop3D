#version 430 core

in vec3 vertexPosition;

uniform sampler3D scalarField;
uniform float xmin;
uniform float ymin;
uniform float zmin;
uniform float xmax;
uniform float ymax;
uniform float zmax;
uniform int xsize;
uniform int ysize;
uniform int zsize;
uniform float xstepsize;
uniform float ystepsize;
uniform float zstepsize;
//uniform float isovalues[6];  // Why can't Qt pass vector arrays to shaders
uniform float isovalue1;
uniform float isovalue2;
uniform float isovalue3;
uniform float isovalue4;
uniform float isovalue5;
uniform float isovalue6;
uniform float isovalue7;
uniform float miscToggle1;
uniform float miscToggle2;
uniform float miscToggle3;
uniform float miscToggle4;
uniform float miscToggle5;
uniform bool invertedView;
uniform mat4 mvp;
uniform mat4 modelView;
uniform mat4 modelMatrix;


out vec3 position;

void main()
{
    position = (modelMatrix * vec4(vertexPosition,1.0)).xyz;
    if (invertedView) position.z = (zmax-zmin)-position.z;
    gl_Position = mvp * vec4(vertexPosition,1.0);
}
