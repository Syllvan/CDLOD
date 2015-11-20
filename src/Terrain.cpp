#include "Terrain.h"

Terrain::Terrain(HeightMap *h) {
    
    GLuint image = loadBMP_custom("./my_texture.bmp");


    heightMap = h;

    //should probably be defined in a settings class or be different?
    //see page 9 in the paper http://www.vertexasylum.com/downloads/cdlod/cdlod_latest.pdf
    leafNodeSize = 32.0f;
    lodDepth = 4;

    //construct level of detail ranges
    for (int i = 0; i < lodDepth; i++) {
        ranges.push_back(leafNodeSize*pow(2,i+1));
    }

    //construct grid of cdlod quadtrees
    float rootNodeSize = leafNodeSize*pow(2,lodDepth);

    int gridWidth = floor(heightMap->getWidth()/rootNodeSize);
    int gridHeight = floor(heightMap->getHeight()/rootNodeSize);

    grid.resize(gridWidth);
    for (int i = 0; i < gridWidth; i++) {
        grid[i].resize(gridHeight);
        for (int j = 0; j < gridHeight; j++) {
            float xPos = i*rootNodeSize;
            float zPos = j*rootNodeSize;
            grid[i][j] = new Node(heightMap, rootNodeSize, lodDepth, xPos, zPos);
        }
    }
}

Terrain::Terrain(const Terrain& orig) {
}

Terrain::~Terrain() {
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            delete grid[i][j];
        }
    }
}

void Terrain::render(glm::mat4 ViewMatrix, glm::vec3 CamPos) {
    //build renderlist
    std::stack<Node*> drawStack;
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            grid[i][j]->LODSelect( ranges, lodDepth-1, camera, drawStack);
        }
    }

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();

        if ( current->isFullResolution() ) {
            //use 32x32 mesh
        } else {
            //use 16x16 mesh
        }

        //get xPos, zPos and size of current, send to shader as uniforms
        //maxHeight minHeight only used for debug rendering
    }
}

