#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>

// constants
const int windowWidth = 640;
const int windowHeight = 480;
const int windowOffset = 25;

const int numOfFoodObjects = 20;
const int numOfMonsters = 4;
const int movementSpeed = 50;

// random number generator for colors and starting locations
std::default_random_engine generator;

// gen for colors
std::uniform_real_distribution<> colorDistributor(0.0,1.0);

// gen for starting locations
std::uniform_int_distribution<> widthDistribution(0 + windowOffset, windowWidth - windowOffset);
std::uniform_int_distribution<> heightDistribution(0 + windowOffset, windowHeight - windowOffset);

// gen for movements
std::uniform_real_distribution<> movementDuration(0.0f, 3.5f);
std::uniform_int_distribution<> direction(-1, 1);

// define our classes
class Color {
    public:
        float r, g, b;

        Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

class PacMan {
    private:
        float xPos, yPos;

    public:
        PacMan(float xPos, float yPos) : xPos(xPos), yPos(yPos) {};

        void move(float x, float y) {
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

        float xPos, yPos;

        float directionX, directionY = 0.0f;
        float directionTimer = 0.0f;
    
    public:
        Ghost(Color color, float xPos, float yPos) : color(color), xPos(xPos), yPos(yPos) {};

    void draw() {
        float width = 30.0f; 
        float radius = width / 2.0f;
        int segments = 10; 

        glBegin(GL_TRIANGLE_FAN);
        glColor3f(color.r, color.g, color.b);

        glVertex2f(xPos, yPos); 
        glVertex2f(xPos - radius, yPos + radius); // bottom left
        glVertex2f(xPos - radius, yPos - radius); // top left

        // render the top dome of the ghost
        for (int i = 0; i <= segments; i++) {
            float theta = 3.14159f + (float(i) / (float)segments) * 3.14159f;
            float x = radius * cosf(theta);
            float y = radius * sinf(theta);
            glVertex2f(xPos + x, (yPos - radius) + y);
        }

        // render bottom right endpoint of the square
        glVertex2f(xPos + radius, yPos + radius);

        int numTriangles = 4; 
        int totalSteps = numTriangles * 2; 
        float stepWidth = width / (float)totalSteps;

        // render the zig-zag at the bottom
        for (int i = 1; i <= totalSteps; i++) {
            float x = (xPos + radius) - (i * stepWidth);
            float y = (i % 2 != 0) ? (yPos + radius + (width / 5)) : (yPos + radius);
            glVertex2f(x, y);
        }

        // close the loop by ending at the same point we started with
        glVertex2f(xPos - radius, yPos + radius);

        glEnd();
    }

    void update(float timeDelta) {
        directionTimer -= timeDelta;

        // if the timer is up, generate a new random direction and reset the timer
        if(directionTimer <= 0) {
            directionX = direction(generator);
            directionY = direction(generator);
            directionTimer = movementDuration(generator);
        }

        xPos += directionX * timeDelta * movementSpeed; // speed of 5 pixels per second
        yPos += directionY * timeDelta * movementSpeed;

        // keep the ghost within the window bounds (wrapping around)
        if(xPos > windowWidth) {
            xPos = windowOffset;
        } else if (xPos < windowOffset) {
            xPos = windowWidth;
        }

        if(yPos > windowHeight - windowOffset) {
            yPos = windowOffset;
        } else if (yPos < windowOffset) {
            yPos = windowHeight - windowOffset;
        }
    }
};

class Food {
    private:
        Color color = Color(255, 105, 180);

        float xPos, yPos;

        float directionX, directionY = 0.0f;
        float directionTimer = 0.0f;

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

        // same func as ghost
        // food moves slower though
        void update(float timeDelta) {
            directionTimer -= timeDelta;

            // if the timer is up, generate a new random direction and reset the timer
            if(directionTimer <= 0) {
                directionX = direction(generator);
                directionY = direction(generator);
                directionTimer = movementDuration(generator);
            }

            xPos += directionX * timeDelta * (float)(movementSpeed / 3); // speed of 5 pixels per second
            yPos += directionY * timeDelta * (float)(movementSpeed / 3);

            // keep the food within the window bounds (wrapping around)
            if(xPos > windowWidth ) {
                xPos = windowOffset;
            } else if (xPos < 0) {
                xPos = windowWidth;
            }

            if(yPos > windowHeight) {
                yPos = windowOffset;
            } else if (yPos < 0) {
                yPos = windowHeight;
            }
        }
};


std::vector<Ghost> generateGhosts() {
    std::vector<Ghost> ghosts = {};

    for (int i = 0; i < numOfMonsters; i++) {
        ghosts.push_back(Ghost(Color(colorDistributor(generator), colorDistributor(generator), colorDistributor(generator)), widthDistribution(generator), heightDistribution(generator)));
    }

    return ghosts;
};

std::vector<Food> generateFoods() {
    std::vector<Food> foods = {};

    for (int i = 0; i < numOfFoodObjects; i++) {
        foods.push_back(Food(widthDistribution(generator), heightDistribution(generator)));
    }

    return foods;
};

// main function 
int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Project2", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    PacMan pacman(widthDistribution(generator), heightDistribution(generator));

    std::vector<Ghost> ghosts = generateGhosts();
    std::vector<Food> foods = generateFoods();

    double lastTime = glfwGetTime();

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

        pacman.draw();
        
        for (auto& food : foods) {
            food.draw();
            food.update(deltaTime);
        }

        for (auto& ghost : ghosts) {
            ghost.draw();
            ghost.update(deltaTime);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        lastTime = currentTime;
    }

    glfwTerminate();
    return 0;
};