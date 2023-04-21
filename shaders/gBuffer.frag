#version 330 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedospec;

uniform sampler2D diffuseMap0;
uniform sampler2D specularMap0;

in vec2 textCoord;
in vec3 normal;
in vec3 fragPos;

void main(){

	gPosition.xyz = fragPos;
	gPosition.a = 1.0;
	gNormal = normalize(normal);
	gAlbedospec.rgb = texture(diffuseMap0,textCoord).rgb;
	gAlbedospec.a = texture(specularMap0,textCoord).r;
}
