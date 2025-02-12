#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define RGFW_IMPLEMENTATION
#define RGFW_NO_IOKIT
#include "RGFW.h"

#ifndef __APPLE__
#include <GL/gl.h>
#else
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>
#endif

#define RGL_PROC_DEF(proc, name) name##SRC = (name##PROC)proc(#name)

typedef void (*RGLapiproc)(void);
typedef RGLapiproc (*RGLloadfunc)(const char *name);

typedef void (*glGenVertexArraysPROC)(GLsizei n, GLuint *arrays);
typedef void (*glDeleteVertexArraysPROC) (GLsizei n, const GLuint *arrays);
typedef void (*glBindVertexArrayPROC)(GLuint array);

glDeleteVertexArraysPROC glDeleteVertexArraysSRC = NULL;
glGenVertexArraysPROC glGenVertexArraysSRC = NULL;
glBindVertexArrayPROC glBindVertexArraySRC = NULL;

#define glDeleteVertexArrays glDeleteVertexArraysSRC
#define glGenVertexArrays glGenVertexArraysSRC
#define glBindVertexArray glBindVertexArraySRC

int RGL_loadGL3(RGLloadfunc proc) {
    RGL_PROC_DEF(proc, glGenVertexArrays);
    RGL_PROC_DEF(proc, glBindVertexArray);
    RGL_PROC_DEF(proc, glDeleteVertexArrays);

    if (glDeleteVertexArraysSRC == NULL ||
        glGenVertexArraysSRC == NULL ||
        glBindVertexArraySRC == NULL      
    )
        return 1;

    GLuint vao;
    glGenVertexArraysSRC(1, &vao);
    
    if (vao == 0) 
        return 1;
    
    glDeleteVertexArraysSRC(1, &vao);
    return 0;
}

#define WIDTH 800
#define HEIGHT 600

int main(void) {
    char* frag_src = "#version 330\n"
"uniform float time;\n"
"in vec2 uv;\n"
"out vec4 out_color;\n"
"void main(void) {\n"
"    out_color = vec4(uv.x, uv.y, 1, 1);\n"
"}\n";

    char* vert_src = "#version 330\n"
"uniform float time;\n"
"out vec2 uv;\n"
"void main(void) {\n"
"    uv.x = (gl_VertexID & 1);\n"
"    uv.y = ((gl_VertexID >> 1) & 1);\n"
"    vec4 pos = vec4(uv.xy - 0.5, 0, 1) * vec4(0.5, 0.5, 0.5, 1.0);\n"
"    float t = time / 512.0;\n"
"    float px = pos.x * cos(t) - pos.z * sin(t);\n"
"    float pz = pos.x * sin(t) + pos.z * cos(t);\n"
"    pos.x = px;\n"
"    pos.z = pz;\n"
"    pos.z += 0.75;\n"
"    pos.x /= pos.z;\n"
"    pos.y /= pos.z;\n"
"    gl_Position = pos;\n"
"}\n";

	RGFW_setGLVersion(RGFW_glCore, 3, 3);

	RGFW_window* window = RGFW_createWindow("test", RGFW_RECT(WIDTH, HEIGHT, WIDTH, HEIGHT), RGFW_windowAllowDND | RGFW_windowCenter);
    if (window == NULL) {
        fprintf(stderr, "failed to create RGFW window\n");
        return 1;
    }
    RGFW_window_makeCurrent(window);

    if (RGL_loadGL3((RGLloadfunc)RGFW_getProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag_shader, 1, (const char**)&frag_src, NULL);
    glCompileShader(frag_shader);

    GLint frag_shader_compiled = 0;
    glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &frag_shader_compiled);

    if (!frag_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(frag_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile fragment shader: %.*s\n", message_size, message);
        return 1;
    }
    
    GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert_shader, 1, (const char**)&vert_src, NULL);
    glCompileShader(vert_shader);

    GLint vert_shader_compiled = 0;
    glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &vert_shader_compiled);

    if (!vert_shader_compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(vert_shader, sizeof(message), &message_size, message);
        fprintf(stderr, "could not compile vertex shader: %.*s\n", message_size, message);
        return 1;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, frag_shader);
    glAttachShader(program, vert_shader);
    glLinkProgram(program);

    GLint linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetProgramInfoLog(program, sizeof(message), &message_size, message);
        fprintf(stderr, "could not link program: %.*s\n", message_size, message);
        return 1;
    }

    glDeleteShader(frag_shader);
    glDeleteShader(vert_shader);

    glUseProgram(program);

    GLint time_uniform_location = glGetUniformLocation(program, "time");
    glViewport(0, 0, WIDTH, HEIGHT);

    while (RGFW_window_shouldClose(window) == 0) {
        RGFW_window_checkEvent(window);

        uint64_t time = RGFW_getTimeNS() / 1e6;
        glUniform1f(time_uniform_location, time);
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                        
        RGFW_window_swapBuffers(window);
        RGFW_window_checkFPS(window, 60);
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);

    RGFW_window_close(window);
    
    return 0;
}
