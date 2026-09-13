#version 330 core

out vec4 FragColor;

in vec2 texCoord;

// These values must match those in enum class ColorSource in Shader.h!!!
const int COLOR_SOURCE_TEXTURE = 0;
const int COLOR_SOURCE_VERTEX_COLOR = 1;
const int COLOR_SOURCE_MATERIAL_COLOR = 2;

uniform int colorSource;
uniform sampler2D diffuse0;
uniform vec3 materialColor;

uniform vec4 tintColor;

void main() {
	vec3 result;
	if (colorSource == COLOR_SOURCE_TEXTURE) {
		vec4 texColor = texture(diffuse0, texCoord); 
		if (texColor.a == 0.0) {
			discard;
		} else {
			result = vec3(texColor);
		}
	} else {
		result = materialColor;
	} // TODO vertex color
	result = mix(result, tintColor.rgb, tintColor.a);
	FragColor = vec4(result, 1.0);
}