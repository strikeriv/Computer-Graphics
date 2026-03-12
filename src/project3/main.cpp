#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>

const int windowWidth = 1920;
const int windowHeight = 1080;

const int gridWidth = 250;
const int gridHeight = 250;

const int maxHeight = gridHeight / 16;

std::default_random_engine generator;
std::uniform_real_distribution<> noiseDistribution(0.0, 5);

class Point {
    public:
        float x, y, z;

        Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Polygon {
    private:
        std::vector<Point> vertices;
    
    public:
        void draw() {
            // AI generated code for colors
            float avgHeight = 0;
            for(const auto& v : vertices) avgHeight += v.y;
            avgHeight /= 4.0f;

            // 2. Define our "Biome" colors
            if (avgHeight <= 0.5f) {
                // Water Bed
                glColor3f(0.0f, 0.7f, 1.0f); // Water Blue
             } else if (avgHeight <= 6.0f) {
                // Deep Valley
                glColor3f(0.1f, 0.4f, 0.1f); // Very Dark Green
            } else if (avgHeight <= 24.0f) {
                // Lush Lowlands
                glColor3f(0.34f, 0.7f, 0.3f); // Grass Green
            } else if (avgHeight <= 30.0f) {
                // High Peaks
                glColor3f(0.45f, 0.38f, 0.26f); // Mountain Rock Brown
            } else {
                // Snow Caps (only for the very highest points)
                glColor3f(0.95f, 0.95f, 1.0f); 
            }

            glBegin(GL_QUADS);
            for (auto& vertex : vertices) {
                Point* vPtr = &vertex;

                glVertex3f(vertex.x, vertex.y, vertex.z);
            }
            glEnd();
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
};

std::vector<std::vector<float>> xCoords, yCoords, zCoords, smoothedYCoords;

void generateTerrainGrid() {
    for(int x = 0; x < gridWidth; x++) {
        for(int y = 0; y < gridHeight; y++) {
            xCoords[x][y] = x;
            zCoords[x][y] = y;

            float offsetX = rand() % 1000;
            float offsetY = rand() % 1000;

            // generate some mountains
            double mountains = (sin((x + offsetX) * 0.01) + cos((y + offsetY) * 0.015)) * 35.0f;

            // generate some hills (less sin / cos than mountains so smaller (duh))
            double hills = (sin(x * 0.06) + cos(y * 0.08)) * 8.0f;

            // add in some noise for some variation
            float noise = noiseDistribution(generator);

            // then add it all together and add it to yCoords
            float totalHeight = (float)(mountains + hills) + noise;

            yCoords[x][y] = totalHeight;
        }
    }
}

float summateTerrainGridNeighbors(int gridX, int gridZ, int radius = 1) {
    int heightCount = 0;
    float heights = 0;

    for (int dx = -radius; dx <= radius; dx++) {
        const int x = gridX + dx;
        if(x < 0 || x >= gridWidth) {
            continue;
        }

        for (int dz = -radius; dz <= radius; dz++) {
            const int z = gridZ + dz;
            if(z < 0 || z >= gridHeight) {
                continue;
            }

            // grid values are guarenteed to be safe now, so we can grab the value
            heightCount++;
            heights += yCoords[x][z];
        }
    }

    return heights / heightCount;
}

void smoothTerrainGrid() {
    for(int x = 0; x < gridWidth; x++) {
        for(int z = 0; z < gridHeight; z++) {
            // 3x3 approach
            // grab neighbors, and summate their heights
            // plus, we cap the bottom so we have a smooth surface down there
            float neighborSummation = summateTerrainGridNeighbors(x, z);
            if(neighborSummation < 7.5f) { 
                neighborSummation = 0.0f;
            }

            smoothedYCoords[x][z] = neighborSummation;
        }
    };

    // set the actual values now after smoothing has completed
    yCoords = smoothedYCoords;
}

std::vector<Polygon> generatePolygonsFromTerrainGrid() {
    std::vector<Polygon> polygons = {};

    int coordsSize = xCoords.size();

    // must -1 here since 100 grid size -> 99 polygons
    // prevents an inaccessable grid error
    for(int x = 0; x < coordsSize - 1; x++) {
        for(int z = 0; z < coordsSize - 1; z++) {
            Polygon polygon = Polygon();

            // we grab the points around it (4 in total for rectangle)
            // then we add that vertex to the polygon
            // we must go in a clockwise order so the points join together properly
            polygon.addVertex({ xCoords[x][z], yCoords[x][z], zCoords[x][z] });
            polygon.addVertex({ xCoords[x + 1][z], yCoords[x + 1][z], zCoords[x + 1][z] });
            polygon.addVertex({ xCoords[x + 1][z + 1], yCoords[x + 1][z + 1], zCoords[x + 1][z + 1] });
            polygon.addVertex({ xCoords[x][z + 1], yCoords[x][z + 1], zCoords[x][z + 1] });

            polygons.push_back(polygon);
        }
    }

    return polygons;
}

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Project3", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    float rotationX, rotationY, rotationZ = 0.0;

    // set the sizes of the vectors on load
    xCoords.resize(gridWidth, std::vector<float>(gridHeight));
    yCoords.resize(gridWidth, std::vector<float>(gridHeight));
    zCoords.resize(gridWidth, std::vector<float>(gridHeight));
    smoothedYCoords.resize(gridWidth, std::vector<float>(gridHeight));

    // generate terrain
    generateTerrainGrid();

    // smooth the terrain
    // we call multiple times to smooth a bunch
    smoothTerrainGrid();
    smoothTerrainGrid();
    smoothTerrainGrid();

    // generate polygons from the grid
    std::vector<Polygon> polygons = generatePolygonsFromTerrainGrid();

    glEnable(GL_DEPTH_TEST);

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // uncomment the following for wireframe mode
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glViewport(0, 0, windowWidth, windowHeight);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // calculate window size based on grid
        // this centers the terrain within the viewport
        float margin = 1.2f;
        float halfW = (gridWidth / 2.0f) * margin;
        float halfH = (gridHeight / 2.0f) * margin;

        glOrtho(-halfW, halfW, -halfH, halfH, -1000, 1000);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // move the camera away so we can actually see the terrain
        glTranslatef(0, 0, -500);
        glRotatef(30, 1, 0, 0);

        // listen for XYZ input to rotate the grid
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            rotationX += gridHeight / 100;
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            rotationY += gridHeight / 100;
        }
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            rotationZ += gridHeight / 100;
        }

        // rotate using rotation variables
        glRotatef(rotationX, 1, 0, 0);
        glRotatef(rotationY, 0, 1, 0);
        glRotatef(rotationZ, 0, 0, 1);

        glTranslatef((-gridWidth / 2), 0, (-gridHeight / 2));

        for (auto& polygon : polygons) {
            polygon.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
};