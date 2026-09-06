#version 330 core
out float nextExposure;

uniform sampler2D targetMipmap;
uniform sampler2D previousExposure;
uniform float deltaTime;

void main() {
    float targetLogSum = textureLod(targetMipmap, vec2(0.5), 99.0).r; 
    float targetLuminance = exp(targetLogSum);
    float prevLuminance = texture(previousExposure, vec2(0.5)).r;
    
    // human eyes adapt to light quickly, but dark environments slowly
    float speed = (targetLuminance > prevLuminance) ? 2.0 : 1.0; 
    // calculate the gradual blend factor (Temporal Adaptation)
    float factor = 1.0 - exp(-deltaTime * speed);
    
    nextExposure = mix(prevLuminance, targetLuminance, factor);
    // nextExposure = targetLuminance;
}
