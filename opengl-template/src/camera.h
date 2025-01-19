#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glm.hpp>
#include <GLFW/gtc/matrix_transform.hpp>
#include <GLFW/gtc/type_ptr.hpp>
#include "shader.h"

using namespace glm;

class Camera {
private:
    Shader& shader;
    float angle; // Угол поля зрения
    float near;  // Ближняя плоскость отсечения
    float far;   // Дальняя плоскость отсечения
    float width; // Ширина окна
    float height; // Высота окна
    GLfloat deltaTime = 0.0f;    // Время, прошедшее между последним и текущим кадром
    GLfloat lastFrame = 0.0f;
    
public:
    vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
    vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
    vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
    GLfloat yaw = -90.0f;
    GLfloat pitch = 0.0f;

    Camera(Shader& shader, float angle, float near, float far, float width, float height)
        : shader(shader), angle(angle), near(near), far(far), width(width), height(height) {
        updateViewProjection();
            
        lastFrame = glfwGetTime();
    }

    GLfloat lastX = width / 2.0f;
    GLfloat lastY = height / 2.0f;

    void updateViewProjection() {
        mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        mat4 projection = perspective(radians(angle), width / height, near, far);

        GLuint transformloc_v = glGetUniformLocation(shader.Program, "view");
        glUniformMatrix4fv(transformloc_v, 1, GL_FALSE, value_ptr(view));

        GLuint transformloc_p = glGetUniformLocation(shader.Program, "projection");
        glUniformMatrix4fv(transformloc_p, 1, GL_FALSE, value_ptr(projection));
    }
    
    void update_delta() {
        GLfloat currentFrame = glfwGetTime(); // Получаем текущее время
        deltaTime = currentFrame - lastFrame; // Вычисляем дельта тайм
        lastFrame = currentFrame; // Обновляем lastFrame для следующего кадра
    }

    void change_position(vec3& position) {
        mat4 model = mat4(1.0f);
        model = translate(model, position);

        GLuint transformloc_m = glGetUniformLocation(shader.Program, "model");
        glUniformMatrix4fv(transformloc_m, 1, GL_FALSE, value_ptr(model));
    }

    void moveForward(float distance) {
        cameraPos += cameraFront * distance * deltaTime;
        updateViewProjection();
    }

    void moveBackward(float distance) {
        cameraPos -= cameraFront * distance * deltaTime;
        updateViewProjection();
    }

    void moveRight(float distance) {
        cameraPos += normalize(cross(cameraFront, cameraUp)) * distance * deltaTime;
        updateViewProjection();
    }

    void moveLeft(float distance) {
        cameraPos -= normalize(cross(cameraFront, cameraUp)) * distance * deltaTime;
        updateViewProjection();
    }

    void moveUp(float distance) {
        cameraPos += cameraUp * distance * deltaTime;
        updateViewProjection();
    }
    void moveDown(float distance) {
        cameraPos -= cameraUp * distance * deltaTime;
        updateViewProjection();
    }
};

#endif // CAMERA_H
