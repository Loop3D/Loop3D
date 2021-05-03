#version 430 core

in vec3 position;
in vec3 modelPos;
in vec3 normal;
flat in int instanceID;

uniform sampler2D observations;
uniform int numObservations;

uniform vec3 lightPos;
uniform float miscToggle1;
uniform float miscToggle2;
uniform float miscToggle3;
uniform float miscToggle4;
uniform float miscToggle5;
uniform float miscToggle6;
uniform float colourSteps[100];
uniform float colourRedOptions[100];
uniform float colourGreenOptions[100];
uniform float colourBlueOptions[100];

out vec4 FragColour;

//vec3 getSpectrumColour(float val)
//{
//    vec3 spectrum[6] = { vec3(1.0,0.0,0.0),
//                         vec3(1.0,1.0,0.0),
//                         vec3(0.0,1.0,0.0),
//                         vec3(0.0,1.0,1.0),
//                         vec3(0.0,0.0,1.0),
//                         vec3(1.0,0.0,1.0)};
//    int segment = int(floor(val / 0.2));
//    vec3 lower = spectrum[segment];
//    vec3 upper = spectrum[segment+1];
//    return vec3(mix(lower,upper,(val-0.2*segment)*5.0));
//}

//vec3 getColourFromSteps(float val)
//{
//    vec3 colour = vec3(colourRedOptions[0],colourGreenOptions[0],colourBlueOptions[0]);
//    for (int i=0;i<100;i++) {
//        if (val > colourSteps[i]) {
//            colour = vec3(colourRedOptions[i],colourGreenOptions[i],colourBlueOptions[i]);
//            break;
//        }
//    }
//    return colour;
//}

void main()
{
    // Add lighting
    vec3 colour = vec3(0.0,1.0,1.0);
    if (modelPos.z > 0.05) {
        colour = vec3(0.0);
    } else {
        float type = texelFetch(observations,ivec2(instanceID,1),0).x;
        if (type < 5.0) { colour = vec3(1.0,0.0,1.0); }
        if (type < 4.0) { colour = vec3(1.0,1.0,0.0); }
        if (type < 3.0) { colour = vec3(0.0,0.0,1.0); }
        if (type < 2.0) { colour = vec3(0.0,1.0,0.0); }
        if (type < 1.0) { colour = vec3(1.0,0.0,0.0); }
    }

    if (miscToggle6 < 1.0) {
        float ambient = 0.1;
        vec3 lightDir = normalize(position - lightPos);
//        colour = colour * max(abs(pow(dot(-normal,-lightDir),0.5)),ambient);
    }
    FragColour = vec4(colour,1.0);
}
