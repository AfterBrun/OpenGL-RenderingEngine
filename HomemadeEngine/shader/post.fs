#version 330 core

in vec2 texCoord;
out vec4 FragColor;

uniform sampler2D tex;
uniform float gamma;

void main() {
    vec4 pixel = texture(tex, texCoord);
    FragColor = vec4(pow(pixel.rgb, vec3(gamma)), 1.0);
}