#pragma once
#include "utils.h"

void Chapter_1_draw(GLFWwindow* window);
void Chapter_2_draw(GLFWwindow* window);
void StencilDraw(GLFWwindow* window);
void BlendDraw(GLFWwindow* window);
void CullFaceDraw(GLFWwindow* window);
void FramebufferDraw(GLFWwindow* window);
void CubemapDraw(GLFWwindow* window);
void CubemapReflectDraw(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void UniformBufferDraw(GLFWwindow* window);