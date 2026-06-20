/**
 * @file main.c
 * @brief Modern OpenGL 4.6 Core Profile Demo using C23, GLFW 3.4, and GLAD.
 * Target Environment: Debian 13 (Trixie), GCC 15, KDE Plasma (Wayland)
 * Note on C23: Standard keywords like 'nullptr', 'true', and 'false' are built-in.
 * Note on Headers: GLAD must ALWAYS be included before GLFW.
 */

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

// --- GLSL SHADER SOURCE CODE ---
// Shaders are small programs running directly on the GPU.

// Vertex Shader: Processes each vertex coordinate.
const char* VERTEX_SHADER_SOURCE = "#version 460 core\n"
                                   "layout (location = 0) in vec3 aPos;\n"
                                   "void main() {\n"
                                   "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                   "}\n";

// Fragment Shader: Computes the pixel color output (RGBA).
const char* FRAGMENT_SHADER_SOURCE = "#version 460 core\n"
                                     "out vec4 FragColor;\n"
                                     "void main() {\n"
                                     "    FragColor = vec4(0.2f, 0.6f, 0.9f, 1.0f); // light blue\n"
                                     "}\n";

/**
 * @brief Window resize callback to update the active OpenGL Viewport.
 */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/**
 * @brief Initializes GLFW, configures window hints for Wayland/OpenGL 4.6, and creates the window.
 * @return Pointer to the created GLFWwindow object, or nullptr on failure.
 */
GLFWwindow* init_glfw_window(int width, int height, const char* title)
{
    if (!glfwInit()) {
        fprintf(stderr, "Critical: Failed to initialize GLFW\n");
        return nullptr;
    }

    // Explicitly target Wayland natively, falling back to X11 if needed
    glfwWindowHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

    // Enforce OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "Critical: Failed to create GLFW window.\n");
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}

/**
 * @brief Prints driver and windowing server details to the terminal.
 */
void log_system_info(void)
{
    printf("OpenGL Version:  %s\n", glGetString(GL_VERSION));
    printf("GPU Vendor:      %s\n", glGetString(GL_VENDOR));
    printf("GPU Renderer:    %s\n", glGetString(GL_RENDERER));

    switch (glfwGetPlatform()) {
    case GLFW_PLATFORM_WAYLAND:
        puts("Display Server:  Wayland");
        break;
    case GLFW_PLATFORM_X11:
        puts("Display Server:  X11");
        break;
    default:
        puts("Display Server:  Other/Unknown");
    }
}

/**
 * @brief Compiles individual GLSL Shaders and links them into a complete Program.
 * @return The compiled shader program ID, or 0 on failure.
 */
unsigned int compile_shader_program(void)
{
    int success;
    char infoLog[512];

    // Compile Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VERTEX_SHADER_SOURCE, nullptr);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        fprintf(stderr, "Error: Vertex shader compilation failed:\n%s\n", infoLog);
        return 0;
    }

    // Compile Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FRAGMENT_SHADER_SOURCE, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        fprintf(stderr, "Error: Fragment shader compilation failed:\n%s\n", infoLog);
        return 0;
    }

    // Link Shaders into Program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        fprintf(stderr, "Error: Shader program linking failed:\n%s\n", infoLog);
        return 0;
    }

    // Free immediate resources once linked successfully
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

/**
 * @brief Sets up memory buffers (VAO, VBO) for geometry on the GPU graphics memory.
 */
void setup_geometry(unsigned int* VAO, unsigned int* VBO)
{
    // Normalised Device Coordinates (NDC): X, Y, Z space bounds [-1.0, 1.0]
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // Bottom-Left
        0.5f, -0.5f, 0.0f, // Bottom-Right
        0.0f, 0.5f, 0.0f // Top-Center
    };

    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);

    // Bind Vertex Array Object first to cache configurations
    glBindVertexArray(*VAO);

    // Bind and upload data to the Vertex Buffer Object
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Map vertex attributes (Location 0 points to our vec3 coordinates)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind to prevent accidental modifications
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main(void)
{
    // Core Window Framework Initialization
    GLFWwindow* window = init_glfw_window(800, 600, "OpenGL 4.6 Core - Triangle");
    if (window == nullptr) {
        return EXIT_FAILURE;
    }

    // Dynamic Hardware Pointer Loader via GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Critical: Failed to initialize GLAD\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    log_system_info();

    // State Engine Configurations
    unsigned int shaderProgram = compile_shader_program();
    if (shaderProgram == 0) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    unsigned int VAO, VBO;
    setup_geometry(&VAO, &VBO);

    // Application Rendering Loop
    while (!glfwWindowShouldClose(window)) {
        // Process keyboard inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // Render pass: Clear color buffer with deep gray background
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Active state and invoke primitive draw call
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Display buffer switch and scan window events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Explicit Resource Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return EXIT_SUCCESS;
}
