#ifndef LAB_GL_UTILS_H
#define LAB_GL_UTILS_H

/**
 * This file contains utility functions to be used for the labs in computer graphics at chalmers,
 * they are not covered by any particular license...
 */

#include "vecmath.h"

#include <string>
#include <cassert>

// Helper function used to create a shader object from text in a file
char *textFileRead(const char *fn);


// Helper function used to get log info (such as errors) about a shader object or shader program
std::string GetShaderInfoLog(GLuint obj);


Mtx4f perspectiveMatrix(float fov, float aspectRatio, float n, float f); /* field of view, aspect ratio, near clipping plane, far clipping plane */


Mtx4f lookAt(const Vec3f &eyePosition, const Vec3f &lookAt, const Vec3f &desiredUp);


GLuint loadCubeMap(const char* facePosX, const char* faceNegX, const char* facePosY, const char* faceNegY, const char* facePosZ, const char* faceNegZ);


/**
 * This macro checks for gl errors using glGetError, it is useful to sprinkle it around the code base, especially
 * when unsure about the correct usage, for example after each call to open gl.
 * When the debugger is atached it will cause a break on the offending line, and also print out the file 
 * and line in a MSVC compatible format on the debug output and console.
 *
 * Note: the macro _cannot_ be used between glBegin/glEnd brackets, as stated by the openGL standard.
 * Note2: be aware that the macro will report any previous errors, since the last call to glGetError.
 *
 * example usage: glClear(GL_COLOR_BUFFER_BIT); CHECK_GL_ERROR(); // this will check for errors in this (and any previous statements)
 */

#if !defined(_WIN32)
#	define __debugbreak() assert(false)
#endif

#define CHECK_GL_ERROR() { checkGLError(__FILE__, __LINE__) && (__debugbreak(), 1); }

/**
 * Internal function used by macro CHECK_GL_ERROR, use that instead.
 */
bool checkGLError(const char *file, int line);

/** 
 * Error reporting function
 */
void fatal_error( std::string errorString, std::string title = std::string() );



GLuint loadShaderProgram(const std::string &vertexShader, const std::string &fragmentShader);
void linkShaderProgram(GLuint shaderProgram);

/**
 * Creates a GL buffer and uploads the given data to it.
 * returns the handle of the GL buffer.
 */
GLuint createAddAttribBuffer(GLuint vertexArrayObject, const void *data, const size_t dataSize, GLuint attributeIndex, GLsizei attributeSize, GLenum type, GLenum bufferUsage = GL_STATIC_DRAW);


void drawTorus(float rout, float rin, int numc, int numt);
void drawQuad(); 

#endif // LAB_GL_UTILS_H
