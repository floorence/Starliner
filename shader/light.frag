#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 LogLuminance;

uniform vec3 materialColor;

void main() {
	FragColor = vec4(materialColor, 1.0);
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 50.0) {
		// bright bloom blur texture is NOT tone mapped in hdr_bloom.frag, must map to 0-1 here, otherwise blurred areas will be too bright and not look blurred
		float maxColorChannel = max(max(FragColor.r, FragColor.g), FragColor.b);
		vec3 mapped = FragColor.rgb / maxColorChannel;
        BrightColor = vec4(mapped, 1.0);
	} else {
		BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
	}

    LogLuminance = vec4(log(0.001 + brightness), 0.0, 0.0, 1.0);
}