#version 330 core

out vec4 FragColor;

in vec2 texCoord;

// These values must match those in enum class ColorSource in Shader.h!!!
const int COLOR_SOURCE_TEXTURE = 0;
const int COLOR_SOURCE_VERTEX_COLOR = 1;
const int COLOR_SOURCE_MATERIAL_COLOR = 2;

uniform int colorSource;
uniform sampler2D diffuse0;
uniform vec4 materialColor;

uniform vec4 tintColor;

void main() {
	vec4 result;
	if (colorSource == COLOR_SOURCE_TEXTURE) {
		result = texture(diffuse0, texCoord); 
	} else {
		result = materialColor;
	} // TODO vertex color

	if (result.a == 0.0) {
		discard;
	} else {
		vec3 mixed = mix(result.rgb, tintColor.rgb, tintColor.a);
		FragColor = vec4(mixed, 1.0);
	}
}