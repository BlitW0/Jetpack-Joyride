#include "platform.h"
#include "main.h"

Platform::Platform(float x, float y, color_t color, float height, float width, float z) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    GLfloat vertex_buffer_data[] = {
         - width/2, - height/2, z,
         - width/2, + height/2, z,
         + width/2, + height/2, z,

         - width/2, - height/2, z,
         + width/2, + height/2, z,
         + width/2, - height/2, z
    };

    this->object = create3DObject(GL_TRIANGLES, 2*3, vertex_buffer_data, color, GL_FILL);
}

void Platform::draw(glm::mat4 VP) {
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