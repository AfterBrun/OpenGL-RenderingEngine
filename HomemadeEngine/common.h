#pragma once

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

//������ �ؽ�Ʈ�� �д� �Լ�
std::optional<std::string> ReadFileString(const char* fileName);