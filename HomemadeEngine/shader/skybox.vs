#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 texCoord;

uniform mat4 transform;

void main() {
	texCoord = aPos;
	vec4 pos = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position = pos.xyww;
}