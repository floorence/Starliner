#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "FontTexture.h"
#include <algorithm>
#include <fstream>
#include "util/Log.h"
#include <fmt/format.h>
#include "stb/stb_image_write.h"

FontTexture::FontTexture(const char* ttfFile, GLenum pixelType)
	: AssetTexture("diffuse0")
{
    std::ifstream file(ttfFile, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        Log::err(TAG, "Failed to open font file.");
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
	unsigned char* ttf_buffer = new unsigned char[size];
    
    if (!file.read(reinterpret_cast<char*>(ttf_buffer), size)) {
		Log::err(TAG, "Failed to read font file.");
		delete[] ttf_buffer;
		return;
	}

	int atlas_w = ATLAS_WIDTH;
    int atlas_h = ATLAS_HEIGHT;
	
    unsigned char* bitmap_pixels = new unsigned char[atlas_w * atlas_h];
	stbtt_bakedchar cData[NUM_CHARS]; // ascii 32-128

    float font_height = fontHeight;
    int result = stbtt_BakeFontBitmap(ttf_buffer, 0, font_height, 
                                      bitmap_pixels, atlas_w, atlas_h, 
                                      32, NUM_CHARS, cData);
    
    if (result <= 0) {
        Log::err(TAG, "stbtt_BakeFontBitmap failed. does the font fit into the atlas matrix?");
    } else {
		//stbi_write_png("assets/font_dev.png", atlas_w, atlas_h, 1, bitmap_pixels, atlas_w);
		processCharData(cData);
		//flipBitmap(bitmap_pixels, atlas_w, atlas_h);
		setTextureData(bitmap_pixels, GL_RED, pixelType, atlas_w, atlas_h);
	}
	delete[] ttf_buffer;
	delete[] bitmap_pixels;
}

std::vector<Vertex> FontTexture::generateVertices(const std::string& text, float x, float y, float lineHeight, float maxWidth, bool center) {
	std::vector<Vertex> vertices;
	processTextRequest(text, x, y, lineHeight, maxWidth, center, &vertices, nullptr);

	return vertices;
}

std::vector<GLuint> FontTexture::generateIndices(std::vector<Vertex>& vertices) {
	// TODO: only needs size of vertices. and indices will be the same for the same number of vertices
	std::vector<GLuint> indices;
	for (uint i = 0; i < vertices.size(); i += 4 /* 4 vertices per quad */ ) {
		indices.insert(indices.end(), {i, i + 2, i + 3, i, i + 3, i + 1});
	}
	return indices;
}

std::pair<float, float> FontTexture::getSize(const std::string& text, float lineHeight, float maxWidth) {
	std::pair<float, float> sizeData;
	processTextRequest(text, 0, 0, lineHeight, maxWidth, false, nullptr, &sizeData);

	return sizeData;
}

std::pair<float, float> FontTexture::getSizeOfChar(char c, float lineHeight) {
	float scale = lineHeight / normalizedLineHeight;
	NormalizedCharQuad ch = charData[c - ' '];
	float width, height;
	scaleCharQuad(ch, scale, &width, &height);
	return {width, height};
}

float FontTexture::getHeightFromBaseline(char c, float charHeight, float lineHeight) {
	if (c == 'g' || c == 'j' || c == 'p' || c == 'q' || c == 'y') { // delimiters
		return charHeight - (charHeight / 4.0f);
	} else if (c == '+' || c == '-' || c == '<' || c == '=' || c == '>' || c == '~') { // center vertically
//		Log::log(TAG, fmt::format("lineHeight: {}, charHeight: {}", lineHeight, charHeight));
		return lineHeight / 2.0f + charHeight / 2.0f;
	} else if (c == '\"' || c == '\'' || c == '*' || c == '^') { // top aligned
		return lineHeight;
	}
	return charHeight; // bottom aligned
}

void FontTexture::scaleCharQuad(NormalizedCharQuad c, float scale, float* width, float* height) {
	*width = (c.x1 - c.x0) * scale;
	*height = (c.y1 - c.y0) * scale;
}

void FontTexture::processCharData(stbtt_bakedchar* cData) {
	for (int i = 0; i < NUM_CHARS; i++) {
		stbtt_bakedchar c = cData[i];
		charData[i].x0 = (float)c.x0 / ATLAS_WIDTH;
		charData[i].x1 = (float)c.x1 / ATLAS_WIDTH;
		charData[i].y0 = (float)c.y0 / ATLAS_HEIGHT;
		charData[i].y1 = (float)c.y1 / ATLAS_HEIGHT;
		// workaround for j being taller than the rest of the letters
		if (i + '0' == 'j') continue;
		normalizedLineHeight = std::max(normalizedLineHeight, charData[i].y1 - charData[i].y0);
	}
}

void FontTexture::processTextRequest(const std::string& text, float x, float y, float lineHeight, float maxWidth, bool center, std::vector<Vertex>* vertexData, std::pair<float, float>* sizeData) {
	float scale = lineHeight / normalizedLineHeight;
	float spacing = lineHeight / 10.0f;
	float spaceWidth = lineHeight / 4.0f;
	float tabWidth = lineHeight * 2;
	// current x and y of the BOTTOM LEFT of the character
	float currX = x;
	float currY = y + lineHeight;
	for (unsigned long i = 0; i < text.size(); i++) {

		// handle "special" characters
		if (text[i] == ' ') {
			currX += spaceWidth;
			continue;
		} else if (text[i] == '\n') {
			currX = x;
			currY += lineHeight;
			continue;
		} else if (text[i] == '\t') {
			currX += tabWidth;
			continue;
		} else if (text[i] < ' ' /* char type can't be > 127 so no need to check here */) {
			Log::warn(TAG, fmt::format("unrecognized character: {}", text[i]));
			continue;
		}

		NormalizedCharQuad c = charData[text[i] - ' '];
		float width, height;
		scaleCharQuad(c, scale, &width, &height);
//		Log::log(TAG, fmt::format("lineHeight: {}, height: {}", lineHeight, height));

		// constrain width
		if (maxWidth > 0 && currX + spacing + width > x + maxWidth + OVERFLOW_ALLOWANCE) {
			//Log::log(TAG, fmt::format("{}, {}", currX + spacing + width, x + maxWidth));
			currX = x;
			currY += lineHeight;
		}
		// spacing at the start of every letter unless it's at the beginning of a line
		if (currX != x) currX += spacing;
	
		if (vertexData != nullptr) {
			float baselineHeight;
			if (center) {
				baselineHeight = lineHeight / 2.0f + height / 2.0f;
			} else {
				baselineHeight = getHeightFromBaseline(text[i], height, lineHeight);
			}

			// texCoords are normalized while actual vertex positions are not and follow window coordinates
			Vertex topLeft = {
				glm::vec3(currX, currY - baselineHeight, 0.0f),
				glm::vec3(0.0f),
				glm::vec2(c.x0, c.y0)
			};
			Vertex topRight = {
				glm::vec3(currX + width, currY - baselineHeight, 0.0f),
				glm::vec3(0.0f),
				glm::vec2(c.x1, c.y0)
			};
			Vertex botLeft = {
				glm::vec3(currX, currY - baselineHeight + height, 0.0f),
				glm::vec3(0.0f),
				glm::vec2(c.x0, c.y1)
			};
			Vertex botRight = {
				glm::vec3(currX + width, currY - baselineHeight + height, 0.0f),
				glm::vec3(0.0f),
				glm::vec2(c.x1, c.y1)
			};

			vertexData->insert(vertexData->end(), {topLeft, topRight, botLeft, botRight});
		}

		currX += width;
	}
	
	if (sizeData != nullptr) {
		sizeData->first = currX - x;
		sizeData->second = currY - y;
	}
}
