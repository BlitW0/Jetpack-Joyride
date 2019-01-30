#include "coin.h"
#include "main.h"

Coin::Coin(float x, float y, color_t color, float r) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    value = 50;
    disabled = 0;

    speed_x = -0.05;
    speed_y = 0;

    box = bounding_box_t(x, y, 2*r, 2*r);

    GLfloat vertex_buffer_data[] = {
        0, 0, 0,
        -r, 0, 0,
        0, r, 0,

        0, 0, 0,
        0, r, 0,
        r, 0, 0,

        0, 0, 0,
        r, 0, 0,
        0, -r, 0,

        0, 0, 0,
        0, -r, 0,
        -r, 0, 0,
    };

    this->object = create3DObject(GL_TRIANGLES, 4*3, vertex_buffer_data, color, GL_FILL);
}

void Coin::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(1, 0, 0));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Coin::drop() {
    float a = -9.81, u = this->speed_y, s, t = 1.0/60, v;
    v = u + a*t;
    s = u*t + (0.5)*a*(t*t);

    this->position.x += this->speed_x;

    float new_y = this->position.y + s;
    if (fabs(new_y - ground_y) > this->box.height/2) {
        this->position.y = new_y;
        this->speed_y = v;
    } else {
        this->disabled = 1;
        this->speed_y = 0;
    }

    this->box.x = this->position.x;
    this->box.y = this->position.y;
}