#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<imgui/imgui.h>
#include<imgui/imgui_impl_glfw.h>
#include<imgui/imgui_impl_opengl3.h>

#include"Cloud.h"
#include"Shader.h"

const int SCR_WIDTH = 1600;
const int SCR_HEIGHT = 900;

Camera camera(glm::vec3(0.0f, 0.0f, 100.0f));

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
float cameraNearPlane = 0.1f;
float cameraFarPlane = 1000.0f;

bool swimMode = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void renderQuad();
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Volume_Cloud", nullptr, nullptr);

	if (window == NULL)
	{
		std::cerr << "error to create window";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "load glad error";
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	Cloud cloud(SCR_WIDTH, SCR_HEIGHT, cameraNearPlane, cameraFarPlane);

	Shader sceneShader("shader/main.vs", "shader/main.fs");
	sceneShader.use();
	sceneShader.setInt("cloud", 0);

	float absorption = 0.055;
	float coverage = 1;
	float crispiness = 2;
	float curliness = 1;
	float density = 0.05;
	float speed = 10.0;
	float type = 1.0;

	float lightCol[3] = { 1 };

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		glClearColor(0.5, 0.6, 0.7, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cloud.render(camera);

		sceneShader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cloud.getCloudTexture());
		renderQuad();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_None);
		ImGui::SetNextWindowSize(ImVec2(float(400), float(700)), ImGuiCond_None);
		{
			ImGui::Begin("Object", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);

			if(ImGui::DragFloat("light absorption", &absorption, 0.001f, 0.0f, 1.0f, "%.3f"))
				cloud.setAbsorption(absorption);
			if(ImGui::DragFloat("cloud coverage", &coverage, 0.1f, 0.0f, 1.0f, "%.1f"))
				cloud.setCoverage(coverage);
			if (ImGui::DragFloat("cloud crispiness", &crispiness, 1.0f, 0.0f, 100.0f))
				cloud.setCrispiness(crispiness);
			if (ImGui::DragFloat("cloud curliness", &curliness, 0.001f, 0.0f, 1.0f, "%.3f"))
				cloud.setCurliness(curliness);
			if (ImGui::DragFloat("cloud density", &density, 0.001f, 0.0f, 1.0f, "%.3f"))
				cloud.setDensity(density);
			if (ImGui::DragFloat("cloud speed", &speed, 1.0f, 0.0f, 100.0f))
				cloud.setSpeed(speed);
			if (ImGui::DragFloat("cloud type", &type, 0.1, 0.0f, 1.0f, "%.1f"))
				cloud.setType(type);

			ImGui::NewLine();
			if (ImGui::ColorEdit3("Light Color", lightCol))
				cloud.setLightColor(glm::vec3(lightCol[0], lightCol[1], lightCol[2]));

			ImGui::End();
		}
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && swimMode)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (swimMode) {
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		swimMode = !swimMode;
		glfwSetInputMode(window, GLFW_CURSOR, swimMode ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int quadVAO = 0;
void renderQuad()
{
	if (quadVAO == 0)
	{
		unsigned int quadVBO;
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

