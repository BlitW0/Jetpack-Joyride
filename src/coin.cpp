#include "coin.h"
#include "main.h"

Coin::Coin(float x, float y, color_t color, float r) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    value = 50;
    disabled = 0;

    box.x = x, box.y = y;
    box.height = box.width = 2*r;

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