#ifndef HEIGHTMAP_H
#define	HEIGHTMAP_H

#include <string>

class HeightMap {
public:
    HeightMap();
    HeightMap(std::string fileName);
    virtual ~HeightMap();

    int getWidth() {
        //TODO
        return 512;
    }
    int getHeight() {
        //TODO
        return 512;
    }

    float maxValArea(float x, float z, float width, float Height) {
        //TODO
        return 1;
    }

    float minValArea(float x, float z, float width, float height) {
        //TODO
        return -1;
    }

private:

};

#endif	/* HEIGHTMAP_H */

