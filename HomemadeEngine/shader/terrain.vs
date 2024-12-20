#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

out float height;
out vec3 fragPos;
out vec4 fragPos_lightT;
out vec3 normal;
out vec2 texCoord;
out vec3 tangent;

uniform mat4 transform;
uniform mat4 modelTransform;
uniform mat4 lightTransform;
uniform float yScale;
uniform float yShift;

void main() {
	//height = (aPos.y * yScale) - yShift;
	height = aPos.y;
	fragPos = (modelTransform * vec4(aPos, 1.0)).xyz;
	//fragPos_lightT = lightTransform * vec4(fragPos, 1.0);

	mat4 invTransModelTransform = transpose(inverse(modelTransform));
	normal = (invTransModelTransform * vec4(aNormal, 0.0)).xyz;
	tangent = (invTransModelTransform * vec4(aTangent, 0.0)).xyz;

	texCoord = aTexCoord;
	gl_Position = transform * vec4(aPos.x, height, aPos.z, 1.0);
}