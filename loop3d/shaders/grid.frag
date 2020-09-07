#version 430 core

out vec4 FragColour;

uniform vec3 colour;

void main()
{
    FragColour = vec4(colour,1.0);
}
