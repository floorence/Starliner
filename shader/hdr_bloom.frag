#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D hdrBuffer; // actual scene
uniform sampler2D bloomBlur; // blurred bright areas
uniform sampler2D exposureTex; // 1x1 exposure
uniform bool bloomEnabled;

void main() {             
    // const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, texCoord).rgb;
    vec3 bloomColor = texture(bloomBlur, texCoord).rgb;
    float exposure = texture(exposureTex, texCoord).r;
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    //vec3 bloomResult = vec3(1.0) - exp(-bloomColor * exposure);
    // bloomBlur is pre-tonemapped in default.frag and light.frag since it should be based on each bright area's colour instead of exposure based on scene brightness
    if (bloomEnabled) result += bloomColor; // additive blending
    // also gamma correct while we're at it       
    // result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

    // uncomment to see hdrColor
    // FragColor = vec4(hdrColor, 1.0);

    // uncomment to see bloom blur
    // FragColor = vec4(bloomColor, 1.0);

    // uncomment to see exposure, divided by 10 since it could be greater than 1
    // FragColor = vec4(vec3(exposure / 10.0), 1.0);
}