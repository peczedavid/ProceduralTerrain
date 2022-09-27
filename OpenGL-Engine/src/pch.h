#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>

#include <string>
#include <cinttypes>
#include <wtypes.h>

#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <algorithm>
#include <memory>
#include <functional>
#include <utility>
#include <ctime>
#include <random>
#include <thread>

// Vendors start
#define GLM_FORCE_SIMD_AVX2
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <stb_image.h>
#include <stb_image_write.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
// Vendors end

#include "Core/Log.h"

#include <Windows.h>