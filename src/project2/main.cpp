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
const int baseMovementSpeed = 50;

int score = 0;

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

        float radius = 20.0f;
        float mouthSize = 0.5f;
        float mouthSpeed = 10.0f;
        float directionX, directionY = 0.0f;

    public:
        PacMan(float xPos, float yPos) : xPos(xPos), yPos(yPos) {};

        float getX() {
            return xPos;
        }

        float getY() {
            return yPos;
        }

        float getRadius() {
            return radius;
        }

        void move(float x, float y) {
            xPos += x;
            yPos += y;

            // keep track of the direction for the mouth animation
            directionX = 0.0f;
            directionY = 0.0f;

            if(x > 0) {
                directionX = 1.0f;
            } else if (x < 0) {
                directionX = -1.0f;
            } else if (y > 0) {
                directionY = 1.0f;
            } else if (y < 0) {
                directionY = -1.0f;
            }

            // keep pacman within the window bounds (wrapping around)
            if(xPos > windowWidth) {
                xPos = windowOffset;
            } else if (xPos < windowOffset) {
                xPos = windowWidth;
            }

            if(yPos > windowHeight) {
                yPos = windowOffset;
            } else if (yPos < windowOffset) {
                yPos = windowHeight;
            }
        }

        void draw(float currentTime) {
            int segments = 30;

            glBegin(GL_TRIANGLE_FAN);
            glColor3f(1.0f, 1.0f, 0.0f); // yellow cause pacman yellow
            glVertex2f(xPos, yPos); 

            for (int i = 0; i <= segments; i++) {
                float theta = 2.0f * 3.1415926f * float(i) / float(segments);
                bool inMouth = false;

                // determine if the current vertex is within the mouth opening
                // only render on specific frames though, to make it look like the mouth is opening and closing
                if((int)(currentTime * mouthSpeed) % 2 == 0) {
                    if (directionX > 0) {
                        if (theta < mouthSize || theta > (2.0f * 3.14159f - mouthSize)) {
                            inMouth = true;
                        }
                    } else if (directionX < 0) {
                        if (theta > (3.14159f - mouthSize) && theta < (3.14159f + mouthSize)) {
                            inMouth = true;
                        }
                    } else if (directionY > 0) {
                        if (theta > (1.5708f - mouthSize) && theta < (1.5708f + mouthSize)) {
                            inMouth = true;
                        }
                    } else if (directionY < 0) {
                        if (theta > (4.71239f - mouthSize) && theta < (4.71239f + mouthSize)) {
                            inMouth = true;
                        }
                    }
                }

                // if we are in the mouth, we change the position of the vertex
                // this lets the vertexes that were not rendered not draw lines in between themselves
                // because opengl weird like that.. idk
                if (inMouth) {
                    glVertex2f(xPos, yPos); 
                } else {
                    float x = radius * cosf(theta);
                    float y = radius * sinf(theta);
                    glVertex2f(xPos + x, yPos + y);
                }
            }

            glEnd();
        }
};

class Ghost {
    private:
        Color color;

        float xPos, yPos;

        float width = 30.0f; 
        float radius = width / 2.0f;

        float directionX, directionY = 0.0f;
        float directionTimer = 0.0f;
    
    public:
        Ghost(Color color, float xPos, float yPos) : color(color), xPos(xPos), yPos(yPos) {};

        float getX() {
            return xPos;
        }

        float getY() {
            return yPos;
        }

        float getRadius() {
            return radius;
        }

        void draw() {
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

            xPos += directionX * timeDelta * baseMovementSpeed;
            yPos += directionY * timeDelta * baseMovementSpeed;

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

        float radius = 5.0f;
        float xPos, yPos;

        float directionX, directionY = 0.0f;
        float directionTimer = 0.0f;

    public:
        Food(int xPos, int yPos) : xPos(xPos), yPos(yPos) {};

        float getX() {
            return xPos;
        }

        float getY() {
            return yPos;
        }

        float getRadius() {
            return radius;
        }

        void draw() {
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

            xPos += directionX * timeDelta * (float)(baseMovementSpeed / 3);
            yPos += directionY * timeDelta * (float)(baseMovementSpeed / 3);

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

// check the collision of two circles (used for pacman, food and the ghosts)
// uses the pythagorean theorem to determine the distance between the centers of the two circles
// if the distance between the two is less than the sum of the radii, then there is a collision
bool checkCollision(float x1, float y1, float r1, float x2, float y2, float r2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distance = sqrt(dx * dx + dy * dy);

    return distance < (r1 + r2);
}

PacMan pacman(widthDistribution(generator), heightDistribution(generator));

std::vector<Ghost> ghosts = generateGhosts();
std::vector<Food> foods = generateFoods();

// main function 
int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Project2", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    double lastTime = glfwGetTime();
    int pacmanMovementSpeed = baseMovementSpeed * 1.25;

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = (float)(currentTime - lastTime);
        
        glClear(GL_COLOR_BUFFER_BIT);
        
        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0, windowWidth, windowHeight, 0, -1, 1);

        // listen for WASD input to move pacman
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            pacman.move(0, -pacmanMovementSpeed * deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            pacman.move(0, pacmanMovementSpeed * deltaTime);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            pacman.move(-pacmanMovementSpeed * deltaTime, 0);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            pacman.move(pacmanMovementSpeed * deltaTime, 0);
        }

        // check for collisions between pacman and the food
        for (auto it = foods.begin(); it != foods.end();) {
            if (checkCollision(pacman.getX(), pacman.getY(), pacman.getRadius(), it->getX(), it->getY(), it->getRadius())) {
                it = foods.erase(it);
                score += 10;
            } else {
                ++it;
            }
        }

        // check for collisions between pacman and the ghosts
        // isn't perfect since the ghosts are not perfect circles, but who cares
        // if we collide with a ghost, we end the game by setting the window to close
        // i'm way too lazy to implement something cool here, sorry TAs!
        for (auto& ghost : ghosts) {
            if (checkCollision(pacman.getX(), pacman.getY(), pacman.getRadius(), ghost.getX(), ghost.getY(), 15.0f)) {
                std::cout << "Game Over!" << std::endl;
                std::cout << "Score: " << score << std::endl;

                glfwWaitEventsTimeout(6.0); // wait a little before closing the window so the player can see the collision
                glfwSetWindowShouldClose(window, true);
            }
        }

        // check to see if any food is left, if not we win and end the game
        if(foods.size() == 0) {
            std::cout << "You Win!" << std::endl;
            std::cout << "Score: " << score << std::endl;

            glfwWaitEventsTimeout(6.0); // wait a little before closing so they can savor the win
            glfwSetWindowShouldClose(window, true);
        }

        // render the food, pacman and the ghosts
        // in a specific order to make sure food is behind pacman, and pacman is behind the ghosts
        // idk I watch a video on pacman and that's how it looked like so we ball
        for (auto& food : foods) {
            food.draw();
            food.update(deltaTime);
        }

        pacman.draw(currentTime);
        
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