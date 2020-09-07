#version 430 core

uniform mat4 modelMatrix;
uniform mat4 mvp;
uniform float xmin;
uniform float ymin;
uniform float zmin;
uniform float xmax;
uniform float ymax;
uniform float zmax;

void main()
{
    float xlen = xmax - xmin;
    float ylen = ymax - ymin;
    float zlen = zmax - zmin;
    float val = min(xlen,min(ylen,zlen)) * 0.01;

    // List of origins and extents for x (0-7), y (8-15), z (16-23) axes
    // Original full matrix of grid lines, later revised below
//    ivec3 gridVert[2][24] = {{ivec3(       0,        0,        0),
//                              ivec3(       0,        0,     zlen),
//                              ivec3(       0, ylen-val,        0),
//                              ivec3(       0, ylen-val,     zlen),
//                              ivec3(       0,        0,        0),
//                              ivec3(       0,     ylen,        0),
//                              ivec3(       0,        0, zlen-val),
//                              ivec3(       0,     ylen, zlen-val),

//                              ivec3(       0,        0,        0),
//                              ivec3(       0,        0,     zlen),
//                              ivec3(xlen-val,        0,        0),
//                              ivec3(xlen-val,        0,     zlen),
//                              ivec3(       0,        0,        0),
//                              ivec3(    xlen,        0,        0),
//                              ivec3(       0,        0, zlen-val),
//                              ivec3(    xlen,        0, zlen-val),

//                              ivec3(       0,        0,        0),
//                              ivec3(       0,     ylen,        0),
//                              ivec3(xlen-val,        0,        0),
//                              ivec3(xlen-val,     ylen,        0),
//                              ivec3(       0,        0,        0),
//                              ivec3(    xlen,        0,        0),
//                              ivec3(       0, ylen-val,        0),
//                              ivec3(    xlen, ylen-val,        0)
//                             },
//                             {ivec3( xlen,  val,    0),
//                              ivec3( xlen,  val, zlen),
//                              ivec3( xlen, ylen,    0),
//                              ivec3( xlen, ylen, zlen),
//                              ivec3( xlen,    0,  val),
//                              ivec3( xlen, ylen,  val),
//                              ivec3( xlen,    0, zlen),
//                              ivec3( xlen, ylen, zlen),

//                              ivec3(  val, ylen,    0),
//                              ivec3(  val, ylen, zlen),
//                              ivec3( xlen, ylen,    0),
//                              ivec3( xlen, ylen, zlen),
//                              ivec3(    0, ylen,  val),
//                              ivec3( xlen, ylen,  val),
//                              ivec3(    0, ylen, zlen),
//                              ivec3( xlen, ylen, zlen),

//                              ivec3(  val,    0, zlen),
//                              ivec3(  val, ylen, zlen),
//                              ivec3( xlen,    0, zlen),
//                              ivec3( xlen, ylen, zlen),
//                              ivec3(    0,  val, zlen),
//                              ivec3( xlen,  val, zlen),
//                              ivec3(    0, ylen, zlen),
//                              ivec3( xlen, ylen, zlen)
//                             }};

//    vec3 origin = vec3(gridVert[0][gl_InstanceID]);
//    vec3 extent = vec3(gridVert[1][gl_InstanceID]);

    // Attempt at simplification of above table with worst complexity of 8 if statements
    vec3 origin = vec3(0);
    vec3 extent = vec3(xlen,ylen,zlen);
    int instMod8 = gl_InstanceID % 8;
    if (gl_InstanceID < 8) {
        if (instMod8 == 0)      { extent.y =      val; extent.z =        0; }
        else if (instMod8 == 1) { origin.z =     zlen; extent.y =      val; }
        else if (instMod8 == 2) { origin.y = ylen-val; extent.z =        0; }
        else if (instMod8 == 3) { origin.y = ylen-val; origin.z =     zlen; }
        else if (instMod8 == 4) { extent.y =        0; extent.z =      val; }
        else if (instMod8 == 5) { origin.y =     ylen; extent.z =      val; }
        else if (instMod8 == 6) { origin.z = zlen-val; extent.y =        0; }
        else if (instMod8 == 7) { origin.y =     ylen; origin.z = zlen-val; }
    } else if (gl_InstanceID < 16) {
        if (instMod8 == 0)      { extent.x =      val; extent.z =        0; }
        else if (instMod8 == 1) { origin.z =     zlen; extent.x =      val; }
        else if (instMod8 == 2) { origin.x = xlen-val; extent.z =        0; }
        else if (instMod8 == 3) { origin.x = xlen-val; origin.z =     zlen; }
        else if (instMod8 == 4) { extent.x =        0; extent.z =      val; }
        else if (instMod8 == 5) { origin.x =     xlen; extent.z =      val; }
        else if (instMod8 == 6) { origin.z = zlen-val; extent.x =        0; }
        else if (instMod8 == 7) { origin.x =     xlen; origin.z = zlen-val; }
    } else {
        if (instMod8 == 0)      { extent.x =      val; extent.y =        0; }
        else if (instMod8 == 1) { origin.y =     ylen; extent.x =      val; }
        else if (instMod8 == 2) { origin.x = xlen-val; extent.y =        0; }
        else if (instMod8 == 3) { origin.x = xlen-val; origin.y =     ylen; }
        else if (instMod8 == 4) { extent.x =        0; extent.y =      val; }
        else if (instMod8 == 5) { origin.x =     ylen; extent.y =      val; }
        else if (instMod8 == 6) { origin.y = ylen-val; extent.x =        0; }
        else if (instMod8 == 7) { origin.x =     xlen; origin.y = ylen-val; }
    }

    // Setting the vertex for each point on the quad
    vec3 position[4] = {vec3(0),vec3(0),vec3(0),vec3(0)};
    if (origin.x == extent.x) {
        position[0] = vec3(origin.x,origin.y,origin.z);
        position[1] = vec3(origin.x,origin.y,extent.z);
        position[2] = vec3(origin.x,extent.y,origin.z);
        position[3] = vec3(origin.x,extent.y,extent.z);
    } else if (origin.y == extent.y) {
        position[0] = vec3(origin.x,origin.y,origin.z);
        position[1] = vec3(origin.x,origin.y,extent.z);
        position[2] = vec3(extent.x,origin.y,origin.z);
        position[3] = vec3(extent.x,origin.y,extent.z);
    } else {
        position[0] = vec3(origin.x,origin.y,origin.z);
        position[1] = vec3(origin.x,extent.y,origin.z);
        position[2] = vec3(extent.x,origin.y,origin.z);
        position[3] = vec3(extent.x,extent.y,origin.z);
    }

    // Add slight z offset due to qt origin glitch (TODO: find issue)
    position[gl_VertexID].z = position[gl_VertexID].z + 1.0;

    // Reproject into projection matrix
    gl_Position = mvp * vec4(position[gl_VertexID],1.0);
}
