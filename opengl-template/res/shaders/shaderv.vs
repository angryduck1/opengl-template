#version 330 core
layout(location = 0) in vec3 position; // Позиция
layout(location = 1) in vec2 texCoord; // Текстурные координаты

out vec2 TexCoord; // Передаем текстурные координаты во фрагментный шейдер
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    TexCoord = texCoord; // Передаем текстурные координаты
}
