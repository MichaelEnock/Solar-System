#ifndef PLANET_H
#define PLANET_H
#include <string>
#include <glm/glm.hpp>
#include "../include/Sphere.h"
using namespace std;
class Planet
{
public:
    ~Planet();
    Planet(float distance, float radius, float rotationSpeed, float orbitPeriod, glm::vec3 rotationAxis, const char *texture);
    void createPlanetSphere(int sectorCount, int stackCount, bool smooth, int upAxis, Shader &planetShader);
    void renderPlanet(Shader &planetShader, unsigned int &VAO);
    void renderMoon(Shader &planetShader, unsigned int &VAO, Planet &Earth);
    void increaseSpinning();
    void decreaseSpinning();
    void increaseOrbiting();
    void decreaseOrbiting();
    glm::mat4 calculateModel();
    
private:
    Sphere pSphere;
    float distance;
    float radius;
    float orbitPeriod;
    glm::vec3 position;
    const char *texture;
    glm::vec3 rotationAxis;
    float rotationSpeed;
    unsigned int texture1;
};
#endif