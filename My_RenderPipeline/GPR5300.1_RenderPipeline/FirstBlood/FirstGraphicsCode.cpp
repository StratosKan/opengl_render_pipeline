#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// Function declaration. Definition at the end of the code.
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// initial window width and height
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// initialize and configure glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "First OpenGL Window", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	// assign callback function for window resizing
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// initialize GLAD (loads OpenGL function pointers)
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// MAIN RENDERING LOOP
	while (!glfwWindowShouldClose(window))
	{
		// check input (keyboard, mouse, etc.)
		processInput(window);

		// render some color
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clear allocated resources and terminate
	glfwTerminate();
	return 0;
}

// process input: use GLFW to query if keys are pressed/released in this frame and react accordingly
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// callback function called when window size changes (by OS or user resize)
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// dedicated OpenGL function
	glViewport(0, 0, width, height);
}