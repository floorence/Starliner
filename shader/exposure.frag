#version 330 core
out float nextExposure;

uniform sampler2D logLuminance;
uniform sampler2D previousExposure;
uniform float deltaTime;

void main() {
    const float desired = 0.18;

    float targetLogLuminance = textureLod(logLuminance, vec2(0.5), 99.0).r;
    float targetLuminance = exp(targetLogLuminance);
    float targetExposure = desired / targetLuminance;
    float prevExposure = texture(previousExposure, vec2(0.5)).r;
    
    // human eyes adapt to light quickly, but dark environments slowly
    float speed = (targetExposure < prevExposure) ? 2.0 : 1.0; 
    // calculate the gradual blend factor (Temporal Adaptation)
    float factor = 1.0 - exp(-deltaTime * speed);
    
    nextExposure = mix(prevExposure, targetExposure, factor);
}
