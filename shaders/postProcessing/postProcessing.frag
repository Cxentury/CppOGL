#version 330 core

out vec4 FragColor;
in vec2 textCoord;

//raw scene texture
uniform sampler2D texture0;

//postProcessed scene's texture;
uniform sampler2D texture1;

struct ChromaticAberration_t {
    float redOff, greenOff, blueOff;
};

struct Bloom_t {
    float intensity;
};

struct Hdr_t {
    float exposure;
};

struct Effects_t{
    bool bloom, chromaticAberration, blur, hdr, bokeh;
};

uniform ChromaticAberration_t cAberration;
uniform Effects_t effects;
uniform float gamma;

void main() {
    if (effects.chromaticAberration) {
        vec2 direction = textCoord - vec2(0.0);

        FragColor.r = texture(texture0, textCoord - (direction * cAberration.redOff)).r;
        FragColor.g = texture(texture0, textCoord - (direction * cAberration.greenOff)).g;
        FragColor.ba = texture(texture0, textCoord - (direction * cAberration.blueOff)).ba;
    }
    else if (effects.blur)
        FragColor = texture(texture1, textCoord);

    else if(effects.bokeh){
        int boxSize = 3;
        //Putting this here until I create a new shader for the bokeh blur
	    for(int i = -boxSize; i <= boxSize; i++){
		    for(int j=-boxSize; j <= boxSize; j++)
			    FragColor+=texture(texture1, textCoord.xy + vec2(i,j) / textureSize(texture1,0));
	    }
	    FragColor /= pow(boxSize*2+1,2);
    }
    else if(effects.hdr){
        vec3 hdrColor = texture(texture0, textCoord).rgb;
	    vec3 toneMap = hdrColor / (hdrColor + vec3(1.0));
        toneMap = pow(toneMap, vec3(1.0 / gamma));
	    FragColor = vec4(toneMap,1.0);
    }

    else
        FragColor = texture(texture0, textCoord);
}