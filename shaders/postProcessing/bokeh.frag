#version 330 core

out vec4 FragColor;
in vec2 textCoord;

int boxSize = 2;
uniform sampler2D texture0;

void main() {
	
	float minTheshold = 0.1, maxThreshold = 0.3;
	
	FragColor = texture(texture0, textCoord);
	float grayScale = dot(FragColor.rgb,vec3(0.21,0.72,0.07));
	vec4 brightestPixel = FragColor;

	for(int i = -boxSize; i <= boxSize; i++){
		for(int j=-boxSize; j <= boxSize; j++){
		
			//To make a circular shape
			if(distance(vec2(i,j), vec2(0,0)) >= boxSize) continue;
			vec4 currentPixel = texture(texture0, textCoord + vec2(i,j) / textureSize(texture0,0));
			
			brightestPixel = grayScale > dot(currentPixel.rgb,vec3(0.21,0.72,0.07)) ? brightestPixel : currentPixel;
			
			grayScale = max(grayScale,dot(currentPixel.rgb,vec3(0.21,0.72,0.07)));
		}
	}

	FragColor.rgb = mix(FragColor.rgb, brightestPixel.rgb, smoothstep(minTheshold,maxThreshold,grayScale));
}