#include<iostream>
#include "Camera.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "DrawFunc.h"


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow * window = glfwCreateWindow(800, 600, "learnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Faled to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Faied to intialize GLAD" << std::endl;
		return -1;
	}
	Chapter_2_draw(window);
	// enable depth test

	return 0;
}
