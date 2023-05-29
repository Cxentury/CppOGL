#version 330 core

#define NB_SAMPLES 64
#define NOISE_SIZE 4.0

out float FragColor;

uniform vec2 screenSize;
uniform mat4 projection;

uniform sampler2D texPosition;
uniform sampler2D texNormal;
uniform sampler2D texNoise;

in vec2 textCoord;

uniform vec3 samples[NB_SAMPLES];

vec2 noiseScale = vec2(screenSize.x/NOISE_SIZE, screenSize.y/NOISE_SIZE);

vec3 fragPos = texture(texPosition, textCoord).xyz;
vec3 normal = texture(texNormal, textCoord).xyz;
vec3 randomVec = texture(texNoise, textCoord * noiseScale).xyz;

vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
vec3 BT = cross(normal, tangent);
mat3 VsMatrix = mat3(tangent, BT, normal);

void main(){

	float radius = 0.5, occlusion = 0.0, bias = 0.025;
	for(int i = 0; i < NB_SAMPLES; i++){
		
		vec3 samplePos = VsMatrix * samples[i];
		samplePos = fragPos + samplePos * radius;
		
		vec4 sampleOff = vec4(samplePos, 1.0);
		sampleOff = projection * sampleOff;
		sampleOff.xyz /= sampleOff.w;
		sampleOff.xyz *= 0.5 + 0.5;

		float sampleDepth = texture(texPosition, sampleOff.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, radius / (fragPos.z - sampleDepth));
		occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;

	}
	occlusion = 1.0 - (occlusion / NB_SAMPLES);
	FragColor = occlusion;
}