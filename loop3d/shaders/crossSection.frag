#version 430 core

in float isovalue;
in vec3 position;
in vec3 normal;
in float instanceID;
in float vsubcubeTetraID;
in vec3 debugColour;
in float difference;

uniform sampler3D scalarField;
uniform float xmin;
uniform float ymin;
uniform float zmin;
uniform float xmax;
uniform float ymax;
uniform float zmax;
uniform float xstepsize;
uniform float ystepsize;
uniform float zstepsize;
uniform int xsize;
uniform int ysize;
uniform int zsize;
//uniform float xsize;
//uniform float ysize;
//uniform float zsize;
uniform float valmin;
uniform float valmax;
uniform vec3 lightPos;
uniform mat4 qt_ModelMatrix;
uniform float miscToggle1;
uniform float miscToggle2;
uniform float miscToggle3;
uniform float miscToggle4;
uniform float miscToggle5;

out vec4 FragColour;

vec3 getSpectrumColour(float val)
{
    vec3 spectrum[6] = { vec3(1.0,0.0,0.0),
                         vec3(1.0,1.0,0.0),
                         vec3(0.0,1.0,0.0),
                         vec3(0.0,1.0,1.0),
                         vec3(0.0,0.0,1.0),
                         vec3(1.0,0.0,1.0)};
    int segment = int(floor(val / 0.2));
    vec3 lower = spectrum[segment];
    vec3 upper = spectrum[segment+1];
    return vec3(mix(lower,upper,(val-0.2*segment)*5.0));
}

void main()
{
    vec3 positionNormalised = vec3((position.x-xmin/(xmax-xmin))/xstepsize,
                                   (position.y-ymin/(ymax-ymin))/ystepsize,
                                   (position.z-zmin/(zmax-zmin))/zstepsize);
    float value = texelFetch(scalarField,ivec3(positionNormalised),0).x;
    float normalisedVal = clamp((value - valmin)/(valmax-valmin),0.0,1.0);
    float alpha = 1.0;
    if (valmax == valmin) alpha = 0.0;
    if (xmax == xmin) alpha = 0.0;
    if (ymax == ymin) alpha = 0.0;
    if (zmax == zmin) alpha = 0.0;
    if (positionNormalised.x < 0) alpha = 0.0;
    if (positionNormalised.y < 0) alpha = 0.0;
    if (positionNormalised.z < 0) alpha = 0.0;
    if (positionNormalised.x > xsize) alpha = 0.0;
    if (positionNormalised.y > ysize) alpha = 0.0;
    if (positionNormalised.z > zsize) alpha = 0.0;
    vec3 colour = getSpectrumColour(normalisedVal) * 0.7 + 0.1;

    FragColour = vec4(colour,alpha);
}

