#include "context.h"


std::unique_ptr<context> context::Create() {
	auto contextInstance = std::unique_ptr<context>(new context());
	if (!contextInstance->Init()) {
		SPDLOG_INFO("Failed to create Context");
		return nullptr;
	}
	return std::move(contextInstance);
}

void context::viewSizeChange(int width, int height) {
	m_width = width;
	m_height = height;
	glViewport(0, 0, width, height);
	//frame buffer 생성
	if (width == 0 && height == 0) { //윈도우 최소화 했을경우 0X0 이 아닌 최소크기의 프레임퍼버 생성
		m_frameBuffer = FrameBuffer::Create(texture::Create(1, 1, GL_RGBA));
		return;
	}
	m_frameBuffer = FrameBuffer::Create(texture::Create(m_width, m_height, GL_RGBA));	
}

void context::keyEvent(GLFWwindow* window) {
	camera->MovePosition_KeyEvent(window, 0.2f, deltaTime);
}

void context::mouseCursorEvent(double xPos, double yPos) {
	camera->Rotation_MouseEvent(xPos, yPos, 10.0f, deltaTime);
}

void context::mouseButtonEvent(int button, int action, double xpos, double ypos) {
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			camera->SetMouseRotationButton(true);
			camera->SetMousePrevPos(xpos, ypos);
		}
		else if (action == GLFW_RELEASE) {
			camera->SetMouseRotationButton(false);
		}
	}
}


bool context::Init() {
	//program 생성
	simpleProgram = ShaderProgram::CreateShaderProgram("./shader/simple.vs", "./shader/simple.fs");
	postProgram = ShaderProgram::CreateShaderProgram("./shader/post.vs", "./shader/post.fs");
	lightProgram = ShaderProgram::CreateShaderProgram("./shader/shadowMapping.vs", "./shader/shadowMapping.fs");
	skyboxProgram = ShaderProgram::CreateShaderProgram("./shader/skybox.vs", "./shader/skybox.fs");
	instancingProgram = ShaderProgram::CreateShaderProgram("./shader/instancing.vs", "./shader/instancing.fs");
	terrainProgram = ShaderProgram::CreateShaderProgram("./shader/terrain.vs", "./shader/terrain.fs");
	normalProgram = ShaderProgram::CreateShaderProgram("./shader/normal.vs", "./shader/normal.fs");
	skeletalProgram = ShaderProgram::CreateShaderProgram("./shader/skeletalAnimation.vs", "./shader/skeletalAnimation.fs");


	//program 실패 여부 판정
	if (!lightProgram || !postProgram || !lightProgram || !skyboxProgram || !instancingProgram || !terrainProgram ||
		!normalProgram) {
		return false;
	}

	//메쉬 객체 생성
	m_box = Mesh::CreateBox();
	m_floor = Mesh::CreateBox();
	m_plane = Mesh::CreatePlane();
	m_brickwall = Mesh::CreatePlane();

	//모델 로딩
	m_backpack = Model::LoadModel("./asset/model/backpack/backpack.obj");
	m_vampire = Model::LoadModel("./asset/model/vampire/dancing_vampire.dae");
	m_animation = Animation::NewAnimation("./asset/model/vampire/dancing_vampire.dae", m_vampire.get());	 
	m_animator = Animator::NewAnimator(m_animation.get());
	SPDLOG_INFO("final transform matrices: {}", m_animator->GetFinalBoneMatrices().size());


	auto container2 = image::CreateFromFile("./asset/texture/container2.png");			//이미지 생성
	auto container2_metal = image::CreateFromFile("./asset/texture/container2_specular.png");
	auto floorImage = image::CreateFromFile("./asset/texture/marble.jpg");
	auto brickwall_texture = image::CreateFromFile("./asset/texture/brickwall.jpg");
	auto brickwall_normal = image::CreateFromFile("./asset/texture/brickwall_normal.jpg");
	auto defualt_normal = image::CreateFromFile("./asset/texture/default_normal.png");


	//스카이박스 초기화
	auto cubeRight = image::CreateFromFile("./asset/cube_texture/skybox/right.jpg", false);
	auto cubeLeft = image::CreateFromFile("./asset/cube_texture/skybox/left.jpg", false);
	auto cubeTop = image::CreateFromFile("./asset/cube_texture/skybox/top.jpg", false);
	auto cubeBottom = image::CreateFromFile("./asset/cube_texture/skybox/bottom.jpg", false);
	auto cubeFront = image::CreateFromFile("./asset/cube_texture/skybox/front.jpg", false);
	auto cubeBack = image::CreateFromFile("./asset/cube_texture/skybox/back.jpg", false);
	m_cubeTexture = CubeTexture::CreateFromImages({
	  cubeRight.get(),
	  cubeLeft.get(),
	  cubeTop.get(),
	  cubeBottom.get(),
	  cubeFront.get(),
	  cubeBack.get(),
	});

	//각종 메쉬 메테리얼 적용
	m_box->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(container2.get(), "texture_diffuse"));
	m_box->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(container2_metal.get(), "texture_specular"));
	m_box->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(defualt_normal.get(), "texture_normal"));

	m_floor->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(floorImage.get(), "texture_diffuse"));
	m_floor->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(floorImage.get(), "texture_specular"));
	m_floor->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(defualt_normal.get(), "texture_normal"));

	
	m_brickwall->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(brickwall_texture.get(), "texture_diffuse"));
	m_brickwall->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(brickwall_texture.get(), "texture_specular"));
	m_brickwall->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(brickwall_normal.get(), "texture_normal"));
	

	//지형 초기화
	m_terrain = Terrain::CreateWithHeightMap("./asset/texture/heightmap4.jpg");
	m_terrain->SetMaterial(image::CreateFromFile("./asset/texture/coast_sand_diff.jpg").get(),
						   image::CreateFromFile("./asset/texture/coast_sand_nor.jpg").get());

	//카메라 객체 생성
	camera = camera::Create(cameraPos, cameraFront, cameraUp);

	//쉐도우 맵 생성
	m_shadowMap = ShadowMap::Create(16384, 16384);

	return true;
}

void context::Render() {
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	//렌더링 영역
	if (ImGui::Begin("Configure UI")) {
		ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor));
		ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
		ImGui::DragFloat3("Camera Position", camera->CameraPosPtr(), 0.01f);
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Light Configure", ImGuiTreeNodeFlags_DefaultOpen)) { //빛 파라미터 조절
			ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
			ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
			ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
			ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
			ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
			ImGui::DragFloat("I.cuttoff", &m_light.cutoff.x);
			ImGui::Checkbox("l.Blinn Phong", &m_light.blinn);
		}
		if (ImGui::CollapsingHeader("Terrain", ImGuiTreeNodeFlags_DefaultOpen)) { //지형 파라미터
			ImGui::ColorEdit3("T.ambient", glm::value_ptr(m_terrain_material.ambient));
			ImGui::ColorEdit3("T.diffuse", glm::value_ptr(m_terrain_material.diffuse));
			ImGui::ColorEdit3("T.specular", glm::value_ptr(m_terrain_material.specular));
			ImGui::Checkbox("T.Blinn Phong", &m_terrain_material.blinn);
		}
		if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) { //마테리얼 파라미터 조절
			ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f); 
		}

		if (ImGui::CollapsingHeader("Shadow Map", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Image((ImTextureID)m_shadowMap->GetDepthTexture()->Get(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
		}
	}
	ImGui::End();
	
	perspectiveProjInfo.fov = 45.0f;
	perspectiveProjInfo.width = (float)m_width;
	perspectiveProjInfo.height = (float)m_height;
	perspectiveProjInfo.zNear = 0.01f;
	perspectiveProjInfo.zFar = 400.0f;

	//셰도우 매핑
	//라이트 시점 projection, view 행렬 계산
	OrthoProjInfo orthoInfo;
	glm::vec3 lightPos;
	m_shadowMap->CalcTightLightProjection(camera->getViewMatrix(), m_light.direction, perspectiveProjInfo,
		lightPos, orthoInfo);

	auto lightView = glm::lookAt(lightPos, lightPos + m_light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
	//auto lightProjection = glm::perspective(glm::radians((m_light.cutoff.x + m_light.cutoff.y) * 2.0f), 1.0f, 1.0f, 150.0f);
	auto lightOrtho = glm::ortho(orthoInfo.left, orthoInfo.right, orthoInfo.bottom, orthoInfo.top, 
		orthoInfo.zNear, orthoInfo.zFar);

	//깊이 버퍼에 깊이값 드로우
	m_shadowMap->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POLYGON_OFFSET_FILL);
	float factor = 1.0f;
	float unit = 1.0f;
	glPolygonOffset(factor, unit);
	glViewport(0, 0, m_shadowMap->GetWidth(), m_shadowMap->GetHeight());
	simpleProgram->Use();
	simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderScene(simpleProgram.get(), lightOrtho, lightView);
	RenderTerrain(simpleProgram.get(), lightOrtho, lightView);
	m_shadowMap->BindToDefault();
	glViewport(0, 0, m_width, m_height);
	glDisable(GL_POLYGON_OFFSET_FILL);

	//m_frameBuffer에 연결된 텍스처에 렌더링시작
	m_frameBuffer->Bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	
	auto view = camera->getViewMatrix();
	auto projection = glm::perspective(glm::radians(perspectiveProjInfo.fov), 
									   perspectiveProjInfo.width / perspectiveProjInfo.height, 
									   perspectiveProjInfo.zNear, 
									   perspectiveProjInfo.zFar);


	//광원큐브 그리기
	//===================================================================================================================================
	simpleProgram->Use(); //광원을 그릴 프로그램 사용
	simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	auto lightModel = glm::translate(glm::mat4(1.0f), m_light.position);
	lightModel = glm::scale(lightModel, glm::vec3(0.1f, 0.1f, 0.1f));
	auto transform = projection * view * lightModel;
	simpleProgram->SetUniform("transform", transform);
	m_box->Draw(simpleProgram.get());
	//===================================================================================================================================

	lightProgram->Use();
	lightProgram->SetUniform("cameraPos", camera->getCameraPos());
	lightProgram->SetUniform("lightTransform", lightOrtho * lightView);
	glActiveTexture(GL_TEXTURE3);
	m_shadowMap->GetDepthTexture()->Bind();
	lightProgram->SetUniform("shadowMap", 3);
	SetLightUniform_Dir(lightProgram.get(), m_light.direction);
	//SetLightUniform_Point(lightProgram.get());
	glActiveTexture(GL_TEXTURE0);

	RenderScene(lightProgram.get(), projection, view); //씬 드로우

	//Skeletal animation Testing
	//==================================================================================================================
	
	m_animator->UpdateAnimation(deltaTime);
	
	skeletalProgram->Use();
	const auto& animationTransforms = m_animator->GetFinalBoneMatrices();
	for (int i = 0; i < animationTransforms.size(); ++i)
		skeletalProgram->SetUniform(("finalBonesMatrices[" + std::to_string(i) + "]").c_str(), animationTransforms[i]);

	auto modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.0f, 2.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
	transform = projection * view * modelTransform;
	skeletalProgram->SetUniform("transform", transform);
	//skeletalProgram->SetUniform("modelTransform", modelTransform);
	m_vampire->Draw(skeletalProgram.get());
	
	//==================================================================================================================


	terrainProgram->Use();
	terrainProgram->SetUniform("lightTransform", lightOrtho * lightView);
	glActiveTexture(GL_TEXTURE2);
	m_shadowMap->GetDepthTexture()->Bind();
	terrainProgram->SetUniform("shadowMap", 2);
	RenderTerrain(terrainProgram.get(), projection, view); //지형 드로우
	glActiveTexture(GL_TEXTURE0);

	RenderSkyBox(skyboxProgram.get(), projection, view); //스카이박스 드로우

	//디폴트 프레임버퍼로 전환 및 m_frameBuffer 텍스처가 입혀진 plane 그리기
	m_frameBuffer->BindToDefault();
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	postProgram->Use();
	transform = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
	postProgram->SetUniform("transform", transform);
	m_frameBuffer->GetColorAttatchment()->Bind();
	postProgram->SetUniform("tex", 0);
	postProgram->SetUniform("gamma", m_gamma);
	m_plane->Draw(postProgram.get());
}

context::~context() {
	SPDLOG_INFO("Context Deleted");
}

void context::SetLightUniform_Dir(const ShaderProgram* program, const glm::vec3& lightDir) {
	program->SetUniform("directionalLight.ambient", m_light.ambient);
	program->SetUniform("directionalLight.diffuse", m_light.diffuse);
	program->SetUniform("directionalLight.specular", m_light.specular);
	program->SetUniform("directionalLight.direction", lightDir);
	program->SetUniform("material.shininess", m_material.shininess);
	program->SetUniform("blinn", m_light.blinn ? 1 : 0);
}

void context::SetLightUniform_Point(const ShaderProgram* program) {
	program->SetUniform("pointLight[0].ambient", m_light.ambient);
	program->SetUniform("pointLight[0].diffuse", m_light.diffuse);
	program->SetUniform("pointLight[0].specular", m_light.specular);
	program->SetUniform("pointLight[0].constant", 1.0f);
	program->SetUniform("pointLight[0].linear", 0.09f);
	program->SetUniform("pointLight[0].quadratic", 0.032f);
	program->SetUniform("pointLight[0].position", m_light.position);
	program->SetUniform("material.shininess", m_material.shininess);
	program->SetUniform("blinn", m_light.blinn ? 1 : 0);
}

void context::SetLightUniform_Spot(const ShaderProgram* program, const glm::vec3& position, const glm::vec3& direction) {
	program->SetUniform("spotLight.ambient", m_light.ambient);
	program->SetUniform("spotLight.diffuse", m_light.diffuse);
	program->SetUniform("spotLight.specular", m_light.specular);
	program->SetUniform("spotLight.ambient", m_light.ambient);
	program->SetUniform("spotLight.constant", 1.0f);
	program->SetUniform("spotLight.linear", 0.027f);
	program->SetUniform("spotLight.quadratic", 0.0028f);
	program->SetUniform("spotLight.position", position);
	program->SetUniform("spotLight.direction", direction);
	program->SetUniform("spotLight.cutoff", glm::cos(glm::radians(m_light.cutoff.x)));
	program->SetUniform("spotLight.cutoffOuter", glm::cos(glm::radians(m_light.cutoff.x + m_light.cutoff.y)));
	program->SetUniform("material.shininess", m_material.shininess);
	program->SetUniform("blinn", m_light.blinn ? 1 : 0);
}

void context::RenderScene(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view) {
	auto modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(20.0f, 1.0f, 20.0f));
	auto transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_floor->Draw(program);

	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.75f, -4.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_box->Draw(program);

	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.75f, 2.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_box->Draw(program);

	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 4.0f, -5.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_box->Draw(program);

	modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 2.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_backpack->Draw(program);
	
	
	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_brickwall->Draw(program);
	
}

void context::RenderSkyBox(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view) {
	//skybox 그리기
	glDepthFunc(GL_LEQUAL);
	program->Use();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	program->SetUniform("transform", transform);
	m_cubeTexture->Bind();
	program->SetUniform("skybox", 0);
	m_box->Draw(program);
	glDepthFunc(GL_LESS);

	//skybox 그리기 2번째 방법
	/*
	program->Use();
	modelTransform = glm::translate(glm::mat4(1.0f), camera->getCameraPos()) *
		glm::scale(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 50.0f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	m_cubeTexture->Bind();
	skyboxProgram->SetUniform("skybox", 0);
	m_box->Draw(program);
	*/
}

void context::RenderGrass(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view) {
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	instancingProgram->Use();
	auto transform = projection * view;
	auto offset = glm::mat4(1.0f);
	instancingProgram->SetUniform("transform", transform);
	m_grassTexture->Bind();
	lightProgram->SetUniform("diffuse", 0);
	instanceVao->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, m_grass->GetElemetCount(), GL_UNSIGNED_INT, 0, 10000);
}

void context::RenderTerrain(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view) {
	program->Use();
	auto modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -12.0f, 0.0f));
	auto transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	program->SetUniform("cameraPos", camera->getCameraPos());
	program->SetUniform("directionalLight.ambient", m_terrain_material.ambient);
	program->SetUniform("directionalLight.diffuse", m_terrain_material.diffuse);
	program->SetUniform("directionalLight.specular", m_terrain_material.specular);
	program->SetUniform("directionalLight.direction", m_light.direction);
	program->SetUniform("directionalLight.shininess", m_material.shininess);
	program->SetUniform("blinn", m_terrain_material.blinn ? 1 : 0);
	m_terrain->Draw(program);
}
