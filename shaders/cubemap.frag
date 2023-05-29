#version 330 core

out vec4 FragColor;

in vec3 textCoords;
uniform samplerCube skybox;
uniform sampler2D skyboxHDR;
uniform bool hdr;

vec2 sampleSpherical(vec3 local){
	vec2 uv = vec2(atan(local.z, local.x), asin(local.y));
	uv *= vec2(0.1591, 0.3183);
	uv += 0.5;
	return uv;
}

void main(){

	vec3 color;
	if(hdr){
		vec2 uv = sampleSpherical(normalize(textCoords));
		color = texture(skyboxHDR, uv).rgb;
	}
	else
		color = texture(skybox, textCoords).rgb;

	FragColor = vec4(color,1.0);
}