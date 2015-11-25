#ifndef NODE_H
#define	NODE_H

#include <stack>
#include <vector>

#include "glm/glm.hpp"

#include "Camera.h"
#include "HeightMap.h"

class Node {
public:
    Node(HeightMap *heightMap, float nodeSize, int lodDepth, float x, float z);
    virtual ~Node();

    bool LODSelect( std::vector<int> &ranges, int lodLevel, Camera *camera, std::stack<Node*> &drawStack);
    bool isFullResolution() { return fullResolution; };

    float getMaxHeight() { return maxHeight; };
    float getMinHeight() { return minHeight; };
    float getXPos() { return xPos; };
    float getZPos() { return zPos; };
    float getSize() { return size; };
    float getRange() { return currentRange; };
private:
    bool inSphere(float radius, glm::vec3 position);
    bool inFrustum(Camera *camera);

    Node *topLeft;
    Node *topRight;
    Node *bottomLeft;
    Node *bottomRight;

    float maxHeight;
    float minHeight;
    float xPos;
    float zPos;
    float size;
    bool fullResolution;
    float currentRange;
};

#endif	/* NODE_H */

