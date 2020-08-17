#version 430 core

in float isovalue;
in vec3 position;
in vec3 normal;
in float instanceID;
in float vsubcubeTetraID;
in vec3 debugColour;
in float difference;

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
    float normalisedVal = clamp((isovalue - valmin)/(valmax-valmin),0.0,1.0);
    if (valmax == valmin) normalisedVal = 0.5;
    vec3 colour = getSpectrumColour(normalisedVal) * 0.7 + 0.1;
    // Add contour lines with 5.0 stepped at 2000
    colour = mix(vec3(0.0,0.0,0.0),colour,step(5.0,abs(float(int(position.z)%2000))));
    // Add lighting
    float ambient = 0.1;
    vec3 lightDir = normalize(position - lightPos);
    colour = colour * max(abs(dot(-normal,-lightDir)),0.0);
    if (difference > 2000.0 && miscToggle3 > 0.0) FragColour = vec4(0.7,0.2,0.2,1.0);
    else FragColour = vec4(colour,1.0);
}

