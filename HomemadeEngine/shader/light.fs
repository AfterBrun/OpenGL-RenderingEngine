#version 330 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;
in vec3 tangent;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform int blinn;

struct Material { //빛이 반사되어 최종적으로 보일 색깔
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;
};
uniform Material material;

struct DirectionalLight { //directional light 파라미터
	//각 요소의 빛의 세기
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
};
uniform DirectionalLight directionalLight;

struct PointLight { //point light 파라미터
	//각 요소의 빛의 세기
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//빛의 감쇠 파라미터
	float constant;
	float linear;
	float quadratic;

	vec3 position;
};
#define NR_MAX_POINT_LIGHTS 4
uniform PointLight pointLight[NR_MAX_POINT_LIGHTS];

struct SpotLight { //spot light 파라미터
	//각 요소의 빛의 세기
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	//빛의 감쇠 파라미터
	float constant;
	float linear;
	float quadratic;
	
	vec3 position;
	vec3 direction;

	float cutoff;
	float cutoffOuter;
};
uniform SpotLight spotLight;

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

void main() {
	vec3 texNormal = normalize(texture(material.texture_normal1, texCoord).xyz * 2.0 - 1.0);
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent);
	vec3 B = cross(N, T);
	mat3 TBN = mat3(T, B, N);
	vec3 norm = normalize(vec3(0.0, 0.0, 0.0) * texNormal);


	//vec3 norm = normalize(normal);
    vec3 viewDir = normalize(cameraPos - fragPos);

	vec3 result = CalcDirectionalLight(directionalLight, norm, viewDir);	
	
	for(int i = 0; i < NR_MAX_POINT_LIGHTS; i++) {
		if(pointLight[i].constant == 0.0) break; //if point light data none exist(witch mean not initialized)
		result += CalcPointLight(pointLight[i], norm, viewDir);
	}
	

	result += CalcSpotLight(spotLight, norm, viewDir);
	
	
	FragColor = vec4(result, 1.0);
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec;
	if(blinn == 0) {
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}
	else {
		vec3 halfVec = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfVec), 0.0), material.shininess);
	}

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoord));
	vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoord));

	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	//attenuation 계산
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + (light.linear * distance) + (light.quadratic * (distance*distance)));

	float diff = max(dot(normal, lightDir), 0.0);
	float spec;
	if(blinn == 0) {
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	}
	else {
		vec3 halfVec = normalize(lightDir + viewDir);
		spec = pow(max(dot(normal, halfVec), 0.0), material.shininess);
	}

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).xyz;
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, texCoord).xyz;
	vec3 specular = light.specular * spec * texture(material.texture_specular1, texCoord).xyz;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	
	return (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir) {
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	//attenuation 계산
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + (light.linear * distance) + (light.quadratic * (distance*distance)));

	//cutoff 계산
	float theta = dot(lightDir, normalize(-light.direction));
	float intensity = clamp((theta - light.cutoffOuter) / (light.cutoff - light.cutoffOuter), 0.0, 1.0);

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, texCoord).xyz;
	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	if(intensity > 0.0) {
		float diff = max(dot(normal, lightDir), 0.0);
		float spec;
		if(blinn == 0) {
			spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		}
		else {
			vec3 halfVec = normalize(lightDir + viewDir);
			spec = pow(max(dot(normal, halfVec), 0.0), material.shininess);
		}
		
		diffuse = light.diffuse * diff * texture(material.texture_diffuse1, texCoord).xyz;
		specular = light.specular * spec * texture(material.texture_specular1, texCoord).xyz;

		diffuse *= attenuation * intensity;
		specular *= attenuation * intensity;
	}

	return (ambient + diffuse + specular);
}