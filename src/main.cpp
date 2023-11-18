#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

const int WIDTH = 800, HEIGHT = 600;

int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		printf("GLFW failed to initialise!");
		glfwTerminate();
		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version: 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Core Profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Hello World", NULL, NULL);

	if (!mainWindow)
	{
		printf("GLFW Window failed to be created!");
		glfwTerminate();
		return 1;
	}

	// Get Buffer size information -> the viewport
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Sync refresh rate to our vsync
	glfwSwapInterval(5);

	// Allow modern extension features
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (glewInit() != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);
	
	std::cout << glGetString(GL_VERSION) << std::endl;
	{
		// Coords of the three points of the triangle
		float positions[] =
		{
			-0.5f, -0.5f, // 0
			 0.5f, -0.5f, // 1
			 0.5f,  0.5f, // 2
			-0.5f,  0.5f, // 3
		};

		// Create an index buffer to remove duplicate vertices
		unsigned int indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};

		VertexArray va;
		VertexBuffer vb(positions, 4 * 2 * sizeof(float));

		VertexBufferLayout layout;
		layout.Push<float>(2);
		va.AddBuffer(vb, layout);

		IndexBuffer ib(indices, 6);

		// Use the shaders
		Shader shader("res/shaders/Basics.shader");
		shader.Bind();
		shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		// Unbind everything
		va.Unbind();
		vb.Unbind();
		ib.Unbind();
		shader.Unbind();

		Renderer renderer;

		float r = 0.0f;
		float increment = 0.05f;

		// Main loop until user closes the window
		while (!glfwWindowShouldClose(mainWindow))
		{
			renderer.Clear();

			shader.Bind();
			shader.SetUniform4f("u_Color", r, r - 0.25f, r + 0.25f, 1.0f);

			renderer.Draw(va, ib, shader);

			// Draw using indices instead of array of positions
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

			// Called per draw
			if (r > 1.0f || r < 0.0f)
			{
				increment *= -1;
			}

			r += increment;


			glfwSwapBuffers(mainWindow);

			// Get and handle user input events
			glfwPollEvents();
		}
	} // scope cleans up Vertex Buffer and Index buffer before context is terminated, or we get infinite loop

	glfwTerminate();
	return 0;
}