#version 330 core
out vec4 color;

in vec2 TexCoord; // Получаем текстурные координаты из вершинного шейдера

uniform sampler2D ourTexture;

void main() {
   color = texture(ourTexture , TexCoord); // Получаем цвет из текстуры
}
