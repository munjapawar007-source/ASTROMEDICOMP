#ifndef TERRAIN_H
#define TERRAIN_H

#include <GL/glew.h> //this headr file must be include before gl/gl.h
#include <gl/GL.h>	 //inside this prototypes presents


#define NUM_ROWS 113
#define NUM_COLOUMS 113

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCORD,

};

struct MPvec3
{
    float x;
    float y;
    float z;
};

struct MPvec2
{
    float u;
    float v;
};

struct quad
{
    MPvec3 a;
    MPvec3 b;
    MPvec3 c;
    MPvec3 d;
};

struct imageTexcoord
{
    MPvec2 a;
    MPvec2 b;
    MPvec2 c;
    MPvec2 d;
};

extern GLuint vao;
extern GLuint vbo_position;
extern GLuint vbo_TexCoordRectangle;

void initializeTerrain(void);
void drawTerrain(void);
void updateTerrain(void);
void uninitializeTerrain(void);

#endif // TERRAIN_H
