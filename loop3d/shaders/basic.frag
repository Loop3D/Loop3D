#version 430 core

in vec3 position;
in vec2 vTexCoord;
in vec3 vColour;
out vec4 FragColour;

void main()
{
    FragColour = vec4(vColour,1.0);
}
