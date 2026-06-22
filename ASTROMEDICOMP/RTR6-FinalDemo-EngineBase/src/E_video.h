#pragma once
#include <GL/glew.h>

typedef struct E_video E_video;

// Load a video file. Returns NULL on failure.
E_video* E_video_load   (const char* path, int loop);

// Call once per frame. Decodes and uploads the next frame when its time has come.
void     E_video_update (E_video* v);

// Returns the GL texture handle  updated in-place each frame.
GLuint   E_video_getTexture(const E_video* v);

// Returns 1 when the video has reached the end (only meaningful when loop=0).
int      E_video_isFinished(const E_video* v);

// Seek back to the beginning.
void     E_video_rewind (E_video* v);

void     E_video_destroy(E_video* v);
