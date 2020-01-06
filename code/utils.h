#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

unsigned int loadTexture(char const* path, GLint wrappingMode = GL_REPEAT, bool grammaCorrection=false);
unsigned int loadCubemap(std::vector<std::string> faces);