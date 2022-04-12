#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <string>
#include <fstream>
#include <iostream>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

# define M_PI           3.14159265358979323846
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
	while (GLenum error = glGetError()) {
		std::cout << "[OpenGL Error] (" << error << ")" << function << "\n" << file << ": " << line << std::endl;
		return false;
	}

	return true;
}

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShaders(std::string filepath) {
	enum class ShaderType {
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::ifstream src(filepath);
	std::string line;
	ShaderType type = ShaderType::NONE;
	std::stringstream ss[2];
	while (getline(src, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, std::string source) {
	const char* src = source.c_str();
	GLCall(unsigned int id = glCreateShader(type));
	GLCall(glShaderSource(id, 1, &src, 0));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		std::cout << "error" << std::endl;
	}

	return id;
}

static unsigned int CreateProgram(std::string vertexSrc, std::string fragmentSrc) {
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSrc);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	return program;
}

int main() {

	if (!glfwInit())
		return -1;

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello", NULL, NULL);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return -1;
	}

	float pts[] = {

		// FRONT
		1.0f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		// LEFT
		-1.0f,1.0f,0.0f,
		-1.0f,1.0f,2.0f,
		-1.0f,-1.0f,0.0f,
		-1.0f,-1.0f,2.0f,
		// BACK
		1.0f,1.0f,2.0f,
		-1.0f,1.0f,2.0f,
		-1.0f,-1.0f,2.0f,
		1.0f,-1.0f,2.0f,
		// RIGHT
		1.0f,1.0f,0.0f,
		1.0f,1.0f,2.0f,
		1.0f,-1.0f,0.0f,
		1.0f,-1.0f,2.0f,
		// TOP
		1.0f,1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		-1.0f,1.0f,2.0f,
		1.0f,1.0f,2.0f,
		// BOTTOM
		1.0f,-1.0f,1.0f,
		-1.0f,-1.0f,1.0f,
		-1.0f,-1.0f,0.5f,
		1.0f,-1.0f,0.5f
	};

	unsigned int ind[] = {
		// FRONT
		0,1,2,
		2,3,0,
		//LEFT
		4,5,6,
		6,7,5,
		// BACK
		8,9,10,
		10,11,8,
		// RIGHT
		12,13,14,
		14,15,13,
		// TOP
		16,17,18,
		18,19,16,
		// BOTTOM
		20,21,22,
		22,23,20
	};

	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	ShaderProgramSource shaders = ParseShaders("lib/shaders/Default.shader");
	unsigned int program = CreateProgram(shaders.VertexSource, shaders.FragmentSource);
	glUseProgram(program);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ind), ind, GL_STATIC_DRAW);

	// Part 2 & 3 - Get any uniform variable locations that you'll need.

	float n = 1.0f;
	float f = 20.0f;
	float theta = 45.0f * M_PI / 180.0f;
	float aspect = (float)1280 / 720;
	float d = 1 / tan(theta / 2);

	/*glm::mat4 projMatrix = glm::ortho(-5.0f*aspect, 5.0f*aspect, -5.0f, 5.0f, -10.0f, 100.0f);*/
	// Part 1 - Instead of using ortho, construct your own perspective matrix.
	// Do not use glm::perspective.

	glm::mat4 projMatrix = glm::mat4((1 / (aspect * tan(theta / 2))), 0, 0, 0,
		0, (1 / tan(theta / 2)), 0, 0,
		0, 0, ((n + f) / (n - f)), ((2 * f * n) / (n - f)),
		0, 0, 1, 0);

	int projLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));



	glEnable(GL_DEPTH_TEST);

	float ang = 0;

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		ang += 0.1;
		glm::mat4 view = glm::mat4(1.0f);
		// note that we're translating the scene in the reverse direction of where we want to move
		view = glm::translate(view, glm::vec3(6.0f, -1.0f, -10.0));

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-8, 4, 0));
		model = glm::rotate(model, glm::radians(ang), glm::vec3(1.0f, 1.0f, 0.0f));

		int viewLoc = glGetUniformLocation(program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		int modelLoc = glGetUniformLocation(program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glEnableVertexAttribArray(0);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glUseProgram(0);

		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glUseProgram(0);
	glfwTerminate();

	return 0;
}