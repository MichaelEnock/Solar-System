#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stb_image.h>
#include "../include/shader_s.h"
#include "../include/Timer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../include/Sphere.h"
#include "../include/planet.h"
#include "../include/camera.h"

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 700;

//camera setting

glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 40.0f);
Camera camera(cameraPos);

float lastX = SCR_WIDTH * 0.5F, lastY = SCR_HEIGHT * 0.5F;
bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;

float deltaTime = 0.0f;
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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "solarSystem", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader skyShader("../shaders/sky.vs", "../shaders/sky.fs");

    float skyVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};

    unsigned int skyVAO, skyVBO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &skyVBO);
    glBindVertexArray(skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // creating sky
    unsigned int skyTexture;
    glGenTextures(1, &skyTexture);
    glBindTexture(GL_TEXTURE_2D, skyTexture);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps for the sky
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("../Textures/space3.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Build and run shader programs
    Shader planetShader("../shaders/planet.vs", "../shaders/planet.fs");
    Shader sunShader("../shaders/sun.vs", "../shaders/sun.fs");

    Planet sun(0.0f, 2.0f, 0.2f, 25.0f, glm::vec3(0, 1, 0), "../Textures/sunmap.jpg");
    Planet neptune(5.0f, 0.7f, 20.0f, 0.2f, glm::vec3(0, 1, 0), "../Textures/neptunemap.jpg");
    Planet earth(10.0f, 1.0f, 15.0f, 1.0f, glm::vec3(0, 1, 0), "../Textures/earthmap1k.jpg");
    Planet mars(15.0f, 1.3f, 13.0f, 1.8f, glm::vec3(0, 1, 0), "../Textures/marsmap1k.jpg");
    Planet venus(20.0f, 1.6f, 9.0f, 2.6f, glm::vec3(0, 1, 0), "../Textures/venusmap.jpg");
    Planet moon(3.0f, 1.0f, 2.0f, 1.0f, glm::vec3(0, 1, 0), "../Textures/moonmap1k.jpg");

    // creating Spheres and passing the shader program
    mars.createPlanetSphere(36, 18, true, 3, planetShader);
    venus.createPlanetSphere(36, 18, true, 3, planetShader);
    neptune.createPlanetSphere(36, 18, true, 3, planetShader);
    earth.createPlanetSphere(36, 18, true, 3, planetShader);
    moon.createPlanetSphere(36, 18, true, 3, planetShader);
    // jupiter.createPlanetSphere(36, 18, true, 3, planetShader);
    // mercury.createPlanetSphere(36, 18, true, 3, planetShader);

    sun.createPlanetSphere(36, 18, true, 3, sunShader);
    Timer timer;
    timer.start();

    //render loop
    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // calcurating delta time

        deltaTime = timer.getElapsedTime();
        timer.start();

        // ----- processing user input
        processInput(window);

        // rendering the sky
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, skyTexture);

        skyShader.use();
        glBindVertexArray(skyVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // depth testing
        glEnable(GL_DEPTH_TEST);
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        // using camera as the view
        glm::mat4 view = camera.GetViewMatrix(); // glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // sending data to the planet shader
        planetShader.use();
        unsigned int viewLoc = glGetUniformLocation(planetShader.ID, "view");
        unsigned int projectionLoc = glGetUniformLocation(planetShader.ID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Directional light
        glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.3f, -1.0f, -0.2f));
        planetShader.setVec3("dirLightDir", lightDirection);
        planetShader.setVec3("dirLightColor", glm::vec3(1.0f, 1.0f, 0.5f));

        // point light
        glm::vec3 sunPos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 pLightPos = sunPos + glm::vec3(0.0f, 10.0f, 0.0f);
        planetShader.setVec3("pointLightPos", pLightPos);
        planetShader.setVec3("pointLightColor", glm::vec3(0.3f, 0.3f, 1.0f));
        planetShader.setVec3("viewPos", cameraPos);

        // rendering the planets using current bound data
        neptune.renderPlanet(planetShader, VAO);
        venus.renderPlanet(planetShader, VAO);
        earth.renderPlanet(planetShader, VAO);
        mars.renderPlanet(planetShader, VAO);
        
        //renderind moon orbiting earth
        moon.renderMoon(planetShader, VAO, earth);
        sun.renderPlanet(planetShader, VAO);
       

        // sending data to sunShaders
        /* sunShader.use();
         unsigned int viewLocSun = glGetUniformLocation(sunShader.ID, "view");
         unsigned int projectionLocSun = glGetUniformLocation(sunShader.ID, "projection");
         glUniformMatrix4fv(viewLocSun, 1, GL_FALSE, glm::value_ptr(view));
         glUniformMatrix4fv(projectionLocSun, 1, GL_FALSE, glm::value_ptr(projection));
         // sun render
        // sun.renderPlanet(sunShader, VAO);
         */
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            sun.increaseSpinning();
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            sun.decreaseSpinning();
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            moon.increaseOrbiting();
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            moon.decreaseOrbiting();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly

// ---------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow *window)
{
    const float cameraSpeed = 10.0f * deltaTime;
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
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
