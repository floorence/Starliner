#pragma once

#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>

namespace Constants {
	static constexpr glm::vec3 UP = glm::vec3(0.0f, 1.0f, 0.0f);
	static constexpr glm::vec3 DOWN = glm::vec3(0.0f, -1.0f, 0.0f);
	static constexpr glm::vec3 FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
	static constexpr glm::vec3 LEFT = glm::vec3(-1.0f, 0.0f, 0.0f);
	static constexpr glm::vec3 RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);

	static constexpr glm::vec4 TRANSPARENT = glm::vec4(0.0f);
    static constexpr glm::vec3 GREY_1 = glm::vec3(0.1f, 0.1f, 0.1f);
    static constexpr glm::vec3 GREY_2 = glm::vec3(0.2f, 0.2f, 0.2f);
    static constexpr glm::vec3 GREY_3 = glm::vec3(0.3f, 0.3f, 0.3f);
    static constexpr glm::vec3 GREY_4 = glm::vec3(0.4f, 0.4f, 0.4f);
    static constexpr glm::vec3 GREY_5 = glm::vec3(0.5f, 0.5f, 0.5f);
    static constexpr glm::vec3 GREY_6 = glm::vec3(0.6f, 0.6f, 0.6f);
    static constexpr glm::vec3 GREY_7 = glm::vec3(0.7f, 0.7f, 0.7f);
};
