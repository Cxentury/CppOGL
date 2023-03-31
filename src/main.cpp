#include "headers/scene.h"
#include "headers/main.h"
#include "headers/logger.h"

int main(void) {
    
    logger.log("Before render");
    Scene sc = Scene(1600, 900);
    sc.renderLoop();

    return 0;
}