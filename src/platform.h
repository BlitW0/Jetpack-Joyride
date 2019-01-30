#include "main.h"

#ifndef PLAT_H
#define PLAT_H


class Platform {
public:
    Platform() {}
    Platform(float x, float y, color_t color, float height, float width);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);

    bounding_box_t box;
    float height, width;
    float dir_y;
    void vertical_shm();
    float t;

    bool disabled;
private:
    VAO *object;
};

#endif // PLAT_H
