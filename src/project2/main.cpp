#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>

// constants
const int windowWidth = 640;
const int windowHeight = 480;

const int numOfFoodObjects = 20;
const int numOfMonsters = 4;

// random number generator for colors and starting locations
std::default_random_engine generator;
std::uniform_real_distribution<> colorDistributor(0.0,1.0);
std::uniform_real_distribution<> pointDistribution(-1.0,1.0);

// define our classes
class Color {
    public:
        float r, g, b;

        Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

class PacMan {
    private:
        int xPos, yPos;

    public:
        PacMan(int xPos, int yPos) : xPos(xPos), yPos(yPos) {};

        void move(int x, int y) {
            xPos += x;
            yPos += y;
        }

        void draw() {
            float radius = 20.0f;
            int segments = 60;

            glBegin(GL_TRIANGLE_FAN);
            glColor3f(1.0f, 1.0f, 0.0f); // yellow cause pacman yellow
            glVertex2f(xPos, yPos); 

            for (int i = 0; i <= segments; i++) {
                float theta = 2.0f * 3.1415926f * float(i) / float(segments);
                
                // skip the mouth area
                // 22.5 degrees and 337.5 degrees as radians
                if(!(theta > 0.392 && theta < 5.89)) continue;

                float x = radius * cosf(theta);
                float y = radius * sinf(theta);

                glVertex2f(xPos + x, yPos + y);
            }

            glEnd();
        }
};

class Ghost {
    private:
        Color color;
        int xPos, yPos;
    
    public:
        Ghost(Color color, int xPos, int yPos) : color(color), xPos(xPos), yPos(yPos) {};

        void draw() {
            // for square
            int width = 200;
            int height = 200;

            // for top circle
            int segments = 10;
            float radius = width / 2;

            // fpr triangles

            glBegin(GL_POLYGON);
            glColor3f(color.r, color.g, color.b);

            glVertex2f(xPos - radius, (yPos + radius) - 50); // bottom left
            glVertex2f(xPos - radius, yPos - radius); // top left


            // render the top circle
            for (int i = 0; i <= segments; i++) {
                float theta = 2.0f * 3.1415926f * float(i) / float(segments);

                if(theta < 3.141) continue;

                float x = radius * cosf(theta);
                float y = radius * sinf(theta);

                glVertex2f(xPos + x, (yPos - radius) + y);
            }

            glVertex2f(xPos + radius, yPos + radius); // bottom right

            // render the triangles at the bottom
            // we draw 5 points, each equally spaced apart
            int right = xPos + radius;
            int bottom = yPos + radius;

            int numTriangles = 6;
            float stepWidth = width / numTriangles;

            // since we are on the right, we start right and move left
            // so, our loop order is reversed
            //for (int i = numTriangles; i < 0; i--) {
            for (int i = 1; i <= numTriangles; i++) {

                float x = right - (float)(i * stepWidth);
                float y = (i % 2 == 0) ? bottom : bottom - 50;



                glVertex2f(x, y);

            }

            // render final point for bottom left of triangle
            glVertex2f(xPos - radius, yPos + radius);

            glEnd();
        }
};

class Food {
    private:
        Color color = Color(255, 105, 180);
        int xPos, yPos;

    public:
        Food(int xPos, int yPos) : xPos(xPos), yPos(yPos) {};

        void draw() {
            float radius = 5.0f;
            int segments = 10;

            glBegin(GL_TRIANGLE_FAN);
            glColor3f(color.r, color.g, color.b);
            glVertex2f(xPos, yPos); 

            for (int i = 0; i <= segments; i++) {
                float theta = 2.0f * 3.1415926f * float(i) / float(segments);

                float x = radius * cosf(theta);
                float y = radius * sinf(theta);

                glVertex2f(xPos + x, yPos + y);
            }

            glEnd();
        }
};

// main function 
int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Project2", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // TODO: make start at random spot
    PacMan pacman(320, 240);

    // TODO: generate bunch of food at random spots
    Food food(350, 200);

    Ghost ghost1(Color(255, 0, 0), 450, 375);

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

        ghost1.draw();

        pacman.draw();
        food.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
};