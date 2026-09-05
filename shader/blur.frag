#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main() {
    int mipLevel = 4;
    vec2 tex_offset = 1.0 / textureSize(image, mipLevel); // gets size of single texel
    vec3 result = texture(image, texCoord).rgb * weight[0];

    if (horizontal) {
        for (int i = 1; i < 5; i++) {
            result += textureLod(image, texCoord + vec2(tex_offset.x * i, 0.0), float(mipLevel)).rgb * weight[i];
            result += textureLod(image, texCoord - vec2(tex_offset.x * i, 0.0), float(mipLevel)).rgb * weight[i];
        }
    } else {
        for (int i = 1; i < 5; i++) {
            result += textureLod(image, texCoord + vec2(0.0, tex_offset.y * i), float(mipLevel)).rgb * weight[i];
            result += textureLod(image, texCoord - vec2(0.0, tex_offset.y * i), float(mipLevel)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0);
}