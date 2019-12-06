#pragma once
#include<glad/glad.h>
#include <GLFW/glfw3.h>

void Chapter_1_draw(GLFWwindow* window);
void Chapter_2_draw(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);