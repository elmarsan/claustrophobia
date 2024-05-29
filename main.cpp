#include <cassert>
#include <cmath>
#include "math.h"
#include "shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

using namespace claustrophobia;

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int screenWidth = 1200;
int screenHeight = 800;

// camera
const float cameraPosY = 0.5f;
vec3 cameraPos{2.0f, cameraPosY, -3.0f};
vec3 cameraFront{0.0f, 0.0f, -1.0f};
vec3 cameraUp{0.0f, 1.0f, 0.0f};

bool firstMouse = true;
float yaw = -90.0f;  // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to
                     // the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = static_cast<float>(screenWidth) / 2.0;
float lastY = static_cast<float>(screenHeight) / 2.0;
float fov = 45.0f;

const float perspectiveNear = 0.1f;
const float perspectiveFar = 100.0f;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// mechanics
bool jumping = false;
const float jumpYLimit = 2.0f;
const float jumpVelocity = 4.5f;
const float cameraVelocity = 10.5;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL claustrophobia", nullptr, nullptr);
    assert(window != nullptr && "Failed to create GLFW Window");

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) && "Failed to initialize GLAD");

    glEnable(GL_DEPTH_TEST);

    // tell stb_image.h to flip loaded texture's on the y-axis.
    stbi_set_flip_vertically_on_load(true);

    GLuint floorTexture1, floorTexture2, wallTexture1;
    glGenTextures(1, &floorTexture1);
    glGenTextures(1, &floorTexture2);
    glGenTextures(1, &wallTexture1);

    ///////////////////// FLOOR TEXTURE 1 /////////////////////
    int width, height, nrChannels;
    glBindTexture(GL_TEXTURE_2D, floorTexture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto* data = stbi_load("./resources/floor_1.png", &width, &height, &nrChannels, 0);
    assert(data && "Failed to load texture 'floor_1.png'");
    glBindTexture(GL_TEXTURE_2D, floorTexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    ///////////////////////////////////////////////////////////

    ///////////////////// FLOOR TEXTURE 2 /////////////////////
    glBindTexture(GL_TEXTURE_2D, floorTexture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("./resources/floor_2.jpg", &width, &height, &nrChannels, 0);
    assert(data && "Failed to load texture 'floor_2.jpg'");
    glBindTexture(GL_TEXTURE_2D, floorTexture2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    ///////////////////////////////////////////////////////////

    ///////////////////// WALL TEXTURE 1 /////////////////////
    glBindTexture(GL_TEXTURE_2D, wallTexture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("./resources/wall_1.jpg", &width, &height, &nrChannels, 0);
    assert(data && "Failed to load texture 'wall_1.jpg'");
    glBindTexture(GL_TEXTURE_2D, wallTexture1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    ///////////////////////////////////////////////////////////
    Shader shader{"rect.vert", "rect.frag"};

    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // top right
        0.5f,  -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,  // bottom left
        -0.5f, 0.5f,  0.0f, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    /////////////////////////////////////////////////////////

    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture1);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Jumping
        if (jumping)
        {
            const float jumpSpeed = jumpVelocity * deltaTime;
            cameraPos.y += jumpSpeed;

            if (cameraPos.y >= jumpYLimit)
            {
                jumping = false;
            }
        }
        // Falling
        if (!jumping && cameraPos.y > cameraPosY)
        {
            const float jumpSpeed = jumpVelocity * deltaTime;
            cameraPos.y -= jumpSpeed;

            // Adjust cameraY when landing
            if (cameraPos.y < cameraPosY)
            {
                cameraPos.y = cameraPosY;  // Ensure camera is at correct position
            }
        }

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        auto view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);

        auto proj = perspective(radians(fov), float(screenWidth) / float(screenHeight),
                                          perspectiveNear, perspectiveFar);
        shader.setMat4("proj", proj);

        mat4 trans;
        mat4 scaleM;
        mat4 rot;
        mat4 model;

        /////////////////////////  WALLS /////////////////////////
        glBindTexture(GL_TEXTURE_2D, wallTexture1);

        for (int i = 0; i < 7; i++)
        {
            // Left wall
            trans = translate(mat4{1.0f}, vec3{-0.3f, 0.8f, i * -5.0f});
            rot = rotate(mat4{1.0f}, radians(90.0f), vec3{0, 1.0f, 0});
            scaleM = scale(mat4{1.0f}, vec3{5.0f, 5.5f, 0});
            model = trans * rot * scaleM;

            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // Right wall
            trans = translate(trans, vec3{10.0f, 0.8f, i * -5.0f});
            model = trans * rot * scaleM;
            shader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // Far wall
        trans = translate(mat4{1.0f}, vec3{4.5f, 0.8f, -32.0f});
        scaleM = scale(mat4{1.0f}, vec3{12.0f, 5.5f, 1.0f});
        model = trans * scaleM;
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Near wall
        trans = translate(mat4{1.0f}, vec3{4.5f, 0.8f, 1.0f});
        scaleM = scale(mat4{1.0f}, vec3{12.0f, 5.5f, 1.0f});
        model = trans * scaleM;
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        //////////////////////////////////////////////////////////

        /////////////////////////  CEILING ///////////////////////
        glBindTexture(GL_TEXTURE_2D, floorTexture1);
        trans = translate(mat4{1.0f}, vec3{4.5f, 3.5f, -16.0f});
        rot = rotate(mat4{1.0f}, radians(-90.f), vec3{1.0f, 0, 0});
        scaleM = scale(mat4{1.0f}, vec3{12.0f, 40.0f, 1.0f});
        model = trans * rot * scaleM;
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //////////////////////////////////////////////////////////

        /////////////////////////  FLOOR /////////////////////////
        glEnableVertexAttribArray(1);
        glBindTexture(GL_TEXTURE_2D, floorTexture1);

        trans = translate(mat4{1.0f}, vec3{4.5f, -1.0f, -16.0f});
        rot = rotate(mat4{1.0f}, radians(-90.0f), vec3{1.0f, 0, 0});
        scaleM = scale(mat4{1.0f}, vec3{12.0f, 34.5f, 1.0f});
        model = trans * rot * scaleM;
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        /////////////////////////////////////////////////////////

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    const float cameraSpeed = cameraVelocity * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!jumping && cameraPos.y == cameraPosY)
        {
            jumping = true;
        }

        return;
    }

    // Disable other key checks while jumping/falling
    if (jumping || (!jumping && cameraPos.y > cameraPosY))
    {
        return;
    }

    // Moving forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    // Moving backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    // Moving through negative x-axis
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        auto cameraRight = cameraFront.cross(cameraUp).normalize();
        cameraPos -= cameraRight * cameraSpeed;
    }
    // Moving through positive x-axis
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        auto cameraRight = cameraFront.cross(cameraUp).normalize();
        cameraPos += cameraRight * cameraSpeed;
    }
    cameraPos.y = cameraPosY;
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.17f;  // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vec3 dir{};
    dir.x = cos(radians(yaw)) * cos(radians(pitch));
    dir.y = sin(radians(pitch));
    dir.z = sin(radians(yaw)) * cos(radians(pitch));
    cameraFront = dir.normalize();
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}
