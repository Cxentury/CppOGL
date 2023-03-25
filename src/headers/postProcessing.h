#ifndef POST_PROCESSING_HEADER
#define POST_PROCESSING_HEADER

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <map>

#include "shader.h"

struct ChromaticAberation_t{
    float redOff = 0.009, greenOff = 0.006, blueOff = -0.002;
};

struct Bloom_t{

};

struct Hdr_t{
    float exposure;
};

class PostProcessing{

public:
    ChromaticAberation_t& getChromatic();
    Bloom_t& getBloom();
    Hdr_t& getHdr();
    bool& getBool(std::string name);

    void updateUniforms(Shader& shader) {
        shader.use();
        std::string uniform;
        for (const auto pair : this->effects) {
            uniform = "effects." + pair.first;
            shader.setBool(uniform, pair.second);
        }
        shader.setFloat("cAberation.redOff", this->cAberation.redOff);
        shader.setFloat("cAberation.greenOff", this->cAberation.greenOff);
        shader.setFloat("cAberation.blueOff", this->cAberation.blueOff);
    }
private:
    std::map<std::string, bool> effects = { {"bloom", false},{"chromaticAberation", false}, {"blur", false}, { "hdr", true } };
    ChromaticAberation_t cAberation;
    Bloom_t bloom; 
    Hdr_t hdr;
};

#endif // POST_PROCESSING_HEADER