#include "../include/shader_s.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../include/planet.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

// Build shaders;

const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 700;

Planet::Planet(float distance, float radius, float rotationSpeed, float orbitPeriod, glm::vec3 rotationAxis, const char *texture)
{
    this->distance = distance;
    this->radius = radius;
    this->position = glm::vec3(0.0f, 0.0f, distance);
    this->rotationSpeed = rotationSpeed;
    this->orbitPeriod = orbitPeriod;
    this->rotationAxis = rotationAxis;
    this->texture = texture;
    this->pSphere = Sphere();
}

void Planet::createPlanetSphere(int sectorCount, int stackCount, bool smooth, int upAxis, Shader &planetShader)
{
    pSphere.setSectorCount(sectorCount);
    pSphere.setStackCount(stackCount);
    pSphere.setSmooth(smooth);
    pSphere.setUpAxis(upAxis);

    glBufferData(GL_ARRAY_BUFFER, pSphere.getInterleavedVertexSize(), pSphere.getInterleavedVertices(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, pSphere.getIndexSize(), pSphere.getIndices(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load(this->texture, &width, &height, &nrChannels, 0);
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
    planetShader.use();
    glUniform1i(glGetUniformLocation(planetShader.ID, "texture1"), 0);
    // planetShader.setInt("texture1", 0);
}
float t;
void Planet::renderPlanet(Shader &planetShader, unsigned int &VAO)
{

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    planetShader.use();
    glm::mat4 model = calculateModel();

    //  unsigned int modelLoc = glGetUniformLocation(planetShader.ID, "modelMatrix");
    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    unsigned int modelLoc = glGetUniformLocation(planetShader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, pSphere.getIndexSize(), GL_UNSIGNED_INT, 0);
}

void Planet::renderMoon(Shader &planetShader, unsigned int &VAO)
{

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);

    planetShader.use();
    glm::mat4 earthModel = glm::mat4(1.0f);
    glm::mat4 localMoon = glm::mat4(1.0f);

    float moonOrbitRadius = 0.2f;

    localMoon = glm::rotate(localMoon, t * rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
    localMoon = glm::translate(localMoon, glm::vec3(moonOrbitRadius, 0.0f, 0.0f));
    localMoon = glm::scale(localMoon, glm::vec3(radius));

    // Earth orbiting sun
    float earthOrbitSpeed = 1.0f;
    float earthRadius = 15.0f;
    float earthAngle = t * earthOrbitSpeed;
    float distance = 10.0f;

    float angleOrbit = t * (1.0f / orbitPeriod);
    float x = cos(angleOrbit) * distance;
    float z = sin(angleOrbit) * distance;

    earthModel = glm::translate(earthModel, glm::vec3(x, 0.0f, z));
    earthModel = glm::rotate(earthModel, t * rotationSpeed, rotationAxis);
    earthModel = glm::scale(earthModel, glm::vec3(earthRadius));

    // Moon orbiting earth
    glm::mat4 moonModel = earthModel * localMoon;

    unsigned int modelLoc = glGetUniformLocation(planetShader.ID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(moonModel));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, pSphere.getIndexSize(), GL_UNSIGNED_INT, 0);
}

glm::mat4 Planet::calculateModel()
{
    glm::mat4 model = glm::mat4(1.0f);
    t = (float)glfwGetTime();

    if (distance > 0.0f)
    {
        float angleOrbit = t * (1.0f / orbitPeriod);
        float x = cos(angleOrbit) * distance;
        float z = sin(angleOrbit) * distance;
        // modelMatrix = glm::translate(modelMatrix, glm::vec3(x, 0.0f, z));
        model = glm::translate(model, glm::vec3(x, 0.0f, z));
    }

    // modelMatrix = glm::rotate(modelMatrix, t * rotationSpeed, rotationAxis);
    // modelMatrix = glm::scale(modelMatrix, glm::vec3(radius));
    model = glm::rotate(model, t * rotationSpeed, rotationAxis);
    model = glm::scale(model, glm::vec3(radius));

    return model;
}

Planet::~Planet()
{
}