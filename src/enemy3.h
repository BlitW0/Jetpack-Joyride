#include "main.h"

#ifndef EN3_H
#define EN3_H


class Enemy3 {
public:
    Enemy3() {}
    Enemy3(float x, float y, color_t color, float a, float b);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);

    bounding_box_t box;
    float a, b;
    float centre_x, centre_y;
    void orbit();

    long double t;
private:
    VAO *object;
};

#endif // EN3_H
