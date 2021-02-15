#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"

#include <iostream>

using namespace std;
//Function declaration
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

//Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Camera.h
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Timers
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float fpsShowTimer = 10.0f;

//Light variables setup
float shaderSpecShininess = 64.0f;
glm::vec3 lightSpecularDefault(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular(1.0f, 1.0f, 1.0f);
glm::vec3 lightDiffuseDefault(0.5f,0.5f,0.5f);
glm::vec3 lightDiffuse(0.5f, 0.5f, 0.5f);
glm::vec3 lightAmbientDefault(0.2f, 0.2f, 0.2f);
glm::vec3 lightAmbient(0.2f, 0.2f, 0.2f);
float lightIntensity = 1.0f;
float lightIntensityDefault = 1.0f;
float lightConstant = 0.5f;
float lightConstantDefault = 0.5f;
float lightLinear = 0.5f;
float lightLinearDefault = 0.5f;
float lightQuadratic = 0.005f;
float lightQuadraticDefault = 0.005f;

//Utility variables
bool cubeShouldRotate = false;
bool lightShouldRoam = true; //TODOODODO
bool shouldRenderSkybox = true;
unsigned int nr_lights = 2;
unsigned int activeLightHandler = 0;
float utilityKeybindsTimer = 0.0f;


//Lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

glm::vec3 pointLightPositions[] = {
	glm::vec3(1.2f, 1.0f, 2.0f),
	glm::vec3(0.2f, 1.0f, -2.0f)
};

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	// ------------------------------------
	Shader multipleLightsShader("Lighting_Maps_1/lightingMaps1.vs","Test/multipleLightsShader.fs");
	Shader lampShader("Lighting_1/lamp1.vs","Lighting_1/lamp1.fs");

	Shader shader("Skyboxes_Environment_Mapping/Skybox_with_cubemaps/simpleShader.vs", "Skyboxes_Environment_Mapping/Skybox_with_cubemaps/simpleShader.fs");
	Shader skyboxShader("Skyboxes_Environment_Mapping/Skybox_with_cubemaps/skybox.vs", "Skyboxes_Environment_Mapping/Skybox_with_cubemaps/skybox.fs");

	//TODO: --------------ADD MORE SHADERS-------------------

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	//Positions all containers
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,   0.0f,   0.0f),
		glm::vec3(2.0f,   5.0f,   -15.0f),
		glm::vec3(-1.5f,  -2.2f,  -2.5f),
		glm::vec3(-3.8f,  -2.0f,  -12.3f),
		glm::vec3(2.4f,   -0.4f,  -3.5f),
		glm::vec3(-1.7f,  3.0f,   -7.5f),
		glm::vec3(1.3f,   -2.0f,  -2.5f),
		glm::vec3(1.5f,   2.0f,   -2.5f),
		glm::vec3(1.5f,   0.2f,   -1.5f),
		glm::vec3(-1.3f,  1.0f,   -1.5f)
	};
	// first, configure the cube's VAO (and VBO)
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// load textures (we now use an utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	std::string str = "Lighting_Maps_1/container.png";
	unsigned int diffuseMap = loadTexture(str.c_str());
	std::string str2 = "Lighting_Maps_2/container_specular.png";
	unsigned int specularMap = loadTexture(str2.c_str());

	vector<std::string> faces
	{
		"Skyboxes_Environment_Mapping/skybox_faces/skybox/right.jpg",
		"Skyboxes_Environment_Mapping/skybox_faces/skybox/left.jpg",
		"Skyboxes_Environment_Mapping/skybox_faces/skybox/top.jpg",
		"Skyboxes_Environment_Mapping/skybox_faces/skybox/bottom.jpg",
		"Skyboxes_Environment_Mapping/skybox_faces/skybox/front.jpg",
		"Skyboxes_Environment_Mapping/skybox_faces/skybox/back.jpg",
	};
	unsigned int cubemapTexture = loadCubemap(faces);

	// shader configuration
	// --------------------
	multipleLightsShader.use();
	multipleLightsShader.setInt("material.diffuse", 0);
	multipleLightsShader.setInt("material.specular", 1);

	// skybox shader configuration
	// --------------------
	shader.use();
	shader.setInt("texture1", 0);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	//fps counter setup
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);  //Clearing the color always to a default value at the start of our loop.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//
		if (lightShouldRoam) 
		{
			pointLightPositions[0].x = 1.0f + sin(glfwGetTime()) * 2.0f;
			pointLightPositions[1].y = sin(glfwGetTime() / 2.0f) * 1.0f;
		}
		
		multipleLightsShader.use();
		multipleLightsShader.setVec3("viewPos", camera.Position);
		// material properties
		multipleLightsShader.setFloat("material.shininess", shaderSpecShininess);

		for (GLuint i = 0; i < nr_lights; i++)                          //Loading shader params and light positions.
		{
			string number = to_string(i);
			multipleLightsShader.setVec3("lights[" + number + "].position", pointLightPositions[i]);

			//Light properties that can get manipulated by the user.
			multipleLightsShader.setVec3("lights[" + number + "].ambient", lightAmbient);  
			multipleLightsShader.setVec3("lights[" + number + "].diffuse", lightDiffuse);
			multipleLightsShader.setVec3("lights[" + number + "].specular", lightSpecular);
			multipleLightsShader.setFloat("lights[" + number + "].intensity", lightIntensity);
			multipleLightsShader.setFloat("lights[" + number + "].constant", lightConstant);
			multipleLightsShader.setFloat("lights[" + number + "].linear", lightLinear);
			multipleLightsShader.setFloat("lights[" + number + "].quadratic", lightQuadratic);
		}

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		multipleLightsShader.setMat4("projection", projection);
		multipleLightsShader.setMat4("view", view);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// Draw 10 containers with the same VAO and VBO information; only their world space coordinates differ
		
		glBindVertexArray(cubeVAO);   
		for (GLuint i = 0; i < 10; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			if (cubeShouldRotate)                                   //If enabled cubes rotate every frame.
			{
				model = glm::rotate(model, (float)glm::radians(glm::sin((float)glfwGetTime()) * 90), glm::vec3(0.0f, 0.0f, 1.0f));
			}
			multipleLightsShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);   //actual draw func.
		}
		glBindVertexArray(0);


		// also draw the lamp models
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		glBindVertexArray(lightVAO);  //same VAO for all lights.

		for (GLuint i = 0; i < nr_lights; i++) 
		{
			glm::mat4 lightModel = glm::mat4(1.0f);
			lightModel = glm::translate(lightModel, pointLightPositions[i]);
			lightModel = glm::scale(lightModel, glm::vec3(0.3f)); // a smaller cube.
			lampShader.setMat4("model", lightModel);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// draw skybox as last
		if (shouldRenderSkybox) 
		{
			glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
			skyboxShader.use();
			view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
			skyboxShader.setMat4("view", view);
			skyboxShader.setMat4("projection", projection);
			// skybox cube
			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			glDepthFunc(GL_LESS); // set depth function back to default
		}
				

		// Measure speed (FPS counter) on console.
		nbFrames++;

		if (currentFrame - lastTime >= 1.0f)
		{ // If last prinf() was more than 1 sec ago
			// printf and reset timer
			if (fpsShowTimer >= 10.0f)
			{
				printf("%f frames per second.\n", (float)nbFrames);
				printf("%f ms/frame.\n", 1000.0 / double(nbFrames)); // ms/frame is more accurate
				fpsShowTimer = 0.0f;
			}
			nbFrames = 0;
			lastTime += 1.0f;
			fpsShowTimer += 1.0f;
		}

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	float currentFr = glfwGetTime();
	float timeBetweenUtilityKeybindCalls = currentFr - utilityKeybindsTimer;

	if (timeBetweenUtilityKeybindCalls >= 0.1f)  //This way we manage to get up to 10 inputs per sec.
	{
		utilityKeybindsTimer = currentFr;

		//Utility function(s)
		if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			shouldRenderSkybox = !shouldRenderSkybox;
			if (shouldRenderSkybox) 
			{
				printf("Enabled skybox rendering. \n");
			}
			else 
			{
				printf("Disabled skybox rendering. \n");
			}
		}
		if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		{
			lightShouldRoam = !lightShouldRoam;
			if (lightShouldRoam)
			{
				printf("Enabled lights roaming. \n");
			}
			else
			{
				printf("Disabled lights roaming. \n");
			}
		}
		//Object material property keybinds
		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) //TODO: FIND WORK ARROUND TO INCREASE/DECREASE EVERY X FRAMES or set key callback.
		{
			shaderSpecShininess -= 0.5f;
			if (shaderSpecShininess <= 0.0f)
			{
				shaderSpecShininess = 0.0f;
			}
			printf("Decreasing object shininess to: %f \n", shaderSpecShininess);
		}
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			shaderSpecShininess += 0.5f;
			if (shaderSpecShininess >= 128.0f)
			{
				shaderSpecShininess = 128.0f;
			}
			printf("Increasing object shininess to: %f \n", shaderSpecShininess);
		}
		//Light Property Keybinds
		if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
		{
			lightIntensity += 0.2f;
			printf("Increasing Light Intensity to: %f \n", lightIntensity);
		}
		if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
		{
			lightIntensity -= 0.2f;
			printf("Decreasing Light Intensity to: %f \n", lightIntensity);
		}
		//SPECULAR (MATTE APPEARANCE)
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			lightSpecular = glm::vec3(0.0f, 0.0f, 0.0f);  //Reducing specular to 0 provides a matte appearance on our object(s).
			printf("Enabled matte appearance by reducing specular properties to 0. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		{
			lightSpecular = lightSpecularDefault;
			printf("Restored specular lighting properties. \n");
		}
		//DIFFUSE
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			lightDiffuse -= glm::vec3(0.1f, 0.1f, 0.1f);
			if (lightDiffuse.x <= 0.0f)
			{
				lightDiffuse = glm::vec3(0.0f, 0.0f, 0.0f);
			}
			printf("Decreasing light diffuse to: %f \n", lightDiffuse);
		}
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			lightDiffuse += glm::vec3(0.1f, 0.1f, 0.1f);
			if (lightDiffuse.x >= 4.0f)
			{
				lightDiffuse = glm::vec3(4.0f, 4.0f, 4.0f);
			}
			printf("Increasing light diffuse to: %f \n", lightDiffuse);
		}
		//CUBE OBJECT KEYBINDS
		if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
		{
			cubeShouldRotate = true;
			printf("Rotating the cubes: ON \n");
		}
		if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
		{
			cubeShouldRotate = false;
			printf("Rotating the cubes: OFF \n");
		}
		//LIGHT OBJECT KEYBINDS
		if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS) 
		{
			activeLightHandler = 0;  //This is used to change which light we move with keypad buttons 4-5-6-8.
		}
		if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
		{
			activeLightHandler = 1;
		}
		if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
		{
			pointLightPositions[activeLightHandler] -= glm::vec3(0.1f, 0.0f, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
		{
			pointLightPositions[activeLightHandler] += glm::vec3(0.1f, 0.0f, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_5) == GLFW_PRESS)
		{
			pointLightPositions[activeLightHandler] -= glm::vec3(0.0f, 0.1f, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
		{
			pointLightPositions[activeLightHandler] += glm::vec3(0.0f, 0.1f, 0.0f);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_7) == GLFW_PRESS) //RESET LIGHT POS
		{
			pointLightPositions[0] = glm::vec3(1.2f, 1.0f, 2.0f);
			pointLightPositions[1] = glm::vec3(0.2f, 1.0f, -2.0f);
		}
		//LIGHTING SETUPS (GOOD LUCK HAVE FUN)
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) //MATTE
		{
			lightAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
			lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			lightSpecular = glm::vec3(0.0f, 0.0f, 0.0f);
			lightConstant = 1.0f;
			lightLinear = 0.20f;
			lightQuadratic = 0.050f;
			shouldRenderSkybox = false;
			printf("Enabled the matte scenario. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) //ROUGH
		{
			lightAmbient = glm::vec3(0.14f, 0.14f, 0.14f);
			lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			lightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
			lightConstant = 1.0f;
			lightLinear = 0.1f;
			lightQuadratic = 0.015f;
			shouldRenderSkybox = false;
			printf("Enabled the rough scenario. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) //GLOSSY
		{
			lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
			lightDiffuse = glm::vec3(0.5f, 0.5f, 0.5f);
			lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
			lightConstant = 1.0f;
			lightLinear = 0.09f;
			lightQuadratic = 0.1f;
			shouldRenderSkybox = false;
			printf("Enabled the glossy scenario. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) //DESERT
		{
			lightAmbient = glm::vec3(0.3f, 0.24f, 0.14f);
			lightDiffuse = glm::vec3(0.7f, 0.42f, 0.26f);
			lightSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
			lightConstant = 1.0f;
			lightLinear = 0.09f;
			lightQuadratic = 0.032f;
			shouldRenderSkybox = false;
			printf("Enabled the desert scenario. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) //FACTORY
		{
			lightAmbient = glm::vec3(0.05f, 0.05f, 0.1f);
			lightDiffuse = glm::vec3(0.2f, 0.2f, 0.7f);
			lightSpecular = glm::vec3(0.7f, 0.7f, 0.7f);
			lightConstant = 1.0f;
			lightLinear = 0.09f;
			lightQuadratic = 0.032f;
			shouldRenderSkybox = false;
			printf("Enabled the factory scenario. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) //BIOCHEMICAL LAB (To actually manage this one we need glClearColor custom values and more lights)
		{
			lightAmbient = glm::vec3(0.5f, 0.5f, 0.5f);
			lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			lightSpecular = glm::vec3(1.0f, 1.0f, 1.0f);
			lightConstant = 1.0f;
			lightLinear = 0.07f;
			lightQuadratic = 0.017f;
			shouldRenderSkybox = false;
			printf("Enabled the biochemical failure scenario. \n");
		}
		if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS)  //RESET ALL
		{
			lightAmbient = lightAmbientDefault;
			lightSpecular = lightSpecularDefault;
			lightDiffuse = lightDiffuseDefault;
			lightIntensity = lightIntensityDefault;
			lightConstant = lightConstantDefault;
			lightLinear = lightLinearDefault;
			lightQuadratic = lightQuadraticDefault;
			pointLightPositions[0] = glm::vec3(1.2f, 1.0f, 2.0f);
			pointLightPositions[1] = glm::vec3(0.2f, 1.0f, -2.0f);
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}
