#include "main.h"

#ifndef CIRC_H
#define CIRC_H


class Semi {
public:
    Semi() {}
    Semi(float x, float y, color_t color, float r);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    
    bool disabled;
    float r;
private:
    VAO *object;
};

#endif // CIRC_H
