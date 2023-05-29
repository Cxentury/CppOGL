#version 330 core

out vec4 FragColor;
in vec2 textCoord;

//raw scene texture
uniform sampler2D texture0;

//postProcessed scene's texture;
uniform sampler2D texture1;

struct ChromaticAberation_t {
    float redOff, greenOff, blueOff;
};

struct Bloom_t {
    float intensity;
};

struct Hdr_t {
    float exposure;
};

struct Effects_t{
    bool bloom, chromaticAberation, blur, hdr, bokeh;
};

uniform ChromaticAberation_t cAberation;
uniform Effects_t effects;

void main() {

    if (effects.chromaticAberation) {
        vec2 direction = textCoord - vec2(0.0);

        FragColor.r = texture(texture0, textCoord - (direction * cAberation.redOff)).r;
        FragColor.g = texture(texture0, textCoord - (direction * cAberation.greenOff)).g;
        FragColor.ba = texture(texture0, textCoord - (direction * cAberation.blueOff)).ba;
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

    else
        FragColor = texture(texture0, textCoord);
}