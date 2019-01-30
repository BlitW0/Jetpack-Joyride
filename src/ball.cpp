#include "ball.h"
#include "main.h"

Ball::Ball(float x, float y, color_t color, float ground_level, float ceiling_level) {
    this->position = glm::vec3(x, y, 0);
    this->rotation = 0;
    
    this->ground_level = ground_level;
    this->ceiling_level = ceiling_level;
    
    is_jumping = 0;
    dir_x = 0;
    
    in_ring = 0;
    theta = 0;

    speed_x = 0.05f;
    speed_y = 0;

    box.x = x, box.y = y;
    box.width = box.height = 1.0;

    a_y = -9.81;

    score = 0;
    health = 2000;

    // Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
    // A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
    GLfloat vertex_buffer_data[] = {
        -0.5f,-0.5f,-0.5f, // triangle 1 : begin
        -0.5f,-0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, // triangle 1 : end
        0.5f, 0.5f,-0.5f, // triangle 2 : begin
        -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f, // triangle 2 : end
        0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        -0.5f,-0.5f,-0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f,-0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f,-0.5f,
        0.5f,-0.5f,-0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f,-0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f,-0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f,-0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        0.5f,-0.5f, 0.5f
    };

    this->object = create3DObject(GL_TRIANGLES, 12*3, vertex_buffer_data, color, GL_FILL);
}

void Ball::draw(glm::mat4 VP) {
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

void Ball::set_position(float x, float y) {
    this->position = glm::vec3(x, y, 0);
}

void Ball::tick() {
    if (this->in_ring) {

    }
    
    
    // Left or Right Movement
    if (this->dir_x) {
        float new_x = this->position.x + this->dir_x*this->speed_x;
        if (fabs(this->dir_x*screen_boundary + screen_center_x - new_x) > this->box.width/2) {
            this->position.x = new_x;
        }
    }

    // Vertical Movement
    if (this->is_jumping) { // Jump
        float new_y = this->position.y + this->speed_x;
        if (fabs(new_y - this->ceiling_level) > this->box.height/2) {
            this->position.y = new_y;
            this->speed_y = 0;
        }
    }
    else { // Gravity
        float a = this->a_y, u = this->speed_y, s, t = 1.0/60, v;

        v = u + a*t;
        s = u*t + (0.5)*a*(t*t);

        float new_y = this->position.y + s;
        if (s <= 0) {
            if (fabs(new_y - this->ground_level) > this->box.height/2) {
                this->position.y = new_y;
                this->speed_y = v;
            }
            else {
                this->position.y = -screen_boundary + 1.0 + 0.5;
                this->speed_y = 0;
            }
        } else {
            if (fabs(new_y - this->ceiling_level) > this->box.height/2) {
                this->position.y = new_y;
                this->speed_y = v;
            }
            else {
                this->speed_y = 0;
            }
        }
    }

    this->box.x = this->position.x;
    this->box.y = this->position.y;
}
