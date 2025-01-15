#pragma once
#include "common.h"

struct BoneInfo {
	int id;					//final bone matricies의 인덱스 번호
	glm::mat4 offset;		//model space -> bone space로 의 변환 matrix
};