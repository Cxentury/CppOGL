#version 330 core
out vec4 FragColor;

in vec2 textCoord;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D diffuseMap0;
uniform sampler2D diffuseMap1;
uniform sampler2D specularMap0;
uniform sampler2D opacityMap0;

//checks if fragment should use a material or a texture color 
uniform bool hasMaterial = false;

struct Material{
    float specularExponent, opacity;
    vec3 diffuse, ambiant, specular;
};

struct Textures{
    bool diffuse, specular, opacity, normal;
};

struct PointLight{
    vec3 lightColor, lightPos;
    float constant, linear, quadratic;
    float ambiantStr, specularStr;
    bool activeLight;
};

struct DirLight{
    vec3 lightDir, lightColor;
    float ambiantStr;
    float specularStr;
    bool activeLight;
};

#define MAX_LIGHTS 4
uniform PointLight pLights[MAX_LIGHTS];
uniform DirLight dLight[MAX_LIGHTS];
uniform Textures maps;
uniform Material material;

uniform vec3 viewPos;
vec3 pointLight(PointLight plight, vec3 normalN);
vec3 directionalLight(DirLight light, vec3 normalN);

void main() {
    
    vec3 normalN = normalize(normal);
    vec3 lightOutput = vec3(0.0);

    for(int i = 0; i < MAX_LIGHTS; i++){
        if(pLights[i].activeLight == true){
            lightOutput += pointLight(pLights[i], normalN);
        }
        if(dLight[i].activeLight == true){
            lightOutput += directionalLight(dLight[i], normalN);
        }
    }
    
    if(maps.opacity)
        FragColor = vec4(lightOutput,texture(opacityMap0,textCoord).r);
    else
        FragColor = vec4(lightOutput,1.0);

}

vec3 pointLight(PointLight pLight, vec3 normalN){
    
    vec3 diffuseTexture;
    if(maps.diffuse){
        diffuseTexture = vec3(texture(diffuseMap0,textCoord));
    }
    else{
        diffuseTexture = vec3(1.0);
    }
    //vector which points from the fragment position to the light source
    vec3 lightDirection = normalize(pLight.lightPos - fragPos);
    
    //calculate the diffuse impact on a fragment, the greater the angle between the normal and the lightDirection
    //the darker the it will get    
    float diffuseShading = max(dot(normalN, lightDirection),0.0);
    
    /* Specular */
    /*reflect function expects the first vector to point from the light source toward the fragPos
     but ours points towards the light source from fragPos (because we did lightPos - fragPos and not the other way around
     so we negate it*/
    vec3 reflectDirection = reflect(-lightDirection, normalN);
    vec3 viewDirection = normalize(viewPos - fragPos);
    float specAmount = pow(max(dot(viewDirection,reflectDirection),0.0),32);
    /* --------- */
    
    float distance = length(pLight.lightPos - fragPos);
    float attenuation = 1.0 / (pLight.constant + pLight.linear * distance + pLight.quadratic * (distance * distance));
    
    vec3 ambiant, specular, diffuse;

    if(hasMaterial){
        float specAmount = pow(max(dot(viewDirection,reflectDirection),0.0),material.specularExponent);
        specular = pLight.specularStr * specAmount * pLight.lightColor * material.specular * attenuation;
        ambiant = pLight.ambiantStr * material.ambiant * attenuation;
        diffuse = diffuseShading * pLight.lightColor * material.diffuse * attenuation;
    }
    else{
        float specAmount = pow(max(dot(viewDirection,reflectDirection),0.0),32);
        specular = pLight.specularStr * specAmount * pLight.lightColor * vec3(texture(specularMap0, textCoord)) * attenuation;
        ambiant = pLight.ambiantStr * diffuseTexture * attenuation;
        diffuse = diffuseShading * pLight.lightColor * diffuseTexture * attenuation;
    }
    
    //ambiant and diffuse lightning
    return (ambiant + diffuse + specular);
}

vec3 directionalLight(DirLight light, vec3 normalN){

    vec3 diffuseTexture;
    if(maps.diffuse){
        diffuseTexture = vec3(texture(diffuseMap0,textCoord));
    }
    else{
        diffuseTexture = vec3(1.0,1.0,1.0);
    }

    vec3 lightDir = normalize(-light.lightDir);
    float diffuseShading = max(dot(normalN,lightDir),0.0);


    vec3 viewDirection = normalize(viewPos - fragPos);
    vec3 reflectDirection = reflect(-lightDir,normalN);

    float specAmount;
    vec3 ambiant, specular, diffuse;

    if(hasMaterial){
        specAmount = pow(max(dot(viewDirection,reflectDirection),0.0),material.specularExponent);

        ambiant = light.ambiantStr * light.lightColor * material.diffuse;
        specular = light.specularStr * light.lightColor * specAmount * material.specular;
        diffuse = diffuseShading  * light.lightColor * material.diffuse;
    }
    else{
        specAmount = pow(max(dot(viewDirection, reflectDirection),0.0),32);

        ambiant = light.ambiantStr * light.lightColor * vec3(texture(diffuseMap0,textCoord));
        diffuse = diffuseShading * light.lightColor * vec3(texture(diffuseMap0,textCoord));
        specular = light.specularStr * specAmount * vec3(texture(specularMap0,textCoord));
    }

    return (ambiant + diffuse + specular);
}