#include <stdio.h>
#include "terrain.h"

GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_TexCoordRectangle = 0;

void initializeTerrain(void)
{
    quad quadPositions[NUM_ROWS][NUM_COLOUMS];
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLOUMS; j++)
        {
            // quadPositions[i][j].a = {(float)i / NUM_ROWS, (float)(j + 1.0f) / NUM_COLOUMS, 0};
            // quadPositions[i][j].b = {(float)i / NUM_ROWS, (float)j / NUM_COLOUMS, 0};
            // quadPositions[i][j].c = {(float)(i + 1) / NUM_ROWS, (float)j / NUM_COLOUMS, 0};
            // quadPositions[i][j].d = {(float)(i + 1.0f) / NUM_ROWS, (float)(j + 1.0f) / NUM_COLOUMS, 0};
			float x0 = ((float)i / NUM_ROWS) - 0.5f;
            float y0 = ((float)j / NUM_COLOUMS) - 0.5f;
            float x1 = (((float)i + 1.0f) / NUM_ROWS) - 0.5f;
            float y1 = (((float)j + 1.0f) / NUM_COLOUMS) - 0.5f;

            quadPositions[i][j].a = {x0, y1, 0.0f};
            quadPositions[i][j].b = {x0, y0, 0.0f};
            quadPositions[i][j].c = {x1, y0, 0.0f};
            quadPositions[i][j].d = {x1, y1, 0.0f};
        }
    }

    imageTexcoord imageUV[NUM_ROWS][NUM_COLOUMS];
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLOUMS; j++)
        {
            imageUV[i][j].a = {(float)i / NUM_ROWS, (float)(j + 1.0f) / NUM_COLOUMS};
            imageUV[i][j].b = {(float)i / NUM_ROWS, (float)j / NUM_COLOUMS};
            imageUV[i][j].c = {(float)(i + 1) / NUM_ROWS, (float)j / NUM_COLOUMS};
            imageUV[i][j].d = {(float)(i + 1.0f) / NUM_ROWS, (float)(j + 1.0f) / NUM_COLOUMS};
        }
    }

    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Position
	glGenBuffers(1, &vbo_position); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadPositions), quadPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// texcoord
	glGenBuffers(1, &vbo_TexCoordRectangle); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_TexCoordRectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(imageUV), imageUV, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void drawTerrain(void)
{
    //code
	// bind with vao
	glBindVertexArray(vao);
	// draw vertex array
	// glPointSize(15.0f);
	glDrawArrays(GL_QUADS, 0, 10000 * 4);
	
}

void updateTerrain(void)
{
}

void uninitializeTerrain(void)
{
    // free vbo postion
	if (vbo_TexCoordRectangle)
	{
		glDeleteBuffers(1, &vbo_TexCoordRectangle);
		vbo_TexCoordRectangle = 0;
	}

	// free vbo postion
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	// free vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

}