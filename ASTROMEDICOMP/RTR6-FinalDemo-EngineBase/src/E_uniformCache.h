#pragma once
#include <GL/glew.h>

// Returns cached glGetUniformLocation(prog, name).
// First call per (prog, name) pair queries the driver; all subsequent calls
// return the cached GLint with no driver overhead.
GLint E_getUniformLocation(GLuint prog, const char* name);
