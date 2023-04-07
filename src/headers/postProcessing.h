#ifndef POST_PROCESSING_HEADER
#define POST_PROCESSING_HEADER

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>

#include "shader.h"

struct ChromaticAberration_t{
    float redOff = 0.009, greenOff = 0.006, blueOff = -0.002;
};

struct Bloom_t{

};

struct Hdr_t{
    float exposure;
};

class PostProcessing{

public:
    ChromaticAberration_t& getChromatic();
    Bloom_t& getBloom();
    Hdr_t& getHdr();
    bool& getBool(std::string name);
    void updateUniforms(Shader& shader);
private:
    std::map<std::string, bool> effects = { {"bloom", false},{"chromaticAberration", false}, {"blur", false}, { "hdr", false } , {"bokeh", false} };
    ChromaticAberration_t cAberration;
    Bloom_t bloom; 
    Hdr_t hdr;
};

#endif // POST_PROCESSING_HEADER