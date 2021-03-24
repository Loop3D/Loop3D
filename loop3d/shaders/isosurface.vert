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
uniform int numTetraPerIso;
uniform float isovalues[7];
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

out float isovalue;
out vec3 position;
out vec3 normal;
out float instanceID;
out float vsubcubeTetraID;
out vec3 debugColour;
out float difference;

struct Tetra
{
   vec4 val;
   vec3 geometryPoint[4];
};

vec3 interp(vec3 pt1, float val1, vec3 pt2, float val2, float target)
{
    vec3 res = pt1 + ((target-val1)/(val2-val1) * (pt2 - pt1));
    if (abs(val2-val1) < 0.001)
        return (pt1+pt2)/2.0;
    else
        return pt1 + ((target-val1)/(val2-val1) * (pt2 - pt1));
}

Tetra applyUpperBound(Tetra tetra)
{
    for (int i=0;i<4;i++)
        tetra.val[i] = min(tetra.val[i],tetra.geometryPoint[i].x + tetra.geometryPoint[i].y + tetra.geometryPoint[i].z);
    return tetra;
}

Tetra applyLowerBound(Tetra tetra)
{
    for (int i=0;i<4;i++)
        tetra.val[i] = max(tetra.val[i],tetra.geometryPoint[i].x + tetra.geometryPoint[i].y + tetra.geometryPoint[i].z - 10000.0);
    return tetra;
}

Tetra getTetra(int subcubeTetraID, ivec3 referenceIndex)
{
    ivec4 tetraReference[2][5] = {{ivec4(0,1,2,4),
                                   ivec4(3,1,2,7),
                                   ivec4(5,1,4,7),
                                   ivec4(6,2,4,7),
                                   ivec4(7,1,2,4)},
                                  {ivec4(1,0,3,5),
                                   ivec4(2,0,3,6),
                                   ivec4(4,0,5,6),
                                   ivec4(7,3,5,6),
                                   ivec4(0,6,5,3)}};
    ivec3 cubeOffsets[8] = {ivec3(0,0,0),
                            ivec3(1,0,0),
                            ivec3(0,1,0),
                            ivec3(1,1,0),
                            ivec3(0,0,1),
                            ivec3(1,0,1),
                            ivec3(0,1,1),
                            ivec3(1,1,1)};
    Tetra tetra;
    ivec3 indexToPoint[4];
    int orientation = int((referenceIndex.x + referenceIndex.y + referenceIndex.z) % 2 == 0);
    for (int i=0;i<4;i++) {
        indexToPoint[i] = referenceIndex + cubeOffsets[tetraReference[orientation][subcubeTetraID][i]];
        tetra.geometryPoint[i] = vec3(indexToPoint[i].x * xstepsize,
                                      indexToPoint[i].y * ystepsize,
                                      indexToPoint[i].z * zstepsize);
//        tetra.geometryPoint[i] = vec3(xmin + indexToPoint[i].x * xstepsize,
//                                      ymin + indexToPoint[i].y * ystepsize,
//                                      zmin + indexToPoint[i].z * zstepsize);
        tetra.val[i] = texelFetch(scalarField,indexToPoint[i].xyz,0).x;
    }
    return tetra;
}

void main() {
    instanceID = gl_InstanceID;
//    float isovalues[7] = {isovalue1, isovalue2,  isovalue3,  isovalue4,  isovalue5,  isovalue6, isovalue7};
    isovalue = isovalues[int(floor(gl_InstanceID / numTetraPerIso))];
//    isovalue = isovalue1;
    int subcubeTetraID = int(instanceID) % 5;
    vsubcubeTetraID = subcubeTetraID;
    int gridIndex = int(floor(instanceID / 5));
    ivec3 gridPos = ivec3(gridIndex % (xsize-1), int(floor(gridIndex/(xsize-1))) % (ysize-1), int(floor(gridIndex/(xsize-1)/(ysize-1))) % (zsize-1));
    if (gridPos.x >= xsize - 1 || gridPos.y >= ysize - 1 || gridPos.z >= zsize - 1) return;
    bool orientation = ((gridPos.x + gridPos.y + gridPos.z) % 2 == 0);
    Tetra tetra = getTetra(subcubeTetraID, gridPos);

//    tetra = applyUpperBound(tetra);
//    tetra = applyLowerBound(tetra);

    bool above0 = tetra.val[0] > isovalue;
    bool above1 = tetra.val[1] > isovalue;
    bool above2 = tetra.val[2] > isovalue;
    bool above3 = tetra.val[3] > isovalue;
    int sum = int(above0) + int(above1) + int(above2) + int(above3);
    if (sum == 0 || sum == 4) {
        normal = vec3(10.0,10.0,10.0);
        gl_Position = vec4(0.0f);
        return;
    }

    vec2 vertexList[6] = {vec2(0.0,0.0),
                          vec2(1.0,1.0),
                          vec2(2.0,3.0),
                          vec2(3.0,0.0),
                          vec2(4.0,0.0),
                          vec2(5.0,0.0)};
    vec2 texCoordList[6] = {vec2(0.0,0.0),
                            vec2(1.0,0.0),
                            vec2(0.0,1.0),
                            vec2(1.0,0.0),
                            vec2(0.0,1.0),
                            vec2(1.0,1.0)};
    vec2 vertex = vertexList[gl_VertexID];
    vec3 newVertex;
    if (sum == 1 || sum == 3) {
        if (vertex.x == 3 || vertex.x == 4 || vertex.x == 5) {
            normal = vec3(10.0,10.0,10.0);
            gl_Position = vec4(0.0f);
            return;
        }
        // Work out odd point out
        int odd = int(above0 == above1) * 2 + int(above0 == above2);
        // Work out line to interpolate (start and end)
        int start = int(vertex.y);
        int end = odd;
        if (end == start) end = 2;
        // Interp along line (start -> end)
        newVertex = interp(tetra.geometryPoint[start], tetra.val[start],
                           tetra.geometryPoint[  end], tetra.val[  end], isovalue);
        difference = abs(tetra.val[start] - tetra.val[end]);
        if (difference > 2000.0) {
            newVertex = interp(tetra.geometryPoint[start], 0.0,
                               tetra.geometryPoint[end],1.0,0.5);
        }

        // Add colour for debugging types of marching tetra polygons
        debugColour = vec3(0.2,0.4,0.4);


    } else if (sum == 2) {
        int option = int(above0 == above3) * 2 + int(above0 == above2);
        int start = int(vertex.x);
        ivec4 offsetReference[3] = {ivec4(3,2,0,1),
                                    ivec4(1,2,3,0),
                                    ivec4(1,3,0,2)};
        int pointIndex = int(vertex.x);
        ivec3 pointIndicies[3] = {ivec3(1,0,3), ivec3(2,0,3), ivec3(1,2,3)};
        if (vertex.x > 2) {
            pointIndex = pointIndicies[option][int(vertex.x)-3];
            start = pointIndex;
        }
        ivec4 ttt = offsetReference[option];
        int end = ttt[pointIndex];
        newVertex = interp(tetra.geometryPoint[start], tetra.val[start],
                           tetra.geometryPoint[  end], tetra.val[  end], isovalue);
        difference = abs(tetra.val[start] - tetra.val[end]);
        if (difference > 2000.0) {
            newVertex = interp(tetra.geometryPoint[start], 0.0,
                               tetra.geometryPoint[end],1.0,0.5);
        }

        // Add colour for debugging types of marching tetra polygons
        vec3 colourise[3] = {vec3(1.0,0.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0,0.0,1.0)};
        debugColour = colourise[option];
        debugColour = debugColour + vec3((isovalue-tetra.val[start])/(tetra.val[end]-tetra.val[start]));
    }

//    // Debug line to remove half the faces
//    if (!orientation) { gl_Position = vec4(0.0f); return; }

    // Calculate normals (easy for subcubes 0-3 as each vector is perpendicular along cardinal axes)
    if (subcubeTetraID != 4) {
        ivec3 order;
        ivec3 sign;
        if (subcubeTetraID == 0) order = ivec3(1,2,3);
        if (subcubeTetraID == 1) order = ivec3(2,1,3);
        if (subcubeTetraID == 2) order = ivec3(2,3,1);
        if (subcubeTetraID == 3) order = ivec3(3,2,1);
        if (orientation) {
            if (subcubeTetraID == 0) sign = ivec3(-1,1,1);
            if (subcubeTetraID == 1) sign = ivec3(1,-1,1);
            if (subcubeTetraID == 2) sign = ivec3(1,1,-1);
            if (subcubeTetraID == 3) sign = ivec3(-1,-1,-1);
        } else {
            sign = ivec3(1,1,1);
            if (subcubeTetraID == 0) sign = ivec3(-1,-1,-1);
            if (subcubeTetraID == 1) sign = ivec3(1,1,-1);
            if (subcubeTetraID == 2) sign = ivec3(1,-1,1);
            if (subcubeTetraID == 3) sign = ivec3(-1,1,1);
        }
        normal = normalize(vec3(sign.x * (tetra.val[order.x] - tetra.val[0]),
                                sign.y * (tetra.val[order.y] - tetra.val[0]),
                                sign.z * (tetra.val[order.z] - tetra.val[0])));
        normal *= ((orientation) ? 1.0 : -1.0 );
    } else {
        // Harder for a regular non-axis aligned tetrahedron
        if (!orientation) {
            normal = normalize(vec3(tetra.val[0]+tetra.val[1]-tetra.val[2]-tetra.val[3],
                                    tetra.val[0]-tetra.val[1]+tetra.val[2]-tetra.val[3],
                                    tetra.val[0]-tetra.val[1]-tetra.val[2]+tetra.val[3]));
        } else {
            normal = normalize(vec3(-tetra.val[0]-tetra.val[1]+tetra.val[2]+tetra.val[3],
                                    -tetra.val[0]+tetra.val[1]-tetra.val[2]+tetra.val[3],
                                    -tetra.val[0]+tetra.val[1]+tetra.val[2]-tetra.val[3]));
        }
    }
    if (!invertedView) newVertex.z = (zmax-zmin)-newVertex.z;

    // Add minor offset as isosurface 3d texture values are mid-cell values not vertex values
    newVertex = newVertex + vec3(xstepsize,ystepsize,0)*0.5;
    position = newVertex;
    gl_Position = mvp * vec4(newVertex.xyz,1.0);
}
