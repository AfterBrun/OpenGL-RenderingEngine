#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main() {
	FragColor = texture(tex0, texCoord) * 0.8 + texture(tex1, texCoord) * 0.2;
}