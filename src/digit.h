#include "main.h"
#include "platform.h"

#ifndef DIG_H
#define DIG_H


class Digit {
public:
    Digit() {}
    Digit(float x, float y, color_t color, bool seven_segs[]);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);

    Platform body[7];
    bool seven_segs[7];
private:
    VAO *object;
}; 

#endif // DIG_H
