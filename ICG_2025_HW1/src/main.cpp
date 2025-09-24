#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <numbers>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "./header/Shader.h"
#include "./header/Object.h"

// Settings
const int INITIAL_SCR_WIDTH = 1280;
const int INITIAL_SCR_HEIGHT = 720;
const float AQUARIUM_BOUNDARY = 8.0f;
const float AQUARIUM_HEIGHT = 7.0f;

// Animation constants
const float TAIL_ANIMATION_SPEED = 5.0f;
const float WAVE_FREQUENCY = 1.5f;

int SCR_WIDTH = INITIAL_SCR_WIDTH;
int SCR_HEIGHT = INITIAL_SCR_HEIGHT;

// Global objects
Shader* shader = nullptr;
Object* cube = nullptr;
Object* fish1 = nullptr;
Object* fish2 = nullptr;
Object* fish3 = nullptr;

struct Fish {
    glm::vec3 position;
    glm::vec3 direction;
    std::string fishType = "fish1";
    float angle = 0.0f;
    float speed = 3.0f;
    glm::vec3 scale = glm::vec3(2.0f, 2.0f, 2.0f);
    glm::vec3 color = glm::vec3(1.0f, 0.5f, 0.3f);
};

struct SeaweedSegment {
    glm::vec3 localPos;
    glm::vec3 color;
    float phase;
	float angle;
    glm::vec3 scale;
	float swayAmplitude;
    SeaweedSegment* next = nullptr;
};

struct Seaweed {
    glm::vec3 basePosition;
    SeaweedSegment* rootSegment = nullptr;
    float swayOffset = 0.0f;
	float swaySpeed = glm::radians(90.0f); // radians per second
};

struct playerFish {
    glm::vec3 position = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 dimensions = glm::vec3(5.0f, 3.0f, 2.5f);
    float angle = 0.0f; // Heading direction in radians
    float speed = 2.0f;
    float rotationSpeed = 2.0f;
    bool mouthOpen = false; 
    float tailAnimation = 0.0f;
    // for tooth
    float duration = 1.0f;     
    float elapsed = 0.0f;    
    struct tooth{
        glm::vec3 pos0, pos1;
    }toothUpperLeft, toothUpperRight, toothLowerLeft, toothLowerRight;
} playerFish;

// Aquarium elements
std::vector<Seaweed> seaweeds;
std::vector<Fish> schoolFish;

float globalTime = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window, float deltaTime);
void drawModel(std::string type, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color);
void drawPlayerFish(const glm::vec3& position, float angle, float tailPhase,
                    const glm::mat4& view, const glm::mat4& projection, bool mouthOpen, float deltaTime);
void drawSeaweed(const Seaweed& seaweed, const glm::mat4& view, const glm::mat4& projection);
void updateSeaweed(Seaweed& seaweed, float deltaTime);
void updateSchoolFish(float deltaTime);
Seaweed createSeaweed(glm::vec3 basePosition, int segmentCount, float swayOffset, float swaySpeed, float scaleDecay, glm::vec3 color);
void initializeAquarium();
void cleanup();
void init();

int main() {
    // Initialize random seed for aquarium elements
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // GLFW: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // GLFW window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GPU-Accelerated Aquarium", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // GLAD: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth test, face culling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

    // Initialize Object and Shader
    init();
    initializeAquarium();

    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time for the usage of animation
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        globalTime = currentFrame;

        playerFish.tailAnimation += deltaTime * TAIL_ANIMATION_SPEED;

        // Render background
        glClearColor(0.2f, 0.5f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->use();

        /*=================== Example of creating model matrix ======================= 
        1. translate
        glm::mat4 model(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 0.0f));
        drawModel("cube",model,view,projection, glm::vec3(0.9f, 0.8f, 0.6f));
        
        2. scale
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 2.0f)); 
        drawModel("cube",model,view,projection, glm::vec3(0.9f, 0.8f, 0.6f));
        
        3. rotate
        glm::mat4 model(1.0f);
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        drawModel("cube",model,view,projection, glm::vec3(0.9f, 0.8f, 0.6f));
        ==============================================================================*/

        // Create model, view, and perspective matrix

		glm::mat4 playerFishModel(1.0f);
		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 10.0f, 25.0f),
			glm::vec3(0.0f, 8.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			(float)SCR_WIDTH / (float)SCR_HEIGHT,
			0.1f,
			1000.0f
		);

        // Aquarium Base
		glm::mat4 aquariumBaseModel(1.0f);
		aquariumBaseModel = glm::scale(aquariumBaseModel, glm::vec3(70.0f, 1.0f, 40.0f));
		drawModel("cube", aquariumBaseModel, view, projection, glm::vec3(0.9f, 0.8f, 0.6f));
        
        // Draw seaweeds with hierarchical structure and wave motion
        // Wave motion is sine wave based on global time and segment phase
        // Each segment sways slightly differently for natural effect
        // E.g. Amplitude * sin(keepingChangingX + delayPhase)
        // delayPhase is different for each segment
        // the deeper the segment is, the larger the delayPhase is.
        // so that you can create a forward wave motion.
		for (Seaweed& seaweed : seaweeds) {
			updateSeaweed(seaweed, deltaTime);
			drawSeaweed(seaweed, view, projection);
		}

        // Draw school of fish
        // The fish movement logic is implemented.
        // All you need is to set up the position like the example in initAquarium()
        for (const auto& fish : schoolFish) {
            glm::mat4 model(1.0f);
            model = glm::translate(model, fish.position);
            model = glm::rotate(model, fish.angle, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, fish.scale);
            drawModel(fish.fishType, model, view, projection, fish.color);
        }
        // Update aquarium elements
        updateSchoolFish(deltaTime);

        // TODO: Draw Player Fish
        // You can use the provided function drawPlayerFish() or implement your own version.
        // The key idea of hierarchy is to reuse the model matrix to the children.
        // E.g. 
        // glm::mat4 model(1.0f);
        // glm::mat4 bodyModel;
        // model = glm::translate(model, position);
        // ^-- "position": Move the whole body to the desired position.
        // model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));  
        //  ^-- "angle": Rotate the whole body but in the homework case, no need to rotate the fish.
        // bodyModel = glm::scale(model, glm::vec3(5.0f, 3.0f, 2.5f)); // Elongated for shark body
        // drawModel("cube", bodyModel, view, projection, glm::vec3(0.4f, 0.4f, 0.6f)); // Dark blue-gray shark color
        // Reuse "model" for the children of the body.
        // glm::mat4 dorsalFinModel;
        // dorsalFinModel = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
        // dorsalFinModel = glm::rotate(dorsalFinModel, glm::radians(-50.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // dorsalFinModel = glm::scale(dorsalFinModel, glm::vec3(3.0f, 1.5f, 1.0f));
        // drawModel("cube", dorsalFinModel, view, projection, glm::vec3(0.35f, 0.35f, 0.55f)); // Fin color
        //
        // Notice that to keep the scale of the children is not affected by the body scale,
        // you need to apply the inverse scale to the fin model matrix, 
        // or separate the scale computation from the parent model matrix.
        //
        // For the wave motion of the tail, you can use a sine function based on time,
        // which is provided as playerFish.tailAnimation that would act as tail phase in the drawPlayerFish().
        // To make the tail motion, follow the formula: Amplitude * sin(tailPhase);
        drawPlayerFish(playerFish.position, playerFish.angle, playerFish.tailAnimation,
                        view, projection, playerFish.mouthOpen, deltaTime);

        // TODO: Implement input processing
        processInput(window, deltaTime);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    cleanup();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void processInput(GLFWwindow* window, float deltaTime) {
    // We use process_input in the display/render loop instead of relying solely on keyCallback
    // because key events (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT) are not emitted every frame.
    // keyCallback only triggers on discrete key events, but for continuous key behavior (e.g., holding down a key),
    // we need to check key states every frame using glfwGetKey.
    // This ensures smooth, frame-consistent input handling such as movement or rotation.

    // Controls:
    // - W / S           : Move the fish along the X-axis.
    // - A / D           : Move the fish along the Z-axis.
    // - SPACE / LSHIFT  : Move the fish up / down along the Y-axis.
    //
    // Behavior:
    // - Movement is directly applied to the fish's position on the X, Y, and Z axes.
    // - Vertical movement (Y-axis) is independent of horizontal movement.
    // - The fish can move freely in all three axes but should be clamped within
    //   the aquarium boundaries to stay visible.
    
	float deltaSpeed = playerFish.speed * deltaTime;
	glm::vec3 playerShift(0.0f, 0.0f, 0.0f);

   	bool xUp	= (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS);
    bool xDown	= (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
    bool yUp	= (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    bool yDown	= (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    bool zUp	= (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS);
    bool zDown	= (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
	if (xUp ^ xDown) {
		if (xUp) playerShift.x += 1.0f;
		else	 playerShift.x -= 1.0f;
	}
	if (yUp ^ yDown) {
		if (yUp) playerShift.y += 1.0f;
		else	 playerShift.y -= 1.0f;
	}
	if (zUp ^ zDown) {
		if (zUp) playerShift.z += 1.0f;
		else	 playerShift.z -= 1.0f;
	}

	if ((xUp ^ xDown) or (yUp ^ yDown) or (zUp ^ zDown))
		playerShift = glm::normalize(playerShift) * deltaSpeed;

    // Keep fish within aquarium bounds
	playerFish.position += playerShift;
	playerFish.position.x = max(playerFish.position.x, -AQUARIUM_BOUNDARY + playerFish.dimensions.x / 2);
	playerFish.position.x = min(playerFish.position.x, AQUARIUM_BOUNDARY - playerFish.dimensions.x / 2);
	playerFish.position.z = max(playerFish.position.z, -AQUARIUM_BOUNDARY + playerFish.dimensions.z / 2);
	playerFish.position.z = min(playerFish.position.z, AQUARIUM_BOUNDARY - playerFish.dimensions.z / 2);
	playerFish.position.y = max(playerFish.position.y, 0 + playerFish.dimensions.y / 2);
	playerFish.position.y = min(playerFish.position.y, AQUARIUM_HEIGHT - playerFish.dimensions.y / 2);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // The action is one of GLFW_PRESS, GLFW_REPEAT or GLFW_RELEASE. 
    // Events with GLFW_PRESS and GLFW_RELEASE actions are emitted for every key press.
    // Most keys will also emit events with GLFW_REPEAT actions while a key is held down.
    // https://www.glfw.org/docs/3.3/input_guide.html
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Implement mouth toggle logic
	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		playerFish.mouthOpen = not playerFish.mouthOpen;
	}
}

void drawModel(std::string type, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& color) {
    shader->set_uniform("projection", projection);
    shader->set_uniform("view", view);
    shader->set_uniform("model", model);
    shader->set_uniform("objectColor", color);
    if (type == "fish1") {
        fish1->draw();
    } else if (type == "fish2") {
        fish2->draw();
    } else if (type == "fish3") {
        fish3->draw();
    }else if (type == "cube") {
        cube->draw();
    }
}

void init() {
#if defined(__linux__) || defined(__APPLE__)
    std::string dirShader = "shaders/";
    std::string dirAsset = "asset/";
#else
    std::string dirShader = "shaders\\";
    std::string dirAsset = "asset\\";
#endif

    shader = new Shader((dirShader + "easy.vert").c_str(), (dirShader + "easy.frag").c_str());
   
    cube = new Object(dirAsset + "cube.obj");
    fish1 = new Object(dirAsset + "fish1.obj");
    fish2 = new Object(dirAsset + "fish2.obj");
    fish3 = new Object(dirAsset + "fish3.obj");
}

void cleanup() {
    if (shader) {
        delete shader;
        shader = nullptr;
    }
    
    if (cube) {
        delete cube;
        cube = nullptr;
    }
    
    for (auto& seaweed : seaweeds) {
        SeaweedSegment* current = seaweed.rootSegment;
        while (current != nullptr) {
            SeaweedSegment* next = current->next;
            delete current;
            current = next;
        }
    }
    seaweeds.clear();
    
    schoolFish.clear();
}

void drawSeaweed(const Seaweed& seaweed, const glm::mat4& view, const glm::mat4& projection) {
	glm::vec3 currentPosition = seaweed.basePosition;
	SeaweedSegment* segment = seaweed.rootSegment;
	while (segment != nullptr) {
		currentPosition += segment->localPos;
		glm::mat4 model(1.0f);
		model = glm::translate(model, currentPosition);
		model = glm::rotate(model, segment->angle, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, segment->scale);
		drawModel("cube", model, view, projection, segment->color);
		currentPosition += segment->localPos;
		cout << currentPosition.x << ' '  << currentPosition.y << ' ' << currentPosition.z << '\n';
		segment = segment->next;
	}
}

void updateSeaweed(Seaweed& seaweed, float deltaTime) {
	SeaweedSegment* segment = seaweed.rootSegment;
	while (segment != nullptr) {
		segment->localPos = glm::vec3(0.0f, 1.0f, 0.0f);
		segment->localPos.x = segment->swayAmplitude * sin(segment->phase);
		segment->localPos = glm::vec3(glm::scale(glm::mat4(1.0f), segment->scale) * glm::vec4(segment->localPos, 0.0f)) * 0.5f;
		segment->angle = atan2(-segment->localPos.x, segment->localPos.y);
		segment->phase += seaweed.swaySpeed * deltaTime;
		segment = segment->next;
	}
}

void drawPlayerFish(const glm::vec3& position, float angle, float tailPhase,
    const glm::mat4& view, const glm::mat4& projection, bool mouthOpen, float deltaTime) {
	cout
		<< "drawing player at ("
		<< position.x << ", "
		<< position.y << ", "
		<< position.z << ")" << endl;

    // Draw body using cube (main body)
    glm::mat4 bodyModel(1.0f);
	bodyModel = glm::scale(bodyModel, glm::vec3(5.0f, 3.0f, 2.5f));
	bodyModel = glm::rotate(bodyModel, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	bodyModel = glm::translate(bodyModel, position);
	drawModel("cube", bodyModel, view, projection, glm::vec3(0.4f, 0.4f, 0.6f));
  
    // TODO: Draw head and Mouth using cube with mouth open/close feature
    if (mouthOpen) {
        // TODO: head and mouth model matrix adjustment

        // TODO: Calculate elapse time for tooth animation
       
        // TODO: Upper teeth right
        
        // TODO: Upper teeth left
      
        // TODO: Lower teeth right 
       
        // TODO: Lower teeth left
        
    } else {
        // TODO: head and mouth model matrix adjustment
    } 

    // TODO: Draw Eyes
 
    // TODO: Draw Pupils

    // TODO: Draw dorsal fin (top fin)

    // TODO: Draw side fins (pectoral fins)

    // TODO: Draw hierarchical animated tail with multiple segments
    
    // TODO: Draw tail fin at the end
 }
 
void updateSchoolFish(float deltaTime) {
    for (auto& fish : schoolFish) {
        // Move fish in their direction
        fish.position += fish.direction * fish.speed * deltaTime;
        
        // Bounce off walls
        // The Movement is clamped within aquarium boundaries to prevent
        // fish from escaping the visible scene.
        // atan2 calculates the angle of the fish's direction vector on the XZ plane.
        // To make the fish movement natural.
        if (fish.position.x > AQUARIUM_BOUNDARY - 1.0f || fish.position.x < -AQUARIUM_BOUNDARY + 1.0f) {
            fish.direction.x *= -1;
            fish.angle = atan2(-fish.direction.z, fish.direction.x);
        }
    }
}

Seaweed createSeaweed(glm::vec3 basePosition, int segmentCount, float swayOffset = 0.0f, float swaySpeed = glm::radians(90.0f), float swayAmplitude = 0.05f, float scaleDecay = 0.95f, glm::vec3 color = glm::vec3(0.4f, 0.6f, 0.4f)) {
	Seaweed newSeaweed;
	newSeaweed.basePosition = basePosition;
	newSeaweed.swayOffset = swayOffset;
	newSeaweed.swaySpeed = swaySpeed;

	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	float phase = 0.0f;

	newSeaweed.rootSegment = new SeaweedSegment;
	SeaweedSegment* segment = newSeaweed.rootSegment;
	for (int i = 0; i < segmentCount; i++) {
		segment->color = color;
		segment->phase = phase;
		segment->scale = scale;
		segment->swayAmplitude = swayAmplitude;
		if (i != segmentCount - 1) {
			segment->next = new SeaweedSegment;
		}
		phase += swayOffset;
		scale *= scaleDecay;
//		swayAmplitude *= scaleDecay;
		segment = segment->next;
	}

	return newSeaweed;
}

void initializeAquarium() {
    srand(static_cast<unsigned int>(time(nullptr)));

    // You can init the aquarium elements here
    // e.g.
    // playerFish.toothUpperRight.pos0 = glm::vec3(1.0f, -0.3f, 0.5f);
    // playerFish.toothUpperRight.pos1 = glm::vec3(1.0f, -1.5f, 0.5f);
    //
    // schoolFish.clear();
    // const std::map<std::string, glm::vec3> FishPositions {
    //     {"fish1", glm::vec3(0.0f, 15.0f, 0.0f)},
    // };
    // for (const auto& [name, pos] : FishPositions) {
    //     Fish fish;
    //     fish.position = pos;
    //     fish.direction = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
    //     fish.fishType = name;
    //     fish.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
    //     schoolFish.push_back(fish);
    // }
	
	schoolFish.clear();
	const std::vector<std::pair<std::string, glm::vec3>> fishPositions {
		{"fish1", glm::vec3(0.0f, 15.0f, 0.0f)},
		{"fish2", glm::vec3(7.0f, 3.0f, 0.0f)},
		{"fish3", glm::vec3(-3.0f, 7.0f, -7.0f)},
	};
	for (const auto& [name, pos] : fishPositions) {
		Fish fish;
		fish.position = pos;
		fish.direction = glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f));
		fish.fishType = name;
		fish.color = glm::vec3(static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX, static_cast<float>(rand()) / RAND_MAX);
		schoolFish.push_back(fish);
	}

	seaweeds.push_back(createSeaweed(
		glm::vec3(7.0f, 0.0f, 0.0f),
		20,
		glm::radians(30.0f),
		glm::radians(100.0f),
		0.20f,
		0.98f
	));
	seaweeds.push_back(createSeaweed(
		glm::vec3(-7.0f, 0.0f, -10.0f),
		20,
		glm::radians(20.0f),
		glm::radians(240.0f),
		0.05f,
		0.98f
	));
	seaweeds.push_back(createSeaweed(
		glm::vec3(-7.0f, 0.0f, 5.0f),
		15,
		glm::radians(7.0f),
		glm::radians(90.0f),
		0.15f,
		0.98f
	));
}
