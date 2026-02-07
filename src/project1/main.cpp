#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Test", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        // Draw things here
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}