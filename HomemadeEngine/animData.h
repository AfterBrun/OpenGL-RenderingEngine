#pragma once
#include "common.h"

struct BoneInfo {
	int id;					//final bone matricies�� �ε��� ��ȣ
	glm::mat4 offset;		//model space -> bone space�� �� ��ȯ matrix
};