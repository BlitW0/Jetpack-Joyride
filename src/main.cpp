#include "main.h"
#include "timer.h"
#include "ball.h"
#include "platform.h"
#include "coin.h"
#include "enemy3.h"
#include "digit.h"
#include "semi.h"

using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/


float ground_y = -screen_boundary + 1.0;

float player_height = 1.0, player_width = 1.0;
float start_x = -screen_boundary + 1.0, start_y = ground_y + player_height/2;

bounding_box_t screen_box = bounding_box_t(screen_center_x, screen_center_y, screen_boundary*2, screen_boundary*2);

Ball player;
Platform lower_platform;
Platform wall;

int total_types = 4, total_groups = 3;
float last_gen = 0;

Semi semi_out, semi_in;
float semi_x = 2, semi_y = 1;

vector <Coin> balloons;
int balloon_cnt = 0;

Coin coins[10][10];
int num_coins = 10;
float coin_size = 0.2;

Coin special_coin[10];
float last_spec_x = 4;

Platform fire[10][10];
int rows_drawn = 0;

Platform magnet[10];
float magnet_height = 1.0, magnet_width = 1.0;

bool seven_segs[10][7];
float dig_h = 0.07, dig_w = 0.3/1.5;

Platform enemy2[10];
float enemy2_width = 4.0, enemy2_height = 0.2;

Platform enemy1[10];
float enemy1_width = 4.0, enemy1_height = 0.2;

/* --------------------------------- Enemy 3 -------------------------------------------- */
float enemy3_orbit_a = 15/2;                            // semi-major axis (best for feasible gameplay)
float enemy3_orbit_b = (2*screen_boundary - 1)/2 - 0.3; // semi-minor axis (don't modify)
float enemy3_orbit_centre_y = 0.5;                      // centre of game area (don't modify)
Enemy3 enemy3[10];
/* -------------------------------------------------------------------------------------- */


/* ---------------------------------- Screen constants ---------------------------------- */
float screen_boundary = 4, screen_speed = 0.01;
float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;
float camera_rotation_angle = 0;
/*--------------------------------------------------------------------------------------- */

Timer t60(1.0 / 60);

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    glm::vec3 up (0, 1, 0);


    // Compute Camera matrix (view)
    Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    Matrices.view = glm::lookAt(
        glm::vec3(screen_center_x, 0, 3), // eye
        glm::vec3(screen_center_x, 0, 0), // target
        glm::vec3(0, 1, 0)                // up
    ); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    wall.draw(VP);
    player.draw(VP);
    lower_platform.draw(VP);

    // semi_out.draw(VP);
    // semi_in.draw(VP);

    for (int i = 0; i < total_groups; i++) {

        for (int j = 0; j < num_coins; j++)
            if (detect_collision(screen_box, coins[i][j].box))
            if (!coins[i][j].disabled)
                coins[i][j].draw(VP);
        
        
        
        if (detect_collision(screen_box, enemy1[i].box))
            if (!enemy1[i].disabled) enemy1[i].draw(VP);
        if (detect_collision(screen_box, enemy2[i].box))
            if (!enemy2[i].disabled) enemy2[i].draw(VP);

        if (detect_collision(screen_box, special_coin[i].box))
            if (!special_coin[i].disabled)
                special_coin[i].draw(VP);
        
        if (detect_collision(screen_box, magnet[i].box))
            magnet[i].draw(VP);
        
        if (detect_collision(screen_box, enemy3[i].box))
            enemy3[i].draw(VP);
    }

    for (int i = 0; i < (int) balloons.size(); i++)
        balloons[i].draw(VP);

    display_number(VP, player.score, dig_h + dig_w);
    display_number(VP, player.health, 4*(dig_h + dig_w));
    draw_jet(VP);
}

void tick_input(GLFWwindow *window) {
    int left  = glfwGetKey(window, GLFW_KEY_LEFT);
    int right = glfwGetKey(window, GLFW_KEY_RIGHT);
    int up    = glfwGetKey(window, GLFW_KEY_UP);
    int down  = glfwGetKey(window, GLFW_KEY_DOWN);
    int z     = glfwGetKey(window, GLFW_KEY_Z);

    // Left Right Key Check
    if (left) player.dir_x = -1;
    if (right) player.dir_x = 1;
    if (!left and !right) player.dir_x = 0;
    
    // Throw Balloon
    if (z and balloon_cnt == 3) {
        balloons.push_back(Coin(player.box.x + player_width/2 + 0.1, player.box.y, COLOR_MAGENTA, coin_size));
        balloons.back().speed_x *= -1;
    }
    if (balloon_cnt++ == 4)
        balloon_cnt = 0;

    // Up Key Check
    if (up) player.is_jumping = 1;
    else player.is_jumping = 0;

    if (player.health <= 0) {
        quit(window);
    }
}

void tick_elements() {

    /* ------------------- Screen Movement ------------------- */
    screen_center_x += screen_speed;
    screen_box.x = screen_center_x;
    player.position.x += screen_speed;
    /* ------------------------------------------------------- */
    
    // Collisions
    for (int t = 0; t < total_groups; t++) {

        for (int i = 0; i < num_coins; i++) {
            if (!coins[t][i].disabled) {
                if (detect_collision(player.box, coins[t][i].box)) {
                    player.score += coins[t][i].value;
                    coins[t][i].disabled = 1;
                }
            }
        }

        if (!enemy2[t].disabled) {
            if (detect_collision(player.box, enemy2[t].box)) {
                player.health -= 50;
                enemy2[t].disabled = 1;
            }
            bool f = 0;
            for (int i = 0; i < (int) balloons.size(); i++)
                if (detect_collision(enemy2[t].box, balloons[i].box)) {
                    f = 1;
                    enemy2[t].disabled = 1;
                    break;
                }
            if (!f)
                enemy2[t].vertical_shm();
        }


        if (!enemy1[t].disabled) {
            float pts[4][2], pts2[4][2];
            for (int i = 0; i < 4; i++) {
                pts[i][0] = player.box.x + pow(-1, i&1)*player_width/2;
                pts[i][1] = player.box.y + pow(-1, (i >> 1)&1)*player_height/2;

                pts2[i][0] = enemy1[t].box.x + pow(-1, i&1)*enemy1_width/2;
                pts2[i][1] = enemy1[t].box.y + pow(-1, (i >> 1)&1)*enemy1_height/2;
            }

            // cout << "begin\n";
            // for (int i = 0; i < 4; i++)
            //     cout << "(" << pts2[i][0] << ", " << pts2[i][1] << ") ";
            // cout << '\n';

            // for (int i = 0; i < 4; i++) {
            //     rotate_point(pts2[i][0], pts2[i][1], enemy1[t].rotation);
            //     cout << "(" << pts2[i][0] << ", " << pts2[i][1] << ") ";                
            // }
            // cout << '\n';

            // for (int i = 0; i < 4; i++)
            //     cout << "(" << pts[i][0] << ", " << pts[i][1] << ") ";
            // cout << '\n';

            for (int i = 0; i < 4; i++)
                if (rect_point(player.box, 0, pts2[i][0], pts2[i][1])
                    or
                    rect_point(enemy1[t].box, enemy1[t].rotation, pts[i][0], pts[i][1])
                ) {
                    player.health -= 100;
                    enemy1[t].disabled = 1;
                    break;
                }
            
            for (int i = 0; i < 4; i++)
            
            
            for (int i = 0; i < (int) balloons.size(); i++)
                if (detect_collision(enemy1[t].box, balloons[i].box))
                    enemy1[t].disabled = 1;
        }

        if (detect_collision(player.box, enemy3[t].box)) {
            player.health -= 0.01;
        }

        if (detect_collision(screen_box, special_coin[t].box)) {
            if (!special_coin[t].disabled) {
                if (detect_collision(player.box, special_coin[t].box)) {
                    player.score += special_coin[t].value;
                    special_coin[t].disabled = 1;
                }
                special_coin[t].drop();
            }
        }

        enemy3[t].orbit();
    }

    int ind = -1;
    for (int t = 0; t < total_groups; t++) {
        if (player.position.x + player_width/2 > magnet[t].position.x - magnet_width/2
            and
            player.position.x - player_width/2 < magnet[t].position.x + magnet_width/2) {
            
            player.ceiling_level = screen_boundary - magnet_height;
            player.a_y = 9.81;
            player.is_jumping = 0;
            ind = t;
            break;
        } else {
            player.ceiling_level = screen_boundary;
            player.a_y = -9.81;
        }
    }

    if (intersects(semi_out, player.box)) {
        player.in_ring = 1;
        player.dir_x = 0;
        player.is_jumping = 0;
    }

    // Movement Logic
    player.tick();

    for (int i = 0; i < (int) balloons.size(); i++)
        if (!balloons[i].disabled) {
            balloons[i].drop();
        } else {
            balloons.erase(balloons.begin() + i--);
        }

}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    player = Ball(start_x, -screen_boundary + 1.0 + player_height/2, COLOR_RED, (-screen_boundary + 1.0), (screen_boundary));

    lower_platform  = Platform(0, (-screen_boundary + 0.5), COLOR_GREEN, 1, 100*screen_boundary);
    wall            = Platform(0, 0, COLOR_BLUE, 2*screen_boundary, 100*screen_boundary);
    
    // enemy2 = Platform(3, 0, COLOR_RED, 0.2, enemy2_width);
    // enemy2.dir_y = 1;

    // enemy3 = Enemy3(6, enemy3_orbit_centre_y, COLOR_BLACK, enemy3_orbit_a, enemy3_orbit_b); 

    // enemy1 = Platform(8, 0, COLOR_RED, 0.2, enemy1_width);
    // enemy1.rotation = 45;

    // Coin generation
    // float y = gen_rand(-screen_boundary + 3*coin_size + lower_platform.height, screen_boundary - coin_size);
    // std::cout << y << -screen_boundary + 3*coin_size + lower_platform.height << ' ' << screen_boundary - coin_size << '\n';
    // for (int i = 0; i < num_coins; i++) {
    //     if (i < num_coins/2)
    //         coins[0][i] = Coin(0 + i*0.5, y, COLOR_GOLD, coin_size);
    //     else
    //         coins[0][i] = Coin(0 + 0.5*(i - num_coins/2), y - 0.5, COLOR_GOLD, coin_size);
    // }
    

    // magnet = Platform(0, screen_boundary - magnet_height/2, COLOR_BLACK, magnet_height, magnet_width);

    semi_out = Semi(2, 1, COLOR_GOLD, 4);
    semi_in = Semi(2, 1, COLOR_BLUE, 4 - 0.2);

    for (int t = 0; t < total_groups; t++) {

        int m[total_types] = {0}, index[total_types];

        for (int i = 0; i < total_types; i++) {
            int r_ind;
            do {
                r_ind = rand() % total_types;
            } while (m[r_ind]);
            index[i] = r_ind;
            m[r_ind] = 1;
        }

        generate_scene(index, t);
    }


    /* ------------------------------- Jet Propulsion ------------------------- */
    for (int i = 0; i < 10; i++) 
        for (int j = 0; j < 10; j++)
            fire[i][j] = Platform(0, 0, COLOR_BLACK, 0.1, 0.1);
    /* ------------------------------------------------------------------------ */

    init_seven_segs();

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) &&
           (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom;
    float right  = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}

void init_seven_segs() {
    bool temp[][7] = {
        {0, 1, 1, 1, 1, 1, 1}, // 0
        {0, 1, 0, 0, 0, 0, 1}, // 1
        {1, 1, 1, 0, 1, 1, 0}, // 2
        {1, 1, 1, 0, 0, 1, 1}, // 3
        {1, 1, 0, 1, 0, 0, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {0, 1, 1, 0, 0, 0, 1}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1}, // 9
    };
    for (int i = 0; i < 10; i++)
        for (int j = 0; j < 7; j++)
            seven_segs[i][j] = temp[i][j];
}

void display_number(glm::mat4 VP, int value, float y_disp) {
    vector <int> digs;
    for (int t = value; t; t /= 10)
        digs.push_back(t % 10);
    if (!value) digs.push_back(0);
    
    float s_x = -screen_boundary + screen_center_x + dig_h + dig_w/2 + 0.2;
    float s_y = screen_boundary - y_disp - 0.2;
    for (int i = (int) digs.size() - 1; i >= 0; i--) {
        Digit temp = Digit(s_x, s_y, COLOR_RED_BRIGHT, seven_segs[digs[i]]);
        temp.draw(VP);
        s_x += dig_w + 2*dig_h + 0.1;
    }
}

void draw_jet(glm::mat4 VP) {
    float a = 0.1;
    for (int i = 0; i < 10; i++) 
        for (int j = 0; j < 10; j++) {
            fire[i][j].position.x = player.position.x - player_width/2 + (j/10.0 + a/2);
            fire[i][j].position.y = player.position.y - player_height/2 - (i/10.0 + a/2) - a/2;
        }
    
    if (player.is_jumping) {
        for (int i = 0; i < 10; i++)
            if (i <= rows_drawn)
                for (int j = abs(2 - i); j < 10 - abs(2 - i); j++)
                    fire[i][j].draw(VP);
        rows_drawn = (rows_drawn + 1) % 10;
    }
}

float gen_rand(float LO, float HI) {
    return LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)));
}

void generate_scene(int index[], int group_index) {
    for (int j = 0; j < total_types; j++) {
        float y, theta, gap = 0.2;
        switch (index[j])
        {
            case 0: // Coin

                y = gen_rand(ground_y + 4*coin_size, screen_boundary - coin_size);
                for (int i = 0; i < num_coins; i++) {
                    if (i < num_coins/2)
                        coins[group_index][i] = Coin(last_gen + i*0.5, y, COLOR_GOLD, coin_size);
                    else
                        coins[group_index][i] = Coin(last_gen + 0.5*(i - num_coins/2), y - 0.5, COLOR_GOLD, coin_size);
                }

                last_gen += 0.5*num_coins/2;
                break;

            case 1: // Enemy 1

                theta = gen_rand(1, 89);
                y = gen_rand(ground_y + enemy1_width/2, screen_boundary - enemy1_width/2);
                enemy1[group_index] = Platform(last_gen + enemy2_width/2, y, COLOR_RED, enemy1_height, enemy1_width);
                enemy1[group_index].rotation = theta;

                last_gen += enemy1_width;
                break;

            case 2: // Enemy 2

                y = gen_rand(ground_y + enemy2_height, screen_boundary - enemy2_height);
                enemy2[group_index] = Platform(last_gen + enemy2_width/2, y, COLOR_RED, enemy2_height, enemy2_width);
                enemy2[group_index].dir_y = pow(-1, rand() % 2);

                last_gen += enemy2_width;
                break;

            case 3:

                magnet[group_index] = Platform(last_gen + magnet_width/2, screen_boundary - magnet_height/2, COLOR_BLACK, magnet_height, magnet_width);
                last_gen += magnet_width;
            default:
                break;
        }


        last_gen += gap;
    }

    special_coin[group_index] = Coin(5*group_index + rand()%10, 4, COLOR_RED_BRIGHT, 1.5*coin_size);
    special_coin[group_index].value = 500;

    enemy3[group_index] = Enemy3(last_gen + rand() % 6, enemy3_orbit_centre_y, COLOR_RED_DARK, enemy3_orbit_a, enemy3_orbit_b);
}

bool rect_point(bounding_box_t box, double rectRotation, double pointX, double pointY) {
    double rectWidth = box.width, rectHeight = box.height, rectCenterX = box.x, rectCenterY = box.y;

    if(rectRotation == 0)   // Higher Efficiency for Rectangles with 0 rotation.
        return abs(rectCenterX-pointX) < rectWidth/2 and abs(rectCenterY-pointY) < rectHeight/2;
    
    float pts[][2] = {
        {box.x + rectWidth/2, box.y + rectHeight/2},
        {box.x + rectWidth/2, box.y - rectHeight/2},
        {box.x - rectWidth/2, box.y + rectHeight/2},
        {box.x - rectWidth/2, box.y - rectHeight/2}
    };

    for (int i = 0; i < 4; i++) {
        rotate_point_about(pts[i][0], pts[i][1], rectRotation);
    }
    
    bool f = (pointY >= pts[3][1] and pointY <= pts[0][1] and pointX >= pts[2][0] and pointX <= pts[1][0]);
    
    double cs = cos(rectRotation), si = cos(rectRotation);


    double tx = cs*pointX - si*pointY;
    double ty = cs*pointY + si*pointX;
    
    double cx = cs*rectCenterX - si*rectCenterY;
    double cy = cs*rectCenterY + si*rectCenterX;

    double m = tan(rectRotation);
    double mp = -1/m;

    double e1_x = cx + cs*rectWidth/2, e1_y = cy + si*rectWidth/2;
    double e2_x = cx - cs*rectWidth/2, e2_y = cy - si*rectWidth/2;

    double v1 = (cy - e1_y) - mp*(cx - e1_x), v2 = (cy - e2_y) - mp*(cx - e2_x);
    double tv1 = (ty - e1_y) - mp*(tx - e1_x), tv2 = (ty - e2_y) - mp*(tx - e2_x);

    double val = (ty - cy) - m*(tx - cx);
    val /= sqrt(1 + m*m);
    if ((v1*tv1) > 0 and (v2*tv2) > 0) {
        return f or val <= rectHeight/2;
    }

    return 0;
}

void rotate_point_about(float& x, float& y, float theta) {
    float tx = x*cos(theta) - y*sin(theta);
    float ty = x*sin(theta) + y*cos(theta);
    
    x = tx, y = ty;
}

bool intersects(Semi circle, bounding_box_t rect)
{
    float circleDistance_x = abs(circle.position.x - rect.x);
    float circleDistance_y = abs(circle.position.y - rect.y);

    if (circleDistance_x > (rect.width/2 + circle.r)) { return false; }
    if (circleDistance_y > (rect.height/2 + circle.r)) { return false; }

    if (circleDistance_x <= (rect.width/2)) { return true; } 
    if (circleDistance_y <= (rect.height/2)) { return true; }

    float cornerDistance_sq = pow(circleDistance_x - rect.width/2, 2) +
                         pow(circleDistance_y - rect.height/2, 2);

    return (cornerDistance_sq <= pow(circle.r, 2));
}
