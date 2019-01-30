#include "digit.h"
#include "main.h"

Digit::Digit(float x, float y, color_t color, bool seven_segs[]) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    float h = 0.07, w = 0.3/1.5;
    for (int i = 0; i < 7; i++) {
        this->seven_segs[i] = seven_segs[i];

        if (i == 0)
            body[i] = Platform(x, y, color, h, w);
        else if (i == 1)
            body[i] = Platform(x + h/2 + w/2, y + w/2, color, w, h);
        else if (i == 2)
            body[i] = Platform(x, y + w + h, color, h, w);
        else if (i == 3)
            body[i] = Platform(x - w/2 - h/2, y + w/2, color, w, h);
        else if (i == 4)
            body[i] = Platform(x - w/2 - h/2, y - w/2, color, w, h);
        else if (i == 5)
            body[i] = Platform(x, y - w - h, color, h, w);
        else
            body[i] = Platform(x + h/2 + w/2, y - w/2, color, w, h);
    }
}

void Digit::draw(glm::mat4 VP) {
    for (int i = 0; i < 7; i++)
        if (this->seven_segs[i])
            this->body[i].draw(VP);
}