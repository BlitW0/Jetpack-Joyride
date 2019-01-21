#include "main.h"

#ifndef COIN_H
#define COIN_H


class Coin {
public:
    Coin() {}
    Coin(float x, float y, color_t color, float r);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    
    int value;
    bool disabled;

    bounding_box_t box;
private:
    VAO *object;
};

#endif // COIN_H
