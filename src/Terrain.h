#ifndef TERRAIN_H
#define	TERRAIN_H

#include <vector>
#include <math.h>

#include "graphic/Shader.hpp"

#include "Node.h"
#include "HeightMap.h"
#include "Camera.h"
#include "FlatMesh.h"

class Terrain {
public:
    Terrain(HeightMap *h);
    virtual ~Terrain();
    void debugRender(Camera *camera);
    void areaRender(Camera *camera);
    void render(Camera *camera);
private:
    HeightMap *heightMap;
    std::vector< std::vector<Node*> > grid;
    int lodDepth;
    float leafNodeSize;
    std::vector<int> ranges;

    Shader vertexDebug;
    Shader fragmentDebug;
    ShaderProgram shaderDebug;

    Shader vertexTerrain;
    Shader fragmentTerrain;
    ShaderProgram shaderTerrain;

    Shader vertexArea;
    Shader fragmentArea;
    ShaderProgram shaderArea;

    FlatMesh fullResMesh;
    FlatMesh halfResMesh;
    FlatMesh simpleMesh;
};

#endif	/* TERRAIN_H */

