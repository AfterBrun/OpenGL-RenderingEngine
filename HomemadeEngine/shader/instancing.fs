#version 330 core

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D diffuse;

void main() {
	vec4 pixel = texture(diffuse, texCoord);
	if(pixel.a < 0.05) 
		discard;

	fragColor = pixel;
}