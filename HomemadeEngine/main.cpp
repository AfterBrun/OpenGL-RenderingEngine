#include "common.h"
#include "context.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"



//������ ���� ����
//======================================================================================================================================
#define WINDOW_WIDTH 800 //������ ũ��
#define WINDOW_HEIGHT 600
const char* winName = "HomeMade Engine"; //������ �̸� ����
//======================================================================================================================================


//�ݹ��Լ� ���� ����
//======================================================================================================================================
void OnFrameBufferSizeChange(GLFWwindow* window, int width, int height);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
//======================================================================================================================================

int main() {
	SPDLOG_INFO("start program");
	SPDLOG_INFO("Initialize GLFW");
	if (!glfwInit()) {
		const char* failedInfo = nullptr;
		glfwGetError(&failedInfo);
		SPDLOG_INFO("Initialization failed : {}", failedInfo);
		return -1;
	} //OpenGL �ʱ�ȭ

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//OpenGL���� ��Ʈ ����


	SPDLOG_INFO("Create Window");
	auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, winName, nullptr, nullptr);
	if (!window) {
		SPDLOG_INFO("Failed to create Window");
		glfwTerminate();
		return -1;
	} //OpenGL ������ ����
	glfwMakeContextCurrent(window); //���� ����� ������ ���ؽ�Ʈ
	//glfwSwapInterval(0); //���� ����ȭ off


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		SPDLOG_INFO("Glad load failed");
		return -1;
	} //Glad �Լ� �ε�

	auto glVersion = glGetString(GL_VERSION);
	SPDLOG_INFO("OpenGL Context Version: {}", (const char*)glVersion); //OpenGL ���ؽ�Ʈ ���� ǥ��

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, false);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();
	//ImGui_ImplGlfw_InstallCallbacks(window);

	std::unique_ptr<context> Context = context::Create(); //context ����
	if (!Context) { return -1; }

	glfwSetWindowUserPointer(window, Context.get()); //user pointer�� ����

	//callback�Լ� ���
	//=================================================================================================================================
	OnFrameBufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
	glfwSetFramebufferSizeCallback(window, OnFrameBufferSizeChange);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetCursorPosCallback(window, mouseCursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	//==================================================================================================================================

	
	SPDLOG_INFO("Start Main Loop");
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow(); // Show demo window! :)

		Context->Render();
		Context->keyEvent(window);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
	}//���η���

	Context.reset();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();										
	SPDLOG_INFO("OpenGL Window Terminated");
	return 0;
}

void OnFrameBufferSizeChange(GLFWwindow* window, int width, int height)
{
	SPDLOG_INFO("Width: {}, Height: {}", width, height);
	auto pointer = (context*)glfwGetWindowUserPointer(window);
	pointer->viewSizeChange(width, height);
}

void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
		key, scancode,
		action == GLFW_PRESS ? "Pressed" :
		action == GLFW_RELEASE ? "Released" :
		action == GLFW_REPEAT ? "Repeat" : "Unknown",
		mods & GLFW_MOD_CONTROL ? "C" : "-",
		mods & GLFW_MOD_SHIFT ? "S" : "-",
		mods & GLFW_MOD_ALT ? "A" : "-");
	if (key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, true);
	}
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

void mouseCursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	auto pointer = (context*)glfwGetWindowUserPointer(window);
	pointer->mouseCursorEvent(xpos, ypos);
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	auto pointer = (context*)glfwGetWindowUserPointer(window);
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	pointer->mouseButtonEvent(button, action, x, y);
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}
