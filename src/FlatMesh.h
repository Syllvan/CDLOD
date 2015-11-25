#ifndef FLATMESH_H
#define FLATMESH_H

#include <glm/glm.hpp>
#include <GL/glew.h>
#include "graphic/Shader.hpp"

class FlatMesh
{
    public:
        FlatMesh(int width, int height);
        virtual ~FlatMesh();

        int getPointCount();
        void bind();
    protected:
    private:
        int pointCount;

        GLuint vao;
        GLuint vbo;
        GLuint ibo;
};

#endif // FLATMESH_H
