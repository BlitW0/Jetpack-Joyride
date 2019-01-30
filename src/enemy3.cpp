#include "enemy3.h"
#include "main.h"

Enemy3::Enemy3(float x, float y, color_t color, float a, float b) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    this->a = a;
    this->b = b;
    t = 0;

    this->centre_x = x;
    this->centre_y = y;

    float width = 0.4, height = 0.4;
    box = bounding_box_t(x, y, width, height);

    GLfloat vertex_buffer_data[] = {
         - width/2, - height/2, 0,
         - width/2, + height/2, 0,
         + width/2, + height/2, 0,

         - width/2, - height/2, 0,
         + width/2, + height/2, 0,
         + width/2, - height/2, 0
    };

    this->object = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color, GL_FILL);
}

void Enemy3::draw(glm::mat4 VP) {
    Matrices.model = glm::mat4(1.0f);
    glm::mat4 translate = glm::translate (this->position);    // glTranslatef
    glm::mat4 rotate    = glm::rotate((float) (this->rotation * M_PI / 180.0f), glm::vec3(0, 0, 1));
    // No need as coords centered at 0, 0, 0 of cube arouund which we waant to rotate
    // rotate          = rotate * glm::translate(glm::vec3(0, -0.6, 0));
    Matrices.model *= (translate * rotate);
    glm::mat4 MVP = VP * Matrices.model;
    glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
    draw3DObject(this->object);
}

void Enemy3::orbit() {
    this->rotation += 15;
    this->t += 1.0/60;

    this->position.x = this->centre_x + a*cos(t);
    this->position.y = this->centre_y + b*sin(t);

    this->box.x = this->position.x;
    this->box.y = this->position.y;
}