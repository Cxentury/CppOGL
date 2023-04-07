#include "headers/postProcessing.h"

ChromaticAberration_t& PostProcessing::getChromatic(){
    return this->cAberration;
}

Bloom_t& PostProcessing::getBloom(){
    return this->bloom;
}

bool& PostProcessing::getBool(std::string name) {
    return this->effects.find(name)->second;
}

void PostProcessing::updateUniforms(Shader& shader) {
    shader.use();
    std::string uniform;
    for (const auto pair : this->effects) {
        uniform = "effects." + pair.first;
        shader.setBool(uniform, pair.second);
    }
    shader.setFloat("cAberration.redOff", this->cAberration.redOff);
    shader.setFloat("cAberration.greenOff", this->cAberration.greenOff);
    shader.setFloat("cAberration.blueOff", this->cAberration.blueOff);
}