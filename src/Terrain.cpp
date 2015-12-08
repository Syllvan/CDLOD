#include "Terrain.h"
#include "utils/glError.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#define SHADER_DIR "../shader/"
#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

Terrain::Terrain(HeightMap *h):
    vertexShader(SHADER_DIR"/basicShader.vert",GL_VERTEX_SHADER),
    fragmentShader(SHADER_DIR"/basicShader.frag",GL_FRAGMENT_SHADER),
    shaderProgram({vertexShader,fragmentShader}),
    fullResMesh(32,32),
    halfResMesh(16,16)
{
    heightMap = h;

    //should probably be defined in a settings class or be different?
    //see page 9 in the paper http://www.vertexasylum.com/downloads/cdlod/cdlod_latest.pdf
    leafNodeSize = 1.0f;
    lodDepth = 4;

    //construct level of detail ranges
    ranges.push_back(2);
    for (int i = 1; i < lodDepth; i++) {
        ranges.push_back(ranges[i-1]*3);
        std::cout << ranges[i-1] << std::endl;
    }

    //construct grid of cdlod quadtrees
    float rootNodeSize = leafNodeSize*pow(2,lodDepth-1);


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

    //opengl stuff
    fullResMesh.bind();
    shaderProgram.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);

    halfResMesh.bind();
    shaderProgram.setAttribute("position", 3, sizeof(glm::vec3), 0);
    glBindVertexArray(0);
}

Terrain::~Terrain() {
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            delete grid[i][j];
        }
    }
}

void drawMesh(FlatMesh *mesh) {
    mesh->bind();

    static int inc = 1;
    inc *= 2;
    glCheckError(__FILE__,__TRIANGLE__);
        glDrawElements(
             GL_LINES,      // mode
             mesh->getPointCount(),         // count
             GL_UNSIGNED_INT,   // type
             NULL               // element array buffer offset
         );
}

GLuint loadDDS(const char * imagepath){

    unsigned char header[124];

    FILE *fp; 
 
    /* try to open the file */ 
    fp = fopen(imagepath, "rb"); 
    if (fp == NULL){
        printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); 
        return 0;
    }
   
    /* verify the type of file */ 
    char filecode[4]; 
    fread(filecode, 1, 4, fp); 
    if (strncmp(filecode, "DDS ", 4) != 0) { 
        fclose(fp); 
        return 0; 
    }
    
    /* get the surface desc */ 
    fread(&header, 124, 1, fp); 

    unsigned int height      = *(unsigned int*)&(header[8 ]);
    unsigned int width       = *(unsigned int*)&(header[12]);
    unsigned int linearSize  = *(unsigned int*)&(header[16]);
    unsigned int mipMapCount = *(unsigned int*)&(header[24]);
    unsigned int fourCC      = *(unsigned int*)&(header[80]);

 
    unsigned char * buffer;
    unsigned int bufsize;
    /* how big is it going to be including all mipmaps? */ 
    bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize; 
    buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char)); 
    fread(buffer, 1, bufsize, fp); 
    /* close the file pointer */ 
    fclose(fp);

    unsigned int components  = (fourCC == FOURCC_DXT1) ? 3 : 4; 
    unsigned int format;
    switch(fourCC) 
    { 
    case FOURCC_DXT1: 
        format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT; 
        break; 
    case FOURCC_DXT3: 
        format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT; 
        break; 
    case FOURCC_DXT5: 
        format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT; 
        break; 
    default: 
        free(buffer); 
        return 0; 
    }

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);   
    
    unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16; 
    unsigned int offset = 0;

    /* load the mipmaps */ 
    for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) 
    { 
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize; 
        glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,  
            0, size, buffer + offset); 
     
        offset += size; 
        width  /= 2; 
        height /= 2; 

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;

    } 

    free(buffer); 

    return textureID;
}

void Terrain::render(Camera *camera) {

    // Use our compiled shader program
    shaderProgram.use();

    // Load texture
    GLuint Texture = loadDDS("rock1.DDS");
    // Use texture position 0 on GPU
    int texturePosition = 0;
    glActiveTexture(GL_TEXTURE0 + texturePosition);
    // Bind loaded texture to this texture position
    glBindTexture(GL_TEXTURE_2D, Texture);

    // Tell shader to sample from this texture position
    shaderProgram.setUniform("myTextureSampler", texturePosition);
    
    
    //build renderlist
    std::stack<Node*> drawStack;
    for (unsigned int i = 0; i < grid.size(); i++) {
        for (unsigned int j = 0; j < grid[0].size(); j++) {
            grid[i][j]->LODSelect( ranges, lodDepth-1, camera, drawStack);
        }
    }

    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();

    // clear
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0,0.0,0.0,0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //TODO! actually drawing stuff!
    while( !drawStack.empty() ) {
        Node* current = drawStack.top();
        drawStack.pop();

        float s = current->getSize();
        float l = current->isFullResolution();
        glm::vec4 color = glm::vec4(1.0, 0.0, 0.0, 1.0);
        if(s < 2.0) {
            color = glm::vec4(1.0);
            if(!l) color = glm::vec4(0.5,1.0,0.5,1.0);
        } else if(s < 4.0) {
            color = glm::vec4(0.0,1.0,0.0,1.0);
            if(!l) color = glm::vec4(0.0,1.0,1.0,1.0);
        } else if(s < 8.0) {
            color = glm::vec4(0.0,0.0,1.0,1.0);
            if(!l) color = glm::vec4(1.0,0.0,1.0,1.0);
        } else if(s < 16.0) {
            color = glm::vec4(1.0,1.0,0.0,1.0);
            if(!l) color = glm::vec4(1.0,0.5,0.0,1.0);
        }

        float scale = current->getSize();
        float range = current->getRange(); //used for interpolation between resolutions
        glm::vec3 cameraPos = camera->getPosition();

        glm::vec3 translation = glm::vec3(current->getXPos(),0.0,current->getZPos());
        shaderProgram.use();
        shaderProgram.setUniform("projection",projection);
        shaderProgram.setUniform("view",view);
        shaderProgram.setUniform("color",color);
        shaderProgram.setUniform("translation",translation);
        shaderProgram.setUniform("scale", scale);
        shaderProgram.setUniform("range", range);
        shaderProgram.setUniform("cameraPos", cameraPos);
        glCheckError(__FILE__,__LINE__);

        if ( current->isFullResolution() ) {
            drawMesh(&fullResMesh);
        } else {
            drawMesh(&halfResMesh);
        }

        glBindVertexArray(0);
        shaderProgram.unuse();

    }

}

