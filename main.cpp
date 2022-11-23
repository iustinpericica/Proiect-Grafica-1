#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

//////////////////////////////////////

GLFWwindow* Window;

using namespace std;

GLuint
        VaoId,
        VboId,
        ColorBufferId,
        ProgramId,
        myMatrixLocation,
        matrScaleLocation,
        matrTranslLocation,
        matrRotlLocation,
        codColLocation;
GLfloat
    xStart = 0, xEnd = 100,
    yStart = 0, yEnd = 100;

int codCol;
float PI = 3.141592, angle = 0;
float tx = 0; float ty = 0;
float width = 450, height = 300;
float i = 0.0, alpha = 0.0, step=1, beta = 0.002;
glm::mat4
        ResizeMat, myMatrix, resizeMatrix, matrTransl1, matrTransl2, matrTransl3, matrScale1, matrScale2, matrRot, matrDepl;
glm::vec3 center;

const char* VertexShaderSource = R"SHADER-SOURCE(
// Shader-ul de varfuri
#version 330 core

layout (location = 0) in vec4 in_Position;
layout (location = 1) in vec4 in_Color;

out vec4 ex_Color;
uniform mat4 myMatrix;

void main(void)
{
    gl_Position = myMatrix * in_Position;
    ex_Color = in_Color;
}
)SHADER-SOURCE";

const char* FragmentShaderSource = R"SHADER-SOURCE(
// Shader-ul de fragment / Fragment shader
#version 330 core

in vec4 ex_Color;
uniform int codCuloare;

out vec4 out_Color;

void main(void)
{
  switch (codCuloare)
  {
	case 0:
	  out_Color = ex_Color;
	  break;
	case 1:
		out_Color=vec4 (0.0, 0.0, 1.0, 0.0);
		break;
	case 2:
		out_Color=vec4 (1.0, 0.0, 0.0, 0.0);
		break;
	default:
		break;
  };
}

)SHADER-SOURCE";

void Initialize();
void Render();
void Cleanup();

GLuint LoadShaders(const char* vertexSource, const char* fragSource);

void CreateVBO(void){
    GLfloat Vertices[] = {
            // MASINA:

            // masina - triunghi stanga
            0, 0,
            0, 15,
            25, 0,
            // masina - triunghi dreapta
            25, 0,
            25, 15,
            0, 15,
            // acoperis - triunghi stanga
            2, 15,
            3.5, 18,
            23, 15,
            // acoperis - triunghi dreapta
            23, 15,
            3.5, 18,
            21.5, 18,

            // Drum stanga

            xStart, 0,
            xStart, 50,
            xEnd, 0,
            // Drum dreapta

            xStart, 50,
            xEnd, 50,
            xEnd, 0,

            // Marcaje drum

            xStart, 20,
            xStart, 24,
            xStart + 7, 20,
            xStart, 24,
            xStart + 7, 24,
            xStart + 7, 20,

            // Orizont
            xStart, 50,
            xStart, yEnd,
            xEnd, 50,
            xStart, yEnd,
            xEnd, yEnd,
            xEnd, 50,
    };

    GLfloat Colors[] = {
            // Masina
            1.0, 0.5, 0.0, 1.0,
            1.0, 0.5, 0.0, 1.0,
            1.0, 0.5, 0.0, 1.0,
            1.0, 0.5, 0.0, 1.0,
            1.0, 0.5, 0.0, 1.0,
            1.0, 0.5, 0.0, 1.0,
            // parte superioara masina
            0.6, 0.8, 0.0, 1.0,
            0.6, 0.8, 0.0, 1.0,
            0.6, 0.8, 0.0, 1.0,
            0.6, 0.8, 0.0, 1.0,
            0.6, 0.8, 0.0, 1.0,
            0.6, 0.8, 0.0, 1.0,
            // Drum
            0.3, 0.3, 0.3, 1.0,
            0.3, 0.3, 0.3, 1.0,
            0.3, 0.3, 0.3, 1.0,
            0.3, 0.3, 0.3, 1.0,
            0.3, 0.3, 0.3, 1.0,
            0.3, 0.3, 0.3, 1.0,
            // Marcaje drum
            1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0,
            // Cer
            0.5, 0.8, 0.8, 1.0,
            0.5, 0.8, 0.8, 1.0,
            0.5, 0.8, 0.8, 1.0,
            0.5, 0.8, 0.8, 1.0,
            0.5, 0.8, 0.8, 1.0,
            0.5, 0.8, 0.8, 1.0,
    };

    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);

    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void Initialize() {
    CreateVBO();
    ProgramId = LoadShaders(VertexShaderSource, FragmentShaderSource);
    codColLocation = glGetUniformLocation(ProgramId, "codCuloare");
    myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");

    ResizeMat = glm::ortho(xStart, xEnd, yStart, yEnd);

}

const float Period = 4;

bool PeriodIsInFirstQuarter(float point){
    // point exists in (0, Period)
    return point < Period / 4;
}

bool PeriodIsInSecondQuarter(float point){
    // point exists in (0, Period)
    return Period / 4 <= point && point < Period / 2;
}

bool PeriodIsInThirdQuarter(float point){
    // point exists in (0, Period)
    return Period / 2 <= point && point < Period * 3 / 4;
}

float MoveHorizontalAxis(float t) {
    double point = t - glm::floor(t / Period) * Period;
    if (PeriodIsInFirstQuarter(point))
        return 0;

    if (PeriodIsInSecondQuarter(point))
        return glm::sin(glm::pi<float>() / 2 * (point - 1));

    if (PeriodIsInThirdQuarter(point))
        return 1;

    return glm::cos(glm::pi<float>() / 2 * (point - 3));
}

float MoveVerticalAxis(float time) {
    double point = time - Period * glm::floor(time / Period); // get period point in (0, Period) ).
    if (PeriodIsInSecondQuarter(point)) {
        return glm::sin(glm::pi<float>() * (point - 1));
    }

    return 0;
}


auto id = glm::identity<glm::mat4>();
glm::vec3 VerticalVecUp = glm::vec3(xStart, 31.0, 0.0);
glm::vec3 HorizontalVecRight = glm::vec3(xEnd - 43, yStart, 0.0);

void Render() {
    const auto currentTime = glfwGetTime();
    glm::mat4 transform;

    glm::mat4 translateHorizontal1 = glm::translate(id, HorizontalVecRight * MoveHorizontalAxis(currentTime));
    glm::mat4 translateHorizontal2 = glm::translate(id, HorizontalVecRight * MoveHorizontalAxis(currentTime + Period / 2));

    glm::mat4 translateUp1 = glm::translate(id, VerticalVecUp * MoveVerticalAxis(currentTime));
    glm::mat4 translateUp2 = glm::translate(id, VerticalVecUp * MoveVerticalAxis(currentTime + Period / 2));


    // Drum
    transform = ResizeMat;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 12, 6);

    // Orizont
    transform = ResizeMat;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 24, 6);

    // Drum
    GLsizei markingsCount = 4;
    for (int i = 0; i < markingsCount; i++) {
        glm::mat4 translate = glm::translate(id, glm::vec3(100 / markingsCount * i + 5, 0.0, 0.0));

        transform = ResizeMat * translate;
        glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &transform[0][0]);

        glDrawArrays(GL_TRIANGLES, 18, 6);
    }

    // Masina 1
    transform = ResizeMat * translateUp1 * translateHorizontal1;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12);

    // Masina 2
    transform = ResizeMat * translateUp2 * translateHorizontal2;
    glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &transform[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 12);

    glFlush();
}

void Cleanup() {
    glDeleteProgram(ProgramId);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

GLuint LoadShaders(const char* vertexSource, const char* fragSource) {
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShaderId, 1, &vertexSource, nullptr);
    glCompileShader(vertexShaderId);

    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertexShaderId, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderId, 1, &fragSource, nullptr);
    glCompileShader(fragmentShaderId);

    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragmentShaderId, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);
    glLinkProgram(programId);
    glUseProgram(programId);

    return programId;
}

int main() {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(640, 480, "macOS OpenGL", nullptr, nullptr);
    if (!Window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(Window);
    glewInit();
    glfwSwapInterval(1);

    Initialize();

    while (!glfwWindowShouldClose(Window)) {
        int width, height;
        glfwGetFramebufferSize(Window, &width, &height);

        glViewport(0, 0, width, height);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        Render();

        glfwSwapBuffers(Window);
        glfwPollEvents();
    }

    Cleanup();

    glfwDestroyWindow(Window);
    glfwTerminate();
    return 0;
}