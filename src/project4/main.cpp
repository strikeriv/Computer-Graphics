#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <random>

// sets up stb_image
// i could not get im_color to work whatsoever so I opted to use this
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// struct for objects
struct SceneObject
{
    float px, py, pz;
    float vx, vy, vz;
    float ax, ay, az;

    GLuint textureID;
};

std::vector<SceneObject> scene;

// loads the texture (duh)
GLuint loadTexture(const char *path)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // parameters for project required by the instructions
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // converts the texture into data we need to format the texture
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

    // map into a texture
    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_image_free(data);

    return texID;
}

int main()
{
    if (!glfwInit())
        return -1;

    GLFWwindow *window = glfwCreateWindow(800, 600, "Project 4", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // create a scene object w/ a texture and put into scene
    SceneObject sq;
    sq.px = 0;
    sq.py = 0;
    sq.pz = -5;
    sq.vx = 0.4f;
    sq.vy = 0.2f;
    sq.ay = 0;
    sq.textureID = loadTexture("../textures/brick0.jpg");
    scene.push_back(sq);

    float lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        // make the square rotate
        float now = glfwGetTime();
        float dt = now - lastTime;
        lastTime = now;

        // loops through the scene objects and rotates it by a little every frame
        for (auto &obj : scene)
        {
            obj.px += obj.vx * dt;
            obj.py += obj.vy * dt;
            obj.ay += 50.0f * dt;
            if (obj.px > 2.0f || obj.px < -2.0f)
                obj.vx *= -1;
            if (obj.py > 1.5f || obj.py < -1.5f)
                obj.vy *= -1;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // set up the aspect ration for viewing
        float aspect = 800.0f / 600.0f;
        glFrustum(-aspect * 0.1, aspect * 0.1, -0.1, 0.1, 0.1, 100.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // draw a square to represent each object within the scene
        for (const auto &obj : scene)
        {
            glBindTexture(GL_TEXTURE_2D, obj.textureID);
            glPushMatrix();

            glTranslatef(obj.px, obj.py, obj.pz);
            glRotatef(obj.ay, 0, 1, 0);
            glBegin(GL_QUADS);

            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-1.0f, -1.0f, 0.0f);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(1.0f, -1.0f, 0.0f);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(1.0f, 1.0f, 0.0f);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-1.0f, 1.0f, 0.0f);

            glEnd();
            glPopMatrix();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}