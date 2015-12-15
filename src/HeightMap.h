#ifndef HEIGHTMAP_H
#define	HEIGHTMAP_H

#include <GL/glew.h>
#include <string>

class HeightMap {
public:
    HeightMap(char* filepath);
    virtual ~HeightMap();

    int getWidth();
    int getHeight();
    float maxValArea(float x, float z, float width, float Height);
    float minValArea(float x, float z, float width, float height);
    float getVal(float x, float y);
    GLuint getTexture();

private:
    unsigned char * data;
    int width;
    int height;
    GLuint textureID;

};

#endif	/* HEIGHTMAP_H */

