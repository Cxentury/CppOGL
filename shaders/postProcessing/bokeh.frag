#version 330 core

out vec4 FragColor;
in vec2 textCoord;

int boxSize = 2;
uniform sampler2D screenTexture;

void main() {
	
	FragColor = texture(screenTexture, textCoord);
	float grayScale = dot(FragColor.rgb,vec3(0.21,0.72,0.07));
	vec4 brightestPixel = FragColor;

	for(int i = -boxSize; i <= boxSize; i++){
		for(int j=-boxSize; j <= boxSize; j++){
			vec4 currentPixel = texture(screenTexture, textCoord + vec2(i,j) / textureSize(screenTexture,0));
			
			brightestPixel = grayScale > dot(currentPixel.rgb,vec3(0.21,0.72,0.07)) ? brightestPixel : currentPixel;
			
			grayScale = max(grayScale,dot(currentPixel.rgb,vec3(0.21,0.72,0.07)));
		}
	}
	FragColor.rgb = mix(FragColor.rgb, brightestPixel.rgb, smoothstep(0.1,0.3,grayScale));
}