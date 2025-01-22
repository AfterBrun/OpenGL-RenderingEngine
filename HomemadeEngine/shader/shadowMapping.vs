#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangent;

layout(location = 4) in ivec4 boneIds; 
layout(location = 5) in vec4 weights;

out vec3 fragPos;
out vec4 fragPos_lightT;
out vec3 normal;
out vec2 texCoord;
out vec3 tangent;

uniform mat4 transform;
uniform mat4 modelTransform;
uniform mat4 lightTransform;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];
uniform int entity_animation;

void main() {
	vec4 totalPosition = vec4(0.0f);
    vec3 totalNormal = vec3(0.0f);
    vec3 totalTangent = vec3(0.0f);

    if(entity_animation == 0) 
    {
        totalPosition = vec4(aPos, 1.0f);
        totalNormal = aNormal;
        totalTangent = aTangent;
    }
    else
    {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
        {
            if(boneIds[i] == -1) 
                continue;
            if(boneIds[i] >=MAX_BONES) 
            {
                totalPosition = vec4(aPos, 1.0f);
                totalNormal = aNormal;
                totalTangent = aTangent;
                break;
            }

            vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(aPos,1.0f);
            totalPosition += localPosition * weights[i];

            vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
            totalNormal += localNormal * weights[i];

            vec3 localTangent = mat3(finalBonesMatrices[boneIds[i]]) * aTangent;
            totalTangent += localTangent * weights[i];
        }
    }

	totalNormal = normalize(totalNormal);
	totalTangent = normalize(totalTangent);

	//gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position = transform * totalPosition;

	//fragPos = (modelTransform * vec4(aPos, 1.0)).xyz;
	fragPos = (modelTransform * totalPosition).xyz;

	fragPos_lightT = lightTransform * vec4(fragPos, 1.0);

	mat4 invTransModelTransform = transpose(inverse(modelTransform));
	normal = (invTransModelTransform * vec4(totalNormal, 0.0)).xyz;
	tangent = (invTransModelTransform * vec4(totalTangent, 0.0)).xyz;

	texCoord = aTexCoord;
}
