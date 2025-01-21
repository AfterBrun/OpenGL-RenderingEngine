#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material { //빛이 반사되어 최종적으로 보일 색깔
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;
};
uniform Material material;

void main()
{    
    FragColor = texture(material.texture_diffuse1, TexCoords);
}