#include "Game.h"

#include <glm/glm.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "utils/glError.hpp"

#include <GLFW/glfw3.h>

char keyOnce[GLFW_KEY_LAST + 1];
#define glfwGetKeyOnce(WINDOW, KEY)             \
    (glfwGetKey(WINDOW, KEY) ?              \
     (keyOnce[KEY] ? false : (keyOnce[KEY] = true)) :   \
     (keyOnce[KEY] = false))

Game::Game()
    : Application()
{
    //init
    glCheckError(__FILE__,__LINE__);

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f,0.0f,1.0f);

    camera = new Camera(position, direction);
    heightMap = new HeightMap("../Textures/fractalnoise.bmp");
    terrain = new Terrain(heightMap);
    glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(getWindow(), GLFW_STICKY_KEYS, 1);
}

Game::~Game() {
    delete camera;
    delete heightMap;
    delete terrain;
}

void Game::loop() {
    glm::mat4 projection = glm::perspective(float(2.0*atan(getHeight()/1920.f)), getWindowRatio(), 0.1f, 100.f);
    if (glfwWindowShouldClose(getWindow()) || !handleUserInput(getWindow(), camera))
        exit();

    if (render_debug_mode)
        terrain->debugRender(camera);
    else
        terrain->render(camera);

    if(render_area_mode)
        terrain->areaRender(camera);
}

bool Game::handleUserInput(GLFWwindow* window, Camera *camera){
    if (glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS){
		return false;
	}

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	if(hidden_cursor) {
        glfwSetCursorPos(window, 1024/2, 768/2);
        // Compute new orientation
        horizontalAngle += mouseSpeed * deltaTime * float(1024/2 - xpos );
        verticalAngle   += mouseSpeed * deltaTime * float( 768/2 - ypos );
	}

    if(verticalAngle > 3.14/2) verticalAngle = 3.14/2;
    else if(verticalAngle < -3.14/2) verticalAngle = -3.14/2;

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f),
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	//lock position for testing
	if (glfwGetKeyOnce( window, GLFW_KEY_SPACE )){
		poslock = !poslock;
	}

    if (glfwGetKeyOnce( window, GLFW_KEY_K )){
        if (hidden_cursor) {
            glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        hidden_cursor = !hidden_cursor;
    }

    if (glfwGetKeyOnce( window, GLFW_KEY_R )){
        render_debug_mode = !render_debug_mode;
        std::cout << "debug render: " << render_debug_mode << std::endl;
    }
    if (glfwGetKeyOnce( window, GLFW_KEY_T )){
        render_area_mode = !render_area_mode;
        std::cout << "area render: " << render_area_mode << std::endl;
    }


	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;


	camera->projectionMatrix = ProjectionMatrix;
	camera->viewMatrix = ViewMatrix;
	if(!poslock) camera->position = position;
	camera->direction = direction;

	return true;
}
