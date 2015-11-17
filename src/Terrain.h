#ifndef TERRAIN_H
#define	TERRAIN_H

#include <vector>
#include <math.h>

#include "Node.h"
#include "HeightMap.h"
#include "Camera.h"

class Terrain {
public:
    Terrain(HeightMap *h);
    Terrain(const Terrain& orig);
    virtual ~Terrain();

    void render(Camera *camera);
private:
    HeightMap *heightMap;
    std::vector< std::vector<Node*> > grid;
    int lodDepth;
    float leafNodeSize;
    std::vector<int> ranges;
};

#endif	/* TERRAIN_H */

