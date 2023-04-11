#version 330 core
out vec4 FragColor;

in vec2 textCoord;

uniform sampler2D screenTexture;

void main() {
	const float gamma = 2.2;
	vec3 hdrColor = texture(screenTexture, textCoord).rgb;

	vec3 toneMap = hdrColor / (hdrColor + vec3(1.0));

//	toneMap = pow(toneMap, vec3(1.0 / gamma));

	FragColor = vec4(toneMap,1.0);
}