#include "platform.h"
#include "main.h"

Platform::Platform(float x, float y, color_t color, float height, float width) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    this->height = height;
    this->width = width;

    box = bounding_box_t(x, y, width, height);

    dir_y = 0;
    t = 0;

    disabled = 0;

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

void Platform::draw(glm::mat4 VP) {
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

void Platform::vertical_shm() {
    float speed_y = 0.02;

    if (std::abs(screen_boundary - this->position.y) <= this->height/2) {
        this->dir_y *= -1;
    }
    if (std::abs(ground_y - this->position.y) <= this->height/2) {
        this->dir_y *= -1;
    }
    this->position.y += dir_y*speed_y;
    this->box.y = this->position.y;
}