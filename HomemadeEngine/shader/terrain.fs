#version 330 core

out vec4 FragColor;

in float height;
in vec3 fragPos;
in vec4 fragPos_lightT;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

struct Material {
    sampler2D diffuse;
    sampler2D normalMap;
};
uniform Material material;

uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

uniform sampler2D shadowMap;
uniform sampler2D normalMap;

uniform float gHeight0 = 0.5;
uniform float gHeight1 = 10.0;
uniform float gHeight2 = 30.0;
uniform float gHeight3 = 40.0;

uniform vec3 cameraPos;
uniform int blinn;

struct DirectionalLight { //directional light 파라미터
	//각 요소의 빛의 세기
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
    float shininess;
};
uniform DirectionalLight directionalLight;

vec4 CalcTexColor();
vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 color);
vec4 textureNoTile( sampler2D samp, vec2 uv );
vec4 hash4( vec2 p );
float CalcShadow(vec4 fragPosLight, vec3 lightDir, vec3 normal);

void main()
{
    vec3 texNormal = normalize(textureNoTile(material.normalMap, texCoord).xyz * 2.0 - 1.0);
	vec3 N = normalize(normal);
	vec3 T = normalize(tangent);
	vec3 B = cross(T, N);
	mat3 TBN = mat3(T, B, N);
	vec3 norm = normalize(TBN * texNormal);


    vec3 viewdir = normalize(cameraPos - fragPos);
    float h = (height + 16)/32.0f;	// shift and scale the height into a grayscale value
    //vec4 pixelColor = CalcTexColor();
    vec4 pixelColor = textureNoTile(material.diffuse, texCoord);
    vec3 result = CalcDirectionalLight(directionalLight, norm, viewdir, pixelColor.xyz);
    FragColor = vec4(result, 1.0);
    //FragColor = pixelColor;
}

vec4 CalcTexColor() {
    vec4 texColor;
    if(height < gHeight0) {
        //texColor = texture(tex0, texCoord);
        texColor = textureNoTile(tex0, texCoord);
    }
    else if(height < gHeight1) {
        //vec4 color0 = texture(tex0, texCoord);
        //vec4 color1 = texture(tex1, texCoord);
        vec4 color0 = textureNoTile(tex0, texCoord);
        vec4 color1 = textureNoTile(tex1, texCoord);
        float delta = gHeight1 - gHeight0;
        float factor = (height - gHeight0) / delta;
        texColor = mix(color0, color1, factor);
    }
    else if(height < gHeight2) {
        //vec4 color0 = texture(tex1, texCoord);
        //vec4 color1 = texture(tex2, texCoord);
        vec4 color0 = textureNoTile(tex1, texCoord);
        vec4 color1 = textureNoTile(tex2, texCoord);
        float delta = gHeight2 - gHeight1;
        float factor = (height - gHeight1) / delta;
        texColor = mix(color0, color1, factor);
    }
    else if(height < gHeight3) {
        //vec4 color0 = texture(tex2, texCoord);
        //vec4 color1 = texture(tex3, texCoord);
        vec4 color0 = textureNoTile(tex2, texCoord);
        vec4 color1 = textureNoTile(tex3, texCoord);
        float delta = gHeight3 - gHeight2;
        float factor = (height - gHeight2) / delta;
        texColor = mix(color0, color1, factor);
    }
    else {
        texColor = texture(tex3, texCoord);
    }
    return texColor;
}

vec3 CalcDirectionalLight(DirectionalLight light, vec3 norm, vec3 viewDir, vec3 color) {
	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir, norm);

	float diff = max(dot(norm, lightDir), 0.0);
    
	float spec;

    
	if(blinn == 0) {
		spec = pow(max(dot(viewDir, reflectDir), 0.0), light.shininess);
	}
	else {
		vec3 halfVec = normalize(lightDir + viewDir);
		spec = pow(max(dot(norm, halfVec), 0.0), light.shininess);
	}
    

	vec3 ambient = light.ambient * color;
	vec3 diffuse = light.diffuse * diff * color;
	vec3 specular = light.specular * spec * color;

    
	float shadow = CalcShadow(fragPos_lightT, lightDir, norm);
	diffuse *= (1.0 - shadow);
	specular *= (1.0 - shadow);
    

	return (ambient + diffuse + specular);
    //return (ambient + diffuse);
}

vec4 textureNoTile( sampler2D samp, in vec2 uv )
{
    ivec2 iuv = ivec2( floor( uv ) );
     vec2 fuv = fract( uv );

    // generate per-tile transform
    vec4 ofa = hash4( iuv + ivec2(0,0) );
    vec4 ofb = hash4( iuv + ivec2(1,0) );
    vec4 ofc = hash4( iuv + ivec2(0,1) );
    vec4 ofd = hash4( iuv + ivec2(1,1) );
    
    vec2 ddx = dFdx( uv );
    vec2 ddy = dFdy( uv );

    // transform per-tile uvs
    ofa.zw = sign( ofa.zw-0.5 );
    ofb.zw = sign( ofb.zw-0.5 );
    ofc.zw = sign( ofc.zw-0.5 );
    ofd.zw = sign( ofd.zw-0.5 );
    
    // uv's, and derivatives (for correct mipmapping)
    vec2 uva = uv*ofa.zw + ofa.xy, ddxa = ddx*ofa.zw, ddya = ddy*ofa.zw;
    vec2 uvb = uv*ofb.zw + ofb.xy, ddxb = ddx*ofb.zw, ddyb = ddy*ofb.zw;
    vec2 uvc = uv*ofc.zw + ofc.xy, ddxc = ddx*ofc.zw, ddyc = ddy*ofc.zw;
    vec2 uvd = uv*ofd.zw + ofd.xy, ddxd = ddx*ofd.zw, ddyd = ddy*ofd.zw;
        
    // fetch and blend
    vec2 b = smoothstep( 0.25,0.75, fuv );
    
    return mix( mix( textureGrad( samp, uva, ddxa, ddya ), 
                     textureGrad( samp, uvb, ddxb, ddyb ), b.x ), 
                mix( textureGrad( samp, uvc, ddxc, ddyc ),
                     textureGrad( samp, uvd, ddxd, ddyd ), b.x), b.y );
}

vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)), 
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); 
}


float CalcShadow(vec4 fragPosLight, vec3 lightDir, vec3 normal) {
	vec3 projPoint = fragPosLight.xyz / fragPosLight.w; //perspective divide
	projPoint = projPoint * 0.5 + 0.5; //0.0~1.0 범위로 조절
	float currentDepth = projPoint.z; //현재 깊이값 저장
	float closestDepth = texture(shadowMap, projPoint.xy).r; //쉐도우 맵에서의 깊이값 저장

	float biasFactor = dot(normal, lightDir);
	float bias = mix(0.005, 0.0005, biasFactor);
	float result = closestDepth < currentDepth - bias  ? 1.0 : 0.0;
	
	return result;
}