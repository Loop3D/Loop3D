#version 430 core

uniform mat4 mvp;

in vec3 vertexColor;
in vec3 vertexPosition;

out vec3 vColour;
void main()
{
    vColour = vertexColor;
    gl_Position = mvp * vec4(vertexPosition,1.0);
}
