#ifndef HEIGHTMAP_H
#define	HEIGHTMAP_H

#include <string>

class HeightMap {
public:
    HeightMap();
    HeightMap(std::string fileName);
    HeightMap(const HeightMap& orig);
    virtual ~HeightMap();

    int getWidth() {
        //TODO
        return 0;
    }
    int getHeight() {
        //TODO
        return 0;
    }

    float maxValArea(float x, float z, float width, float Height) {
        //TODO
        return 0;
    }

    float minValArea(float x, float z, float width, float height) {
        //TODO
        return 0;
    }

private:

};

#endif	/* HEIGHTMAP_H */

