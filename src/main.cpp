#include "../include/shader_s.h"
#include <glad/glad.h>
#include "../include/Timer.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../include/Sphere.h"
#include "../include/planet.h"

void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 700;

glm::vec3 cameraPos = glm::vec3(0.0f, 10.0f, 40.0f);                   // glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(0.0f, -0.2f, -1.0f)); // glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH * 0.5F, lastY = SCR_HEIGHT * 0.5F;
bool firstMouse = true;

float yaw = -90.0f;
float pitch = 0.0f;

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

    // load image, create texture and generate mipmaps
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

    // Build and run shader program
    Shader planetShader("../shaders/planet.vs", "../shaders/planet.fs");
    Shader sunShader("../shaders/sun.vs", "../shaders/sun.fs");

    Planet sun(0.0f, 2.0f, 0.2f, 25.0f, glm::vec3(0, 1, 0), "../Textures/sunmap.jpg");
    Planet neptune(5.0f, 0.7f, 20.0f, 0.2f, glm::vec3(0, 1, 0), "../Textures/neptunemap.jpg");
    Planet earth(10.0f, 1.0f, 15.0f, 1.0f, glm::vec3(0, 1, 0), "../Textures/earthmap1k.jpg");
    Planet mars(15.0f, 1.3f, 13.0f, 1.8f, glm::vec3(0, 1, 0), "../Textures/marsmap1k.jpg");
    Planet venus(20.0f, 1.6f, 9.0f, 2.6f, glm::vec3(0, 1, 0), "../Textures/venusmap.jpg");
    Planet moon(1.5f, 0.03f, 8.0f, 1.0f, glm::vec3(0, 1, 0), "../Textures/moonmap1k.jpg");


    // Planet jupiter(25.0f, 1.4f, 6.0f, 3.4f, glm::vec3(0, 1, 0), "..\\Textures\\jupitermap.jpg");
    // Planet mercury(25.0f, 1.4f, 20.5f, 7.0f, glm::vec3(0, 1, 0), "..\\Textures\\mercurymap1.jpg");
    //  Planet mercury(11.0f, 1.4f, 20.5f, 7.0f, glm::vec3(0, 1, 0), "..\\Textures\\mercurymap1k.jpg");

    mars.createPlanetSphere(36, 18, true, 3, planetShader);
    venus.createPlanetSphere(36, 18, true, 3, planetShader);
    neptune.createPlanetSphere(36, 18, true, 3, planetShader);
    earth.createPlanetSphere(36, 18, true, 3, planetShader);
    moon.createPlanetSphere(36, 18, true, 3, planetShader);
    // jupiter.createPlanetSphere(36, 18, true, 3, planetShader);
    // mercury.createPlanetSphere(36, 18, true, 3, planetShader);

    sun.createPlanetSphere(36, 18, true, 3, sunShader);

    while (!glfwWindowShouldClose(window))
    {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // calcurating delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

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
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // sending data to the planet shader
        planetShader.use();
        unsigned int viewLoc = glGetUniformLocation(planetShader.ID, "view");
        unsigned int projectionLoc = glGetUniformLocation(planetShader.ID, "projection");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // rendering the planets using current bound datas
        neptune.renderPlanet(planetShader, VAO);
        venus.renderPlanet(planetShader, VAO);
        earth.renderPlanet(planetShader, VAO);
        mars.renderPlanet(planetShader, VAO);
       // moon.renderPlanet(planetShader, VAO);
        //  jupiter.renderPlanet(planetShader, VAO);
        // mercury.renderPlanet(planetShader, VAO);
        moon.renderMoon(planetShader, VAO);
        
       
      
        // sending data to sunShaders
        sunShader.use();
        unsigned int viewLocSun = glGetUniformLocation(sunShader.ID, "view");
        unsigned int projectionLocSun = glGetUniformLocation(sunShader.ID, "projection");
        glUniformMatrix4fv(viewLocSun, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocSun, 1, GL_FALSE, glm::value_ptr(projection));
        // sun render
        sun.renderPlanet(sunShader, VAO);

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
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    ;
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

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw) * cos(glm::radians(pitch)));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw) * cos(glm::radians(pitch)));
    cameraFront = glm::normalize(direction);
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

