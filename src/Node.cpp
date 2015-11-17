#include "Node.h"

Node::Node(HeightMap *heightMap, float nodeSize, int lodDepth, float x, float z) {
    xPos = x;
    zPos = z;
    size = nodeSize;

    if (lodDepth == 1) {
        topLeft = nullptr;
        topRight = nullptr;
        bottomLeft = nullptr;
        bottomRight = nullptr;
        maxHeight = heightMap->maxValArea(x, z, nodeSize, nodeSize);
        minHeight = heightMap->minValArea(x, z, nodeSize, nodeSize);
    minHeight;
    } else {
        float halfSize = nodeSize/2;
        topLeft = new Node(heightMap, halfSize, lodDepth-1, x, z);
        topRight = new Node(heightMap, halfSize, lodDepth-1, x+halfSize, z);
        bottomLeft = new Node(heightMap, halfSize, lodDepth-1, x, z+halfSize);
        bottomRight = new Node(heightMap, halfSize, lodDepth-1, x+halfSize, z+halfSize);

        maxHeight = std::max( std::max(topLeft->maxHeight,topRight->maxHeight),
                              std::max(bottomLeft->maxHeight,bottomRight->maxHeight));

        minHeight = std::min( std::min(topLeft->minHeight,topRight->minHeight),
                              std::min(bottomLeft->minHeight,bottomRight->minHeight));
    }
}

Node::~Node() {
    delete topLeft;
    delete topRight;
    delete bottomLeft;
    delete bottomRight;
}

bool Node::LODSelect( std::vector<int> &ranges, int lodLevel, Camera *camera, std::stack<Node*> &drawStack) {

    if ( !inSphere( ranges[lodLevel], camera->getPosition() ) ) {
        return false;
    }

    if ( !inFrustum( camera ) ) {
        return true;
    }

    if( lodLevel == 0 ) {
        fullResolution = true;
        drawStack.push(this);

        return true;
    } else {
        if( !inSphere( ranges[lodLevel-1], camera->getPosition() ) ) {
            fullResolution = true;
            drawStack.push(this);
        } else {
            Node *child;
            child = topLeft;
            if ( !child->LODSelect( ranges, lodLevel-1, camera, drawStack) ) {
                child->fullResolution = false;
                drawStack.push(child);
            }

            child = topRight;
            if ( !child->LODSelect( ranges, lodLevel-1, camera, drawStack) ) {
                child->fullResolution = false;
                drawStack.push(child);
            }

            child = bottomLeft;
            if ( !child->LODSelect( ranges, lodLevel-1, camera, drawStack) ) {
                child->fullResolution = false;
                drawStack.push(child);
            }

            child = bottomRight;
            if ( !child->LODSelect( ranges, lodLevel-1, camera, drawStack) ) {
                child->fullResolution = false;
                drawStack.push(child);
            }
        }
        return true;
    }

}

bool Node::inSphere(float radius, glm::vec3 position) {
    float r2 = radius*radius;
    glm::vec3 c1 = glm::vec3(xPos, minHeight, zPos);
    glm::vec3 c2 = glm::vec3(xPos+size, maxHeight, zPos+size);
    glm::vec3 distV;

    if (position.x < c1.x) distV.x = (position.x - c1.x);
    else if (position.x > c2.x) distV.x = (position.x - c2.x);
    if (position.y < c1.y) distV.y = (position.y - c1.y);
    else if (position.y > c2.y) distV.y = (position.y - c2.y);
    if (position.z < c1.z) distV.z = (position.z - c1.z);
    else if (position.z > c2.z) distV.z = (position.z - c2.z);

    float dist2 = glm::dot(distV,distV);

    return dist2 <= r2;
}

bool Node::inFrustum(Camera *camera) {
    //TODO
    return true;
}
