#include <iostream>
#include <stdio.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const GLint WIDTH = 800, HEIGHT = 600;

unsigned int VAO, VBO, shader; // Vertex Array Object, Vertex Buffer Object

// Vertex Shader - called per vertex. location is the index of the attribute we want to use
static const char* vShader = "									\n\
#version 330 core												\n\
																\n\
layout (location = 0) in vec4 position;							\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = position;										\n\
}";

// Fragment Shader - called per pixel that needs to be rasterized
static const char* fShader = "									\n\
#version 330 core												\n\
																\n\
layout (location = 0) out vec4 color;							\n\
																\n\
void main()														\n\
{																\n\
	color = vec4(1.0, 0.0, 0.0, 1.0);							\n\
}";

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, NULL);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// Create char array on stack instead of heap
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	// Delete shaders when it detaches from the program
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void CreateTriangle()
{
	// Coords of the three points of the triangle
	float positions[] = {
		-0.5f, -0.5f, 
		 0.5f, -0.5f,
		 0.0f,  0.5f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW); // Static draw -> Not going to change triangle values

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int shader = CreateShader(vShader, fShader);
	glUseProgram(shader);
}

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

	CreateTriangle();

	// Main loop until user closes the window
	while (!glfwWindowShouldClose(mainWindow))
	{

		// Clear window
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(mainWindow);

		// Get and handle user input events
		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}