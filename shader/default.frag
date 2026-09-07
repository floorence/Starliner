#version 330 core

// parts of this file were adapted from
// * https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/2.lighting/6.multiple_lights/6.multiple_lights.fs
// * https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/5.advanced_lighting/3.2.1.point_shadows/3.2.1.point_shadows.fs
// * https://github.com/JoeyDeVries/LearnOpenGL/blob/master/src/5.advanced_lighting/7.bloom/7.bloom.fs

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 LogLuminance;

// These values must match those in enum class ColorSource in Shader.h!!!
const int COLOR_SOURCE_TEXTURE = 0;
const int COLOR_SOURCE_VERTEX_COLOR = 1;
const int COLOR_SOURCE_MATERIAL_COLOR = 2;

// These values must match those in enum class ShadowQuality in LightController.h!!!
const int SHADOW_QUALITY_OFF = 0;
const int SHADOW_QUALITY_LOW = 1;
const int SHADOW_QUALITY_HIGH = 2; // softer shadows

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct PointLight {
    vec3 position;
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;

    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
};

#define MAX_POINT_LIGHTS 100

in vec3 crntPos;
in vec3 normal;
in vec2 texCoord;
in vec3 color;

// These are not part of the material struct since other shaders also have these uniforms and it would be annoying
// to have these have a different uniform name
uniform int colorSource;
uniform vec3 materialColor;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;
uniform samplerCube depthMap;

uniform int numPointLights;
uniform vec4 tintColor;
uniform vec3 camPos;
uniform float farPlane;
uniform int shadowQuality;

vec3 sampleOffsetDirections[20] = vec3[] (
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calculateShadow(vec3 fragPos, vec3 normal, vec3 lightPos, bool softShadows) {
    // calculate bias
    vec3 lightToFrag = fragPos - lightPos;
    vec3 lightDir = normalize(lightPos - fragPos); // direction to light from frag
    float currentDepth = length(lightToFrag);
    // float depthScale = (1.0 / farPlane) * currentDepth; 
    float maxBias = 0.05; float minBias = softShadows ? 0.015 : 0.005;
    float cosTheta = clamp(dot(normal, lightDir), 0.0, 1.0);
    float angleScale = sqrt(1.0 - cosTheta * cosTheta); // sin(theta)
    float depthScale = (currentDepth / farPlane) / (1.0 / 3.0); // 1 at depth 1/3 of farPlane

    float bias = max((maxBias * angleScale * depthScale), minBias);

    // normal offset
    vec3 biasedFragPos = fragPos + normal * bias;
    vec3 biasedLightToFrag = biasedFragPos - lightPos;
    currentDepth = length(biasedLightToFrag);

    float shadow = 0.0;

    if (softShadows) {
        int samples = 20;
        float diskRadius = 0.05 * (1.0 / farPlane) * currentDepth;
        for (int i = 0; i < samples; i++) {
            vec3 sampleDir = biasedLightToFrag + sampleOffsetDirections[i] * diskRadius;
            float closestDepth = texture(depthMap, sampleDir).r;
            closestDepth *= farPlane; // transform [0,1] back to original depth value

            if (currentDepth > closestDepth) shadow += 1.0;
        }
        shadow /= float(samples);
    } else {
        float closestDepth = texture(depthMap, biasedLightToFrag).r;
        closestDepth *= farPlane;
        if (currentDepth > closestDepth) shadow = 1.0;
    }

    return 1.0 - shadow;
}

vec3 calculatePointLight(PointLight light, vec3 texColor, vec3 specColor, vec3 normal, vec3 fragPos, vec3 viewDir) {
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse intensity
    float diff = max(dot(normal, lightDir), 0.0);
    // specular intensity
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results with texture
    vec3 diffuse = diff * texColor;
    // handle if specular texture only has red channel but needs to be grey
    vec3 specTex = specColor;
    vec3 greySpecTex = vec3(specTex.r);
    vec3 specular = spec * greySpecTex;
    diffuse *= attenuation;
    specular *= attenuation;
    float shadow = 1.0;
    if (shadowQuality != SHADOW_QUALITY_OFF)
        shadow = calculateShadow(fragPos, normal, light.position, shadowQuality == SHADOW_QUALITY_HIGH);
    return light.color * shadow * (diffuse + specular);
}

vec3 getColorFromSource() {
    if (colorSource == COLOR_SOURCE_TEXTURE) {
        return vec3(texture(material.diffuse, texCoord));
    } else if (colorSource == COLOR_SOURCE_MATERIAL_COLOR) {
        return materialColor;
    } else {
        return color;
    }
}

float getBrightness(vec3 color) {
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
	vec3 normal = normalize(normal);
	vec3 viewDirection = normalize(camPos - crntPos);
    vec3 texColor = getColorFromSource();
    vec3 specColor = (colorSource == COLOR_SOURCE_TEXTURE) ? vec3(texture(material.specular, texCoord)) : vec3(getBrightness(texColor));

	vec3 result = vec3(0);

	for (int i = 0; i < numPointLights; i++)
        result += calculatePointLight(pointLights[i], texColor, specColor, normal, crntPos, viewDirection);    

    vec3 ambient = 0.2 * texColor;
    result += ambient;
	result = mix(result, tintColor.rgb, tintColor.a);
    
    FragColor = vec4(result, 1.0);

    // uncomment to see depthMap
    // vec3 fragToLight = crntPos - pointLights[0].position;
    // float depth = texture(depthMap, fragToLight).r;
    // FragColor = vec4(vec3(depth), 1.0);

    LogLuminance = vec4(log(0.001 + getBrightness(FragColor.rgb)), 0.0, 0.0, 1.0);

    // check whether fragment output is higher than threshold, if so output as brightness color
    float brightness = getBrightness(FragColor.rgb);
    if (brightness > 50.0) {
		// bright bloom blur texture is NOT tone mapped in hdr_bloom.frag, must map to 0-1 here, otherwise blurred areas will be too bright and not look blurred
		float maxColorChannel = max(max(FragColor.r, FragColor.g), FragColor.b);
		vec3 mapped = FragColor.rgb / maxColorChannel;
        BrightColor = vec4(mapped, 1.0);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    // uncomment to temporarily disable bloom for non light sources
    // BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}