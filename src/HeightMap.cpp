#include "HeightMap.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
HeightMap::HeightMap(char* filepath) {
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int imageSize;   // = width*height*3

    FILE * file = fopen(filepath,"rb");
    if (!file) {
        perror("Image could not be opened\n");
    }

    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        perror("Not a correct BMP file\n");
    }

    if ( header[0]!='B' || header[1]!='M' ){
        perror("Not a correct BMP file\n");
    }

     // Read ints from the byte array
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    //Create one OpenGL texture
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

HeightMap::~HeightMap() {
}

int HeightMap::getWidth() {
    //TODO
    return width;
}
int HeightMap::getHeight() {
    //TODO
    return height;
}

float HeightMap::getVal(float x, float y) {
    int i = (int)x%width;
    int j = (int)y%height;
    unsigned char charval = data[(i + j*width)*3] + 1; //read red color channel
    float val = 20.0f*(charval/255.0f - 0.5f);
    return val;
}

float HeightMap::maxValArea(float x, float z, float w, float h) {
    float maxVal = -999999.0f;
    x *= 5.12;
    z *= 5.12;
    w *= 5.12;
    h *= 5.12;
    for(float i = x; i < x+w; i++)
    for(float j = z; j < z+h; j++){
        float newVal = getVal(i,j);
        if(newVal > maxVal) {
            maxVal = newVal;
        }
    }
    return maxVal;
}

float HeightMap::minValArea(float x, float z, float w, float h) {
    float minVal = 999999.0f;
    x *= 5.12;
    z *= 5.12;
    w *= 5.12;
    h *= 5.12;
    for(float i = x; i < x+w; i++)
    for(float j = z; j < z+h; j++){
        float newVal = getVal(i,j);
        if(newVal < minVal) {
            minVal = newVal;
        }
    }
    return minVal;
}

GLuint HeightMap::getTexture() {
    return textureID;
}
