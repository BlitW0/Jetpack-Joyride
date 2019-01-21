#include "main.h"

#ifndef BALL_H
#define BALL_H


class Ball {
public:
    Ball() {}
    Ball(float x, float y, color_t color, float ground_level, float ceiling_level);
    glm::vec3 position;
    float rotation;
    void draw(glm::mat4 VP);
    void set_position(float x, float y);
    void tick();
    double speed_x, speed_y;

    bounding_box_t box;

    float dir_x;
    float ground_level, ceiling_level;
    bool is_jumping;

    int score;

private:
    VAO *object;
};

#endif // BALL_H
