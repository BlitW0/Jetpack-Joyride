#include "semi.h"
#include "main.h"

Semi::Semi(float x, float y, color_t color, float r) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;

    disabled = 0;
    this->r = r;

    float vertices[31][2];
    for (int i = 0; i <= 30; i++) {
        vertices[i][0] = r*cos(-M_PI*(float(i)/30.0));
        vertices[i][1] = r*sin(-M_PI*(float(i)/30.0));
    }

    GLfloat vertex_buffer_data[30*3*3];
    for (int i = 0; i < 30; i++) {

        vertex_buffer_data[9*i] = vertices[i][0];
        vertex_buffer_data[9*i + 1] = vertices[i][1];
        vertex_buffer_data[9*i + 2] = 0;

        vertex_buffer_data[9*i + 3] = vertices[(i + 1)][0];
        vertex_buffer_data[9*i + 4] = vertices[(i + 1)][1];
        vertex_buffer_data[9*i + 5] = 0;

        vertex_buffer_data[9*i + 6] = 0;
        vertex_buffer_data[9*i + 7] = 0;
        vertex_buffer_data[9*i + 8] = 0;

    }


    this->object = create3DObject(GL_TRIANGLES, 30*3, vertex_buffer_data, color, GL_FILL);
}

void Semi::draw(glm::mat4 VP) {
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
