#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include "shader.h"
#include "camera.h"
#include <GLFW/stb_image.h>
#include <GLFW/glm.hpp>
#include <GLFW/gtc/matrix_transform.hpp>
#include <GLFW/gtc/type_ptr.hpp>
#include "debug.h"

using namespace std;
using namespace glm;

static int width = 1400;
static int height = 1000;
static bool keys[1024];

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

Camera* camera = nullptr; // Глобальная переменная для хранения камеры

Debug debug;

int main() {
	const char* title = "3D world";
	// Инициализация GLFW
	if (!glfwInit()) {
		debug.error_debug("Failed to initialize GLFW");
		return -1;
	}

	// Установка параметров окна
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Создание окна
	GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (window == nullptr) {
		debug.error_debug("Failed to create GLFW window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); // Делаем контекст текущим

	debug.success_debug("Window was created");

	glewExperimental = GL_TRUE; // Включаем экспериментальные функции GLEW
	if (glewInit() != GLEW_OK) { // Проверяем инициализацию GLEW
		debug.error_debug("Failed to initialize GLEW");
		return -1;
	}

	// Получение размеров окна
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height); // Устанавливаем область отображения

	Shader shader("res/shaders/shaderv.vs", "res/shaders/shaderf.frag");

	int channelsi = 4;

	unsigned char* icon = stbi_load("res/textures/icon.png", &width, &height, &channelsi, 0); // Загружаем наш icon

	if (icon == nullptr) {
		debug.error_debug("Failed load icon image");
		return -1;
	}

	GLFWimage images[1];

	images[0].pixels = icon;
	images[0].width = width;
	images[0].height = height;

	glfwSetWindowIcon(window, 1, images);
	stbi_image_free(icon);

	debug.success_debug("Icon was init");

	GLfloat vertices[] = {
		// Передняя грань (плоскость Z = -0.5)
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Вершина 0
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Вершина 1
		 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, // Вершина 2
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, // Вершина 3

		// Задняя грань (плоскость Z = +0.5)
		-0.5f, -0.5f, +0.5f, 0.0f, 1.0f, // Вершина 4
		 0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Вершина 5
		 0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Вершина 6
		-0.5f, +0.5f, +0.5f, 0.0f, 0.0f, // Вершина 7

		// Левый бок (слева)
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Вершина 8
		-0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Вершина 9
		-0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Вершина 10
		-0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Вершина 11

		// Правый бок (справа)
		 0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Вершина 12
		 0.5f, -0.5f, +0.5f, 1.0f, 1.0f, // Вершина 13
		 0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Вершина 14
		 0.5f, +0.5f, -0.5f, 0.0f, 0.0f, // Вершина 15

		 // Верх (сверху)
		 -0.5f, +0.5f, -0.5f, 0.0f, 1.0f, // Вершина 16
		  0.5f, +0.5f, -0.5f, 1.0f, 1.0f, // Вершина 17
		  0.5f, +0.5f, +0.5f, 1.0f, 0.0f, // Вершина 18
		 -0.5f, +0.5f, +0.5f, 0.0f, 0.0f, // Вершина 19

		 // Низ (снизу)
		 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // Вершина 20
		  0.5f, -0.5f, -0.5f, 1.0f, 1.0f, // Вершина 21
		  0.5f, -0.5f, +0.5f, 1.0f, 0.0f, // Вершина 22
		 -0.5f, -0.5f, +0.5f, 0.0f, 0.0f  // Вершина 23
	};

	GLuint indices[] = {
		// Передняя грань
		0, 1, 2,
		2, 3, 0,

		// Задняя грань
		4, 5, 6,
		6, 7, 4,

		// Левый бок (слева)
		8, 9, 10,
		10, 11, 8,

		// Правый бок (справа)
		12, 13, 14,
		14, 15, 12,

		// Верх (сверху)
		16, 17, 18,
		18, 19, 16,

		// Низ (снизу)
		20, 21, 22,
		22, 23, 20
	};

	// Загрузка текстуры
	int channels;

	const char* texture_path = "res/textures/fgfggf.jpg";
	unsigned char* image = stbi_load(texture_path, &width, &height, &channels, 0); // Загружаем наш image

	if (image == nullptr) {
		debug.error_debug("Failed load texture image ->");
		debug.error_debug(texture_path);
		return -1;
	}

	GLuint texture;

	glGenTextures(1, &texture); // Генерируем texture
	glBindTexture(GL_TEXTURE_2D, texture); // Привязуем нашу texture

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Ставим параметр, при выходе за границы S(x), что изображение будет повторяться
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Ставим параметр, при выходе за границы T(y), что изображение будет повторяться
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum format = GL_RGB;

	if (channels == 3) {
		format = GL_RGB;
	}
	else if (channels == 4) {
		format = GL_RGBA;
	}
	else {
		debug.error_debug("Error Create format::Unsupported channel used");
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image); // Наносим пиксели

	stbi_image_free(image);

	glActiveTexture(GL_TEXTURE0); // Устанавливаем индекс нашей текстуры
	glBindTexture(GL_TEXTURE_2D, texture); // Опять привязуем

	debug.success_debug("Texture was loaded ->");
	debug.success_debug(texture_path);

	GLuint VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO); // Генерируем VAO
	glGenBuffers(1, &VBO); // Генерируем VBO
	glGenBuffers(1, &EBO); // Генерируем EBO

	glBindVertexArray(VAO); // Привязываем VAO

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // Привязываем VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Загружаем данные в VBO

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Привязываем EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Загружаем данные в EBO

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glfwSetKeyCallback(window, key_callback); // Устанавливаем коллбек для клавиш

	glEnable(GL_DEPTH_TEST);

	camera = new Camera(shader, 45.0f, 0.1f, 100.0f, (float)width, (float)height);

	if (camera == nullptr) {
		debug.error_debug("Camera is not init");
	}
	else {
		debug.success_debug("Camera init");
	}

	vec3 position = vec3(0.1f, 1.0f, -3.0f);

	glfwSetCursorPosCallback(window, mouse_callback);

	// Создаем камеру

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		cout << "X - " << camera->cameraPos.x << " Y - " << camera->cameraPos.y << " Z - " << camera->cameraPos.z << endl;;

		camera->update_delta();

		glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use(); // Используем шейдеры

		float distance = length(camera->cameraPos - position);

		if (distance < 0.7f) { // Коллизия
			camera->moveBackward(3.0f);
			debug.success_debug("Collision Detected");
		}

		camera->updateViewProjection();
		camera->change_position(position);

		do_movement();

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	return 0;
}

// Функция обработки нажатий клавиш
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {

	if (action == GLFW_PRESS) {
		keys[key] = true;
	}

	if (action == GLFW_RELEASE) {
		keys[key] = false;
	}

}

void do_movement() {
	if (keys[GLFW_KEY_W]) {
		camera->moveForward(2.0f);
	}
	if (keys[GLFW_KEY_S]) {
		camera->moveBackward(2.0f);
	}
	if (keys[GLFW_KEY_A]) {
		camera->moveLeft(2.0f);
	}
	if (keys[GLFW_KEY_D]) {
		camera->moveRight(2.0f);
	}
	if (keys[GLFW_KEY_SPACE]) {
		camera->moveUp(2.0f);
	}
	if (keys[GLFW_KEY_LEFT_SHIFT]) {
		camera->moveDown(2.0f);
	}
}

bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		camera->lastX = xpos;
		camera->lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - camera->lastX;
	GLfloat yoffset = camera->lastY - ypos; // Инвертируем Y, чтобы движение мыши вверх было "положительным"

	camera->lastX = xpos;
	camera->lastY = ypos;

	GLfloat sensivity = 0.3f; // Увеличиваем чувствительность для лучшего управления
	xoffset *= sensivity;
	yoffset *= sensivity;

	camera->yaw += xoffset;   // Обновляем угол yaw
	camera->pitch += yoffset; // Обновляем угол pitch

	// Ограничиваем угол pitch, чтобы избежать переворота камеры
	if (camera->pitch > 89.0f) {
		camera->pitch = 89.0f;
	}
	if (camera->pitch < -89.0f) {
		camera->pitch = -89.0f;
	}

	// Вычисляем новый вектор направления камеры
	vec3 front;
	front.x = cos(radians(camera->yaw)) * cos(radians(camera->pitch));
	front.y = sin(radians(camera->pitch));
	front.z = sin(radians(camera->yaw)) * cos(radians(camera->pitch));
	camera->cameraFront = normalize(front); // Нормализуем вектор для единичной длины
}