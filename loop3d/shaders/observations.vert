#version 430 core

in vec3 vertexPosition;

uniform sampler2D observations;
uniform int numObservations;
uniform float xmin;
uniform float xmax;
uniform float ymin;
uniform float ymax;
uniform float zmin;
uniform float zmax;
uniform float miscToggle1;
uniform float miscToggle2;
uniform float miscToggle3;
uniform float miscToggle4;
uniform float miscToggle5;
uniform float miscToggle6;
uniform bool invertedView;
uniform mat4 mvp;

out vec3 position;
out vec3 modelPos;
out vec3 normal;
flat out int instanceID;

void main() {
    float scale = 100.0;
    instanceID = gl_InstanceID;
    vec3 newVertex = vec3(0);
    vec3 obsPos = vec3(0);
    vec3 obsOrient = vec3(0);
    if (gl_InstanceID < numObservations) {
        obsPos = texelFetch(observations,ivec2(instanceID,0),0).xyz;
        obsOrient = texelFetch(observations,ivec2(instanceID,1),0).xyz;
    }
    mat3x3 zRotMatCW = mat3(cos(-obsOrient.y),-sin(-obsOrient.y),0,
                            sin(-obsOrient.y), cos(-obsOrient.y),0,
                                           0,                0,1);
    mat3x3 xRotMat = mat3(1,               0,                0,
                            0,cos(obsOrient.z),-sin(obsOrient.z),
                            0,sin(obsOrient.z), cos(obsOrient.z));
    newVertex = vertexPosition * -xRotMat * zRotMatCW * scale + obsPos;
    if (invertedView) newVertex.z = (zmax-zmin)-newVertex.z;

    modelPos = vertexPosition;
    position = obsPos;
//    normal = normalize(vertexPosition * -xRotMat * -zRotMatCW);
    gl_Position = mvp * vec4(newVertex.xyz,1.0);
}
