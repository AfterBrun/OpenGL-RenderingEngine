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
	//frame buffer ����
	if (width == 0 && height == 0) { //������ �ּ�ȭ ������� 0X0 �� �ƴ� �ּ�ũ���� �������۹� ����
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
	//program ����
	simpleProgram = ShaderProgram::CreateShaderProgram("./shader/simple.vs", "./shader/simple.fs");
	postProgram = ShaderProgram::CreateShaderProgram("./shader/post.vs", "./shader/post.fs");
	//lightProgram = ShaderProgram::CreateShaderProgram("./shader/light.vs", "./shader/light.fs");
	lightProgram = ShaderProgram::CreateShaderProgram("./shader/shadowMapping.vs", "./shader/shadowMapping.fs");
	skyboxProgram = ShaderProgram::CreateShaderProgram("./shader/skybox.vs", "./shader/skybox.fs");
	instancingProgram = ShaderProgram::CreateShaderProgram("./shader/instancing.vs", "./shader/instancing.fs");
	terrainProgram = ShaderProgram::CreateShaderProgram("./shader/terrain.vs", "./shader/terrain.fs");
	normalProgram = ShaderProgram::CreateShaderProgram("./shader/normal.vs", "./shader/normal.fs");


	//program ���� ���� ����
	if (!lightProgram || !postProgram || !lightProgram || !skyboxProgram || !instancingProgram || !terrainProgram ||
		!normalProgram) {
		return false;
	}

	//�޽� ��ü ����
	m_box = Mesh::CreateBox();
	m_floor = Mesh::CreateBox();
	m_plane = Mesh::CreatePlane();
	m_grass = Mesh::CreatePlane();
	m_brickwall = Mesh::CreatePlane();

	
	//m_chair = Model::LoadModel("./asset/model/chair/chair.fbx");
	m_backpack = Model::LoadModel("./asset/model/backpack/backpack.obj");

	std::unique_ptr container2 = image::CreateFromFile("./asset/texture/container2.png");			//�̹��� ����
	std::unique_ptr container2_metal = image::CreateFromFile("./asset/texture/container2_specular.png");
	std::unique_ptr floorImage = image::CreateFromFile("./asset/texture/marble.jpg");
	std::unique_ptr brickwall_texture = image::CreateFromFile("./asset/texture/brickwall.jpg");
	std::unique_ptr brickwall_normal = image::CreateFromFile("./asset/texture/brickwall_normal.jpg");
	

	//��ī�̹ڽ� �ʱ�ȭ
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

	//�ν��Ͻ��� ���� Ǯ�� �ؽ�ó �� vao, vbo, ebo ����
	m_grassTexture = texture::CreateFromImage(image::CreateFromFile("./asset/texture/grass.png").get());

	m_grassPos.resize(10000);
	for (size_t i = 0; i < m_grassPos.size(); i++) {
		m_grassPos[i].x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
		m_grassPos[i].z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * 5.0f;
		m_grassPos[i].y = glm::radians((float)rand() / (float)RAND_MAX * 360.0f);
	}
	instanceVao = vertexLayout::CreateVertexLayout();
	instanceVao->Bind();
	m_grass->GetVertexBuffer()->Bind();
	instanceVao->SetAttributePointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	instanceVao->SetAttributePointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, Normal));
	instanceVao->SetAttributePointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, TexCoord));

	instanceVbo = buffer::CreateBuffer(GL_ARRAY_BUFFER, m_grassPos.data(), size_t(m_grassPos.size() * sizeof(glm::vec3)),
		                               GL_STATIC_DRAW);
	instanceVbo->Bind();
	instanceVao->SetAttributePointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glVertexAttribDivisor(3, 1);
	m_grass->GetElementBuffer()->Bind();


	//���� �޽� ���׸��� ����
	m_box->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(container2.get(), "texture_diffuse"));
	m_box->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(container2_metal.get(), "texture_specular"));
	m_floor->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(floorImage.get(), "texture_diffuse"));
	m_floor->GetMaterialArrayPtr()->push_back(texture::CreateFromImage(floorImage.get(), "texture_diffuse"));

	m_brickDiffuseTexture = texture::CreateFromImage(brickwall_texture.get(), "texture_diffuse");
	m_brickNormalTexture = texture::CreateFromImage(brickwall_normal.get(), "texture_normal");
	
	//���� �ʱ�ȭ
	m_terrain = Terrain::CreateWithHeightMap("./asset/texture/heightmap4.jpg");
	m_terrain->SetTexture({ image::CreateFromFile("./asset/texture/terrain2.jpg").get(),
							image::CreateFromFile("./asset/texture/terrain0.jpg").get(),
							image::CreateFromFile("./asset/texture/terrain1.jpg").get(),
							image::CreateFromFile("./asset/texture/terrain3.png").get() });

	//ī�޶� ��ü ����
	camera = camera::Create(cameraPos, cameraFront, cameraUp);

	//������ �� ����
	m_shadowMap = ShadowMap::Create(8192, 8192);

	return true;
}

void context::Render() {
	static double previous = glfwGetTime();
	double current = glfwGetTime();
	deltaTime = (float)current - (float)previous;
	previous = current;

	//������ ����
	if (ImGui::Begin("Configure UI")) {
		ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor));
		ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
		ImGui::DragFloat3("Camera Position", camera->CameraPosPtr(), 0.01f);
		ImGui::Separator();
		if (ImGui::CollapsingHeader("Light Configure", ImGuiTreeNodeFlags_DefaultOpen)) { //�� �Ķ���� ����
			ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
			ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
			ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient));
			ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse));
			ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular));
			ImGui::DragFloat("I.cuttoff", &m_light.cutoff.x);
			ImGui::Separator();
			ImGui::Checkbox("Blinn Phong", &m_light.blinn);
		}
		if (ImGui::CollapsingHeader("material", ImGuiTreeNodeFlags_DefaultOpen)) { //���׸��� �Ķ���� ����
			ImGui::DragFloat("m.shininess", &m_material.shininess, 1.0f, 1.0f, 256.0f); 
		}
		if (ImGui::CollapsingHeader("Terrain", ImGuiTreeNodeFlags_DefaultOpen)) { //���� �Ķ����
			ImGui::DragFloat("y Scale", &yScale, 1.0f, 1.0f, 1256.0f); // ���� ���� ������ ����(�󸶳� ������ ���ĸ���)
			ImGui::DragFloat("y Shift", &yShift, 1.0f, 1.0f, 256.0f); // ���� ���� ����
		}

		if (ImGui::CollapsingHeader("Shadow Map", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Image((ImTextureID)m_shadowMap->GetDepthTexture()->Get(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
		}
	}
	ImGui::End();
	

	//�ε��� ����
	//����Ʈ ���� projection, view ��� ���
	auto lightView = glm::lookAt(m_light.position, m_light.position + m_light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
	auto lightProjection = glm::perspective(glm::radians((m_light.cutoff.x + m_light.cutoff.y) * 2.0f), 1.0f, 1.0f, 150.0f);
	auto lightOrtho = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f);
	//auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 30.0f);

	//���� ���ۿ� ���̰� ��ο�
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
	m_shadowMap->BindToDefault();
	glViewport(0, 0, m_width, m_height);
	glDisable(GL_POLYGON_OFFSET_FILL);

	//m_frameBuffer�� ����� �ؽ�ó�� ����������
	m_frameBuffer->Bind();
	glEnable(GL_DEPTH_TEST);
	glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	
	auto view = camera->getViewMatrix();
	auto projection = glm::perspective(glm::radians(45.0f), (float)m_width / (float)m_height, 0.01f, 1000.0f);


	//����ť�� �׸���
	//===================================================================================================================================
	simpleProgram->Use(); //������ �׸� ���α׷� ���
	simpleProgram->SetUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	auto lightModel = glm::translate(glm::mat4(1.0f), m_light.position);
	lightModel = glm::scale(lightModel, glm::vec3(0.1f, 0.1f, 0.1f));
	auto transform = projection * view * lightModel;
	simpleProgram->SetUniform("transform", transform);
	m_box->Draw(simpleProgram.get());
	//===================================================================================================================================
	/*
	auto modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	normalProgram->Use();
	normalProgram->SetUniform("viewPos", camera->getCameraPos());
	normalProgram->SetUniform("lightPos", m_light.position);
	glActiveTexture(GL_TEXTURE0);
	m_brickDiffuseTexture->Bind();
	normalProgram->SetUniform("diffuse", 0);
	glActiveTexture(GL_TEXTURE1);
	m_brickNormalTexture->Bind();
	normalProgram->SetUniform("normalMap", 1);
	glActiveTexture(GL_TEXTURE0);
	normalProgram->SetUniform("modelTransform", modelTransform);
	normalProgram->SetUniform("transform", projection * view * modelTransform);
	m_brickwall->Draw(normalProgram.get());
	*/

	lightProgram->Use();
	lightProgram->SetUniform("cameraPos", camera->getCameraPos());
	lightProgram->SetUniform("lightTransform", lightOrtho * lightView);
	glActiveTexture(GL_TEXTURE3);
	m_shadowMap->GetDepthTexture()->Bind();
	lightProgram->SetUniform("shadowMap", 3);
	//SetLightUniform_Dir(lightProgram.get(), m_light.direction);
	SetLightUniform_Point(lightProgram.get());
	glActiveTexture(GL_TEXTURE0);

	RenderScene(lightProgram.get(), projection, view); //�� ��ο�
	RenderTerrain(terrainProgram.get(), projection, view); //���� ��ο�
	RenderSkyBox(skyboxProgram.get(), projection, view); //��ī�̹ڽ� ��ο�

	//����Ʈ �����ӹ��۷� ��ȯ �� m_frameBuffer �ؽ�ó�� ������ plane �׸���
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
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_backpack->Draw(program);

	modelTransform =
		glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 3.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glActiveTexture(GL_TEXTURE0);
	m_brickDiffuseTexture->Bind();
	program->SetUniform("material.texture_diffuse1", 0);
	glActiveTexture(GL_TEXTURE1);
	m_brickDiffuseTexture->Bind();
	program->SetUniform("material.texture_specular1", 1);
	glActiveTexture(GL_TEXTURE2);
	m_brickNormalTexture->Bind();
	program->SetUniform("material.texture_normal1", 2);
	glActiveTexture(GL_TEXTURE0);
	transform = projection * view * modelTransform;
	program->SetUniform("transform", transform);
	program->SetUniform("modelTransform", modelTransform);
	m_brickwall->Draw(program);
}

void context::RenderSkyBox(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view) {
	//skybox �׸���
	glDepthFunc(GL_LEQUAL);
	program->Use();
	auto skyboxView = glm::mat4(glm::mat3(view));
	auto transform = projection * skyboxView;
	program->SetUniform("transform", transform);
	m_cubeTexture->Bind();
	program->SetUniform("skybox", 0);
	m_box->Draw(program);
	glDepthFunc(GL_LESS);

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
	program->SetUniform("directionalLight.ambient", m_light.ambient);
	program->SetUniform("directionalLight.diffuse", m_light.diffuse);
	program->SetUniform("directionalLight.specular", m_light.specular);
	program->SetUniform("directionalLight.direction", m_light.direction);
	program->SetUniform("directionalLight.shininess", m_material.shininess);
	program->SetUniform("blinn", m_light.blinn ? 1 : 0);
	m_terrain->Draw(program, yScale, yShift);
}