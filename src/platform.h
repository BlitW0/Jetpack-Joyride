#include "main.h"

#ifndef PLAT_H
#define PLAT_H


class Platform {
public:
    Platform() {}
    Platform(float x, float y, color_t color, float height, float width, float z);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
private:
    VAO *object;
};

#endif // PLAT_H
