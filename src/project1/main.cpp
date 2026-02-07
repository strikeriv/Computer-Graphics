#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>

// constants
const int numOfPolygons = 10;
const int maxNumOfPoints = 5;

// random number generator for colors and points
std::default_random_engine generator;
std::uniform_real_distribution<> colorDistributor(0.0,1.0);
std::uniform_real_distribution<> pointDistribution(-1.0,1.0);

// define our classes
class Point {
    public:
        float x, y, z;

        Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Color {
    public:
        float r, g, b;

        Color(float r, float g, float b) : r(r), g(g), b(b) {}
};

class Polygon {
    private:
        std::vector<Point> vertices;
        Color color;
    
    public:
        Polygon(Color color, std::vector<Point> vertices) : color(color), vertices(vertices) {};

        void display(GLfloat angle) {
            glPushMatrix();
            
            glRotatef(angle, 0.0f, 1.0f, 0.0f);
            glColor3f(color.r, color.g, color.b);

            glBegin(GL_POLYGON);
            for (auto& vertex : vertices) {
                Point* vPtr = &vertex;

                glVertex3f(vertex.x, vertex.y, vertex.z);
            }
            glEnd();

            glPopMatrix();
        }

        void printPolygonInformation() {
            for (auto& vertex : vertices) {
                std::cout << "Vertex: (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")\n";
            }

            std::cout << "Color: (" << color.r << ", " << color.g << ", " << color.b << ")\n";
        }

        void addVertex(Point p) {
            vertices.push_back(p);
        }

        void updateVertex(int index, Point p) {
            if (index >= 0 && index < vertices.size()) {
                vertices[index] = p;
            } else {
                std::cerr << "Index out of bounds\n";
            }
        }
        
        void setColor(Color c) {
            color = c;
        }
};

// helper functions to generate the polygons
Color generateRandomColor() {
    return Color(colorDistributor(generator), colorDistributor(generator), colorDistributor(generator));
};

Point generateRandomPoint() {
    return Point(pointDistribution(generator), pointDistribution(generator), pointDistribution(generator));
};

std::vector<Point> generateRandomPoints() {
    std::vector<Point> points = {};

    for (int i = 0; i < maxNumOfPoints; ++i) {
        points.push_back(generateRandomPoint());
    };

    return points;
}

std::vector<Polygon> generatePolygons() {
    std::vector<Polygon> polygons = {};

    for (int i = 0; i < numOfPolygons; ++i) {
        polygons.push_back(Polygon(generateRandomColor(), generateRandomPoints()));
    }

    return polygons;
};

GLfloat angle = 0.0f;
std::vector<Polygon> randomPolygons = generatePolygons();

// main function 
int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "Project1", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // print polygon info at the start of the program
    for (auto& polygon : randomPolygons) {
        polygon.printPolygonInformation();
    }

    // render the polygons
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // increment the angle to then be passed to display function to rotate
        angle += 0.1f;
        for (auto& polygon : randomPolygons) {
            polygon.display(angle);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
};