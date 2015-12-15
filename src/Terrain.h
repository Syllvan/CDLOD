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
    void render(Camera *camera);
private:
    HeightMap *heightMap;
    std::vector< std::vector<Node*> > grid;
    int lodDepth;
    float leafNodeSize;
    std::vector<int> ranges;

    Shader vertexDebug;
    Shader vertexShader;
    Shader fragmentShader;
    ShaderProgram shaderProgram;
    ShaderProgram debugShader;

    ShaderProgram terrainShader;
    Shader vertexTerrain;
    Shader fragmentTerrain;

    FlatMesh fullResMesh;
    FlatMesh halfResMesh;
};

#endif	/* TERRAIN_H */

