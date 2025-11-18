#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "./header/Object.h"
#include "./header/stb_image.h"

struct VertexAttribute {
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

using namespace std;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int createShader(const string &filename, const string &type);
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader);
unsigned int modelVAO(Object &model);
unsigned int loadTexture(const char *tFileName);
string readTextFile(const string &filename);
void init();

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// Shader
unsigned int vertexShader, fragmentShader, shaderProgram;

// Texture
unsigned int groundTexture, fishTexture, columnTexture;

// VAO, VBO
unsigned int groundVAO, fishVAO, columnVAO;

// Objects to display
Object *groundObject, *fishObject, *columnObject;

// Constants you may need
const int rotateColumnSpeed = 10;
const int revolveFishSpeed = 20;
const int rotateFishSpeed = 180;
const float fishColumnDist = 3;
const float swayHeight = 3;
const int squeezeSpeed = 90;
const float breathePeriod = 3;

// Variables you can use
float rotateColumnDegree = 0;
float revolveFishDegree = 0;
float rotateFishDegree = 0; 
float breatheTime = 0;

bool useSelfRotation = false;
bool useSqueeze = false;
float squeezeFactor = 0;
bool useBreathing = false; 
float intensity = 1.0;
bool useGradient = false;
glm::vec3 breathingColor = glm::vec3(1.0f, 1.0f, 0.0f);

int main() {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    /* Done#0: Change window title to "HW2 - [your student id]"
     *        Ex. HW2-112550000
     */

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "HW2 - 112550033", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Done#1: Finish function createShader
    // Done#2: Finish function createProgram
    // Done#3: Finish function modelVAO
    // Done#4: Finish function loadTexture
    // You can find the above functions right below the main function

    // Initialize Object, Shader, Texture, VAO, VBO
    init();

    // Enable depth test, face culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // render loop variables
    double dt;
    double lastTime = glfwGetTime();
    double currentTime;

    /* Done#5: Data connection - Retrieve uniform variable locations
     *    1. Retrieve locations for model, view, and projection matrices.
     *    2. Retrieve locations for squeezeFactor, breathingColor, intensity, and other parameters.
     * Hint:
     *    glGetUniformLocation
     */
	
	GLint viewLocation = glGetUniformLocation(shaderProgram, "view");
	GLint projectionLocation = glGetUniformLocation(shaderProgram, "projection");
	GLint modelLocation = glGetUniformLocation(shaderProgram, "model");

	GLint squeezeFactorLocation = glGetUniformLocation(shaderProgram, "squeezeFactor");
	GLint intensityLocation = glGetUniformLocation(shaderProgram, "intensity");
	GLint breathingColorLocation = glGetUniformLocation(shaderProgram, "breathingColor");
	GLint useGradientLocation = glGetUniformLocation(shaderProgram, "useGradient");

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // render color of water
        glClearColor(0.15, 0.50, 0.65, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 8.5f, 13.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

        glm::mat4 groundModel(1.0f), fishModel(1.0f), columnModel(1.0f);

        /* Done#6-1: Render Ground
         *    1. Set up ground model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         *    4. Apply the texture, and render the ground.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */
		
		groundModel = glm::translate(groundModel, glm::vec3(0, -5, 8));
		groundModel = glm::scale(groundModel, glm::vec3(35, 1, 25));
		
		glBindTexture(GL_TEXTURE_2D, groundTexture);

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(groundModel));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));
		glUniform1f(squeezeFactorLocation, 0);
		glUniform1fv(breathingColorLocation, 3, value_ptr(breathingColor));
		glUniform1f(intensityLocation, -1);
		glUniform1i(useGradientLocation, 0);

		glBindVertexArray(groundVAO);
		glDrawArrays(GL_TRIANGLES, 0, groundObject->positions.size());

        /* Done#6-2: Render Column
         *    1. Set up column model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         *    4. Apply the texture, and render the column.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */

		columnModel = glm::translate(columnModel, glm::vec3(0, -4, 0));
		columnModel = glm::rotate(columnModel, (float)glm::radians(rotateColumnDegree), glm::vec3(0, 1, 0));
		columnModel = glm::rotate(columnModel, (float)glm::radians(-90.0), glm::vec3(1, 0, 0));
		columnModel = glm::scale(columnModel, glm::vec3(0.05, 0.05, 0.05));
		
		glBindTexture(GL_TEXTURE_2D, columnTexture);

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(columnModel));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));
		glUniform1f(squeezeFactorLocation, 0);
		glUniform1fv(breathingColorLocation, 3, value_ptr(breathingColor));
		glUniform1f(intensityLocation, -1);
		glUniform1i(useGradientLocation, 0);

		glBindVertexArray(columnVAO);
		glDrawArrays(GL_TRIANGLES, 0, columnObject->positions.size());

        /* Done#6-3: Render Fish
         *    1. Set up fish model matrix.
         *    2. Send model, view, and projection matrices to the program.
         *    3. Send squeezeFactor, breathingColor, intensity, or other parameters to the program.
         *    4. Apply the texture, and render the fish.
         * Hint:
         *	  rotate, translate, scale
         *    glUniformMatrix4fv, glUniform1f, glUniform3fv
         *    glActiveTexture, glBindTexture, glBindVertexArray, glDrawArrays
         */

		fishModel = glm::rotate(fishModel, (float)glm::radians(revolveFishDegree), glm::vec3(0, 1, 0));
		fishModel = glm::translate(fishModel, glm::vec3(0, swayHeight * glm::sin(glm::radians(currentTime * 45)), fishColumnDist));
		fishModel = glm::rotate(fishModel, (float)glm::radians(rotateFishDegree), glm::vec3(-1, 0, 0));
		fishModel = glm::rotate(fishModel, (float)glm::radians(-90.0), glm::vec3(1, 0, 0));
		fishModel = glm::scale(fishModel, glm::vec3(0.05, 0.05, 0.05));
		
		glBindTexture(GL_TEXTURE_2D, fishTexture);

		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(fishModel));
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, value_ptr(view));
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, value_ptr(projection));
		glUniform1f(squeezeFactorLocation, glm::radians(squeezeFactor));
		glUniform3f(breathingColorLocation, breathingColor.x, breathingColor.y, breathingColor.z);
		glUniform1f(intensityLocation, (useBreathing ? (0.3 + 0.3 * glm::abs(glm::cos(glm::radians(breatheTime * 90)))) : -1));
		glUniform1i(useGradientLocation, useGradient);

		glBindVertexArray(fishVAO);
		glDrawArrays(GL_TRIANGLES, 0, fishObject->positions.size());


        // Status update
        currentTime = glfwGetTime();
        dt = currentTime - lastTime;
        lastTime = currentTime;

        /* TODO#7: Update "revolveFishDegree", "rotateColumnDegree", "rotateFishDegree", 
         *          "fishHeight", "heightDir", 
         *          "squeezeFactor", "breathingColor", "intensity"
         */
		revolveFishDegree -= revolveFishSpeed * dt;
		if (useSelfRotation) {
			rotateFishDegree += rotateFishSpeed * dt;
			if (rotateFishDegree > 360) {
				rotateFishDegree = 0;
				useSelfRotation = false;
			}
		}
		rotateColumnDegree += rotateColumnSpeed * dt;
		if (useSqueeze)
			squeezeFactor += squeezeSpeed * dt;
		breatheTime += dt;

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE.
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html

    /* TODO#8: Key callback 
    *    1. Press 'r' to rotate the fish.
    *    2. Press 's' to squeeze the fish.
    *    3. Press 'b' to make the breathing light.
    * Hint:
    *      GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_B
    *      GLFW_PRESS, GLFW_REPEAT
    */    

	if (action == GLFW_PRESS) {
		switch(key) {
			case GLFW_KEY_R:
				if (not useSelfRotation) {
					useSelfRotation = true;
					rotateFishDegree = 0;
				}
				break;
			case GLFW_KEY_S:
				useSqueeze = not useSqueeze;
				squeezeFactor = 0;
				break;
			case GLFW_KEY_B:
				useBreathing = not useBreathing;
				breatheTime = 0;
				break;
			case GLFW_KEY_G:
				useGradient = not useGradient;
				break;
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

/* Done#1: createShader
 * input:
 *      filename: shader file name
 *      type: shader type, "vert" means vertex shader, "frag" means fragment shader
 * output: shader object
 * Hint:
 *      glCreateShader, glShaderSource, glCompileShader
 */
unsigned int createShader(const string &filename, const string &type) {
	unsigned int shader;
	if (type == "vert")			shader = glCreateShader(GL_VERTEX_SHADER);
	else if (type == "frag")	shader = glCreateShader(GL_FRAGMENT_SHADER);
	else {
		std::cerr << "[ERROR] In createShader(): unknown shader type '" << type << "'\n";
		exit(EXIT_FAILURE);
	}
	string fileContent = readTextFile(filename);
	GLchar const* files[] = { fileContent.c_str() };
	GLint lengths[]       = { (int)fileContent.size() };
	glShaderSource(shader, 1, files, lengths);
	glCompileShader(shader);
	
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (not success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "[ERROR] In createShader(): shader '" << filename << "' of type '" << type << "' failed to compile.\n";
		std::cerr << infoLog << '\n';
		exit(EXIT_FAILURE);
	}

	return shader;
}

/* Done#2: createProgram
 * input:
 *      vertexShader: vertex shader object
 *      fragmentShader: fragment shader object
 * output: shader program
 * Hint:
 *      glCreateProgram, glAttachShader, glLinkProgram, glDetachShader
 */
unsigned int createProgram(unsigned int vertexShader, unsigned int fragmentShader) {
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (not success) {
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "[ERROR] In createProgram(): program failed to link.\n";
		std::cerr << infoLog << '\n';
		exit(EXIT_FAILURE);
	}

	return program;
}

/* Done#3: modelVAO
 * input:
 *      model: Object you want to render
 * output: VAO
 * Hint:
 *      glGenVertexArrays, glBindVertexArray, glGenBuffers, glBindBuffer, glBufferData,
 *      glVertexAttribPointer, glEnableVertexAttribArray,
 */
unsigned int modelVAO(Object &model) {
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	float mn = 1000000000.0;
	float mx = -1000000000.0;
	for (int i = 0; i < model.positions.size() / 3; i++) {
		mn = min(mn, model.positions[i * 3 + 0]);
		mx = max(mx, model.positions[i * 3 + 0]);
	}
	vector<float> gradientLevel;
	for (int i = 0; i < model.positions.size() / 3; i++) {
		gradientLevel.push_back((mx - model.positions[i * 3 + 0]) / (mx - mn));
	}

	unsigned int VBO[4];
	glGenBuffers(4, VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * model.positions.size(), &model.positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * model.normals.size(), &model.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * model.texcoords.size(), &model.texcoords[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * gradientLevel.size(), &gradientLevel[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(3);

	return VAO;
}

/* Done#4: loadTexture
 * input:
 *      filename: texture file name
 * output: texture object
 * Hint:
 *      glEnable, glGenTextures, glBindTexture, glTexParameteri, glTexImage2D
 */
unsigned int loadTexture(const string &filename) {
	unsigned int texture;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// get image data
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (not img) {
		cerr << "[ERROR] In loadTexture(): failed to load texture '" << filename << "'\n";
		exit(EXIT_FAILURE);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(img);

	return texture;
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    string dirShader = "../../src/shaders/";
    string dirAsset = "../../src/asset/obj/";
    string dirTexture = "../../src/asset/texture/";
#else
    string dirShader = "..\\..\\src\\shaders\\";
    string dirAsset = "..\\..\\src\\asset\\obj\\";
    string dirTexture = "..\\..\\src\\asset\\texture\\";
#endif

    // Object
    fishObject = new Object(dirAsset + "fish.obj");
    columnObject = new Object(dirAsset + "column.obj");
    groundObject = new Object(dirAsset + "cube.obj");

    // Shader
    vertexShader = createShader(dirShader + "vertexShader.vert", "vert");
    fragmentShader = createShader(dirShader + "fragmentShader.frag", "frag");
    shaderProgram = createProgram(vertexShader, fragmentShader);

    // Texture
    fishTexture = loadTexture(dirTexture + "fish.jpg");
    columnTexture = loadTexture(dirTexture + "column.jpg");
    groundTexture = loadTexture(dirTexture + "ground.jpg");

    // VAO, VBO
    fishVAO = modelVAO(*fishObject);
    columnVAO = modelVAO(*columnObject);
    groundVAO = modelVAO(*groundObject);
}

string readTextFile(const string &filename) {
	ifstream ifs(filename);
	ostringstream ss;
	ss << ifs.rdbuf();
	return ss.str();
}
