#include <cstdio>

// GLAD
#include "../glad/glad.h"

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

// Other includes
#include "shader.h"
#include "camera.h"

#define NUM 100000 // the maximal number of vertices/faces allowed in the obj file

GLfloat* readObjFile(char* filename, int &numFaces){
    // read file to RAM
    FILE *file = fopen(filename, "r");
    if( file == NULL ){
        printf("Failed to open the file !n");
        return NULL;
    }

    int numVertices = 0;
    numFaces = 0;
    GLfloat x, y, z;
    int a, b, c;

    GLfloat* vertices = new GLfloat[NUM];
    GLfloat* faces = new GLfloat[NUM];

    char lineheader[10]; // assume line header length < 10
    while (fscanf(file, "%s", lineheader) != EOF){
        if (lineheader[0] == 'v'){ // a vertice line
            fscanf(file, "%f %f %fn", &x, &y, &z);
            vertices[numVertices * 3] = x;
            vertices[numVertices * 3 + 1] = y;
            vertices[numVertices * 3 + 2] = z;
            numVertices ++;
        }
        else
            if (lineheader[0] == 'f'){ // a face line
                fscanf(file, "%d %d %dn", &a, &b, &c);
                faces[numFaces * 3] = a;
                faces[numFaces * 3 + 1] = b;
                faces[numFaces * 3 + 2] = c;
                numFaces ++;
            }
            else{
                char buf[128] = ""; // assuming a single line is within 128 chars
                fgets(buf, 128, file); // this line is useless, just skip (read it and skip)
            }
    }

    // rearrange vertices into verticesReordered, according to faces
    GLfloat* verticesReordered = new GLfloat[numFaces * 15];
    for (int i = 0; i < numFaces; i++){
        int pt1 = faces[i * 3] - 1;
        int pt2 = faces[i * 3 + 1] - 1;
        int pt3 = faces[i * 3 + 2] - 1; // index of the 1st point, index in file starts from 1 but index in array should start from 0!
        
        verticesReordered[i * 15] = vertices[pt1 * 3]; // x
        verticesReordered[i * 15 + 1] = vertices[pt1 * 3 + 1]; // y
        verticesReordered[i * 15 + 2] = vertices[pt1 * 3 + 2]; // z
        verticesReordered[i * 15 + 3] = 0.0f; // texture coord
        verticesReordered[i * 15 + 4] = 1.0f; //

        verticesReordered[i * 15 + 5] = vertices[pt2 * 3]; // x
        verticesReordered[i * 15 + 6] = vertices[pt2 * 3 + 1]; // y
        verticesReordered[i * 15 + 7] = vertices[pt2 * 3 + 2]; // z
        verticesReordered[i * 15 + 8] = 0.0f; // texture coord
        verticesReordered[i * 15 + 9] = 1.0f; //

        verticesReordered[i * 15 + 10] = vertices[pt3 * 3]; // x
        verticesReordered[i * 15 + 11] = vertices[pt3 * 3 + 1]; // y
        verticesReordered[i * 15 + 12] = vertices[pt3 * 3 + 2]; // z
        verticesReordered[i * 15 + 13] = 0.0f; // texture coord
        verticesReordered[i * 15 + 14] = 1.0f; //

        // printf("%d %d %d\n", pt1, pt2, pt3);
        printf("%f %f %f %f %f\n", verticesReordered[i * 15], verticesReordered[i * 15 + 1], verticesReordered[i * 15 + 2], verticesReordered[i * 15 + 3], verticesReordered[i * 15 + 4]);
        printf("%f %f %f %f %f\n", verticesReordered[i * 15 + 5], verticesReordered[i * 15 + 6], verticesReordered[i * 15 + 7], verticesReordered[i * 15 + 8], verticesReordered[i * 15 + 9]);
        printf("%f %f %f %f %f\n", verticesReordered[i * 15 + 10], verticesReordered[i * 15 + 11], verticesReordered[i * 15 + 12], verticesReordered[i * 15 + 13], verticesReordered[i * 15 + 14]);
    }
    //printf("%d\n", numFaces);

    return verticesReordered;
}