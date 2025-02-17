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

#ifndef GL_COMPILE_STATUS
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_VERTEX_SHADER                  0x8B31
#define GL_COMPILE_STATUS                 0x8B81
#define GL_LINK_STATUS                    0x8B82

typedef char GLchar;
#endif

#define RGL_PROC_DEF(proc, name) name##SRC = (name##PROC)proc(#name)

typedef void (*RGLapiproc)(void);
typedef RGLapiproc (*RGLloadfunc)(const char *name);

typedef void (*glGenVertexArraysPROC)(GLsizei n, GLuint *arrays);
typedef void (*glDeleteVertexArraysPROC) (GLsizei n, const GLuint *arrays);
typedef void (*glBindVertexArrayPROC)(GLuint array);
typedef void (*glShaderSourcePROC) (GLuint shader, GLsizei count, const GLchar *const*string, const GLint *length);
typedef GLuint (*glCreateShaderPROC) (GLenum type);
typedef void (*glCompileShaderPROC) (GLuint shader);
typedef GLuint (*glCreateProgramPROC) (void);
typedef void (*glAttachShaderPROC) (GLuint program, GLuint shader);
typedef void (*glLinkProgramPROC) (GLuint program);
typedef void (*glUseProgramPROC) (GLuint program);
typedef void (*glDeleteShaderPROC) (GLuint shader);
typedef void (*glDeleteProgramPROC) (GLuint program);
typedef void (*glGetShaderivPROC)(GLuint shader, GLenum pname, GLint *params);
typedef void (*glGetShaderInfoLogPROC)(GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (*glGetProgramivPROC)(GLuint program, GLenum pname, GLint *params);
typedef void (*glGetProgramInfoLogPROC)(GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
typedef void (*glGenVertexArraysPROC)(GLsizei n, GLuint *arrays);
typedef GLint (*glGetUniformLocationPROC)(GLuint program, const GLchar *name);
typedef void (*glUniform1fPROC)(GLint location, GLfloat v0);

glDeleteVertexArraysPROC glDeleteVertexArraysSRC = NULL;
glGenVertexArraysPROC glGenVertexArraysSRC = NULL;
glBindVertexArrayPROC glBindVertexArraySRC = NULL;
glShaderSourcePROC glShaderSourceSRC = NULL;
glCreateShaderPROC glCreateShaderSRC = NULL;
glCompileShaderPROC glCompileShaderSRC = NULL;
glCreateProgramPROC glCreateProgramSRC = NULL;
glAttachShaderPROC glAttachShaderSRC = NULL;
glLinkProgramPROC glLinkProgramSRC = NULL;
glUseProgramPROC glUseProgramSRC = NULL;
glDeleteShaderPROC glDeleteShaderSRC = NULL;
glDeleteProgramPROC glDeleteProgramSRC = NULL;
glGetShaderivPROC glGetShaderivSRC = NULL;
glGetShaderInfoLogPROC glGetShaderInfoLogSRC = NULL;
glGetProgramivPROC glGetProgramivSRC = NULL;
glGetProgramInfoLogPROC glGetProgramInfoLogSRC = NULL;
glGetUniformLocationPROC glGetUniformLocationSRC = NULL;
glUniform1fPROC glUniform1fSRC = NULL;

#define glDeleteVertexArrays glDeleteVertexArraysSRC
#define glGenVertexArrays glGenVertexArraysSRC
#define glBindVertexArray glBindVertexArraySRC
#define glShaderSource glShaderSourceSRC
#define glCreateShader glCreateShaderSRC
#define glCompileShader glCompileShaderSRC
#define glCreateProgram glCreateProgramSRC
#define glAttachShader glAttachShaderSRC
#define glLinkProgram glLinkProgramSRC
#define glUseProgram glUseProgramSRC
#define glDeleteShader glDeleteShaderSRC
#define glDeleteProgram glDeleteProgramSRC
#define glGetShaderiv glGetShaderivSRC
#define glGetShaderInfoLog glGetShaderInfoLogSRC
#define glGetProgramiv glGetProgramivSRC
#define glGetProgramInfoLog glGetProgramInfoLogSRC
#define glGetUniformLocation glGetUniformLocationSRC
#define glUniform1f glUniform1fSRC

int load_opengl_extensions() {
    RGLloadfunc proc = (RGLloadfunc)RGFW_getProcAddress;

    RGL_PROC_DEF(proc, glGenVertexArrays);
    RGL_PROC_DEF(proc, glBindVertexArray);
    RGL_PROC_DEF(proc, glDeleteVertexArrays);
    RGL_PROC_DEF(proc, glCreateShader);
    RGL_PROC_DEF(proc, glShaderSource);
    RGL_PROC_DEF(proc, glCompileShader);
    RGL_PROC_DEF(proc, glGetShaderiv);
    RGL_PROC_DEF(proc, glGetShaderInfoLog);
    RGL_PROC_DEF(proc, glCreateProgram);
    RGL_PROC_DEF(proc, glAttachShader);
    RGL_PROC_DEF(proc, glLinkProgram);
    RGL_PROC_DEF(proc, glGetProgramiv);
    RGL_PROC_DEF(proc, glGetProgramInfoLog);
    RGL_PROC_DEF(proc, glDeleteShader);
    RGL_PROC_DEF(proc, glUseProgram);
    RGL_PROC_DEF(proc, glGetUniformLocation);
    RGL_PROC_DEF(proc, glDeleteProgram);
    RGL_PROC_DEF(proc, glUniform1f);

    if (glDeleteVertexArraysSRC == NULL ||
        glGenVertexArraysSRC == NULL    ||
        glBindVertexArraySRC == NULL    ||
        glCreateShaderSRC == NULL       ||
        glShaderSourceSRC == NULL       ||
        glCompileShaderSRC == NULL      ||
        glGetShaderivSRC == NULL        ||
        glGetShaderInfoLogSRC == NULL   ||
        glCreateProgramSRC == NULL      ||
        glAttachShaderSRC == NULL       ||
        glLinkProgramSRC == NULL        ||
        glGetProgramivSRC == NULL       ||
        glGetProgramInfoLogSRC == NULL  ||
        glDeleteShaderSRC == NULL       ||
        glUseProgramSRC == NULL         ||
        glGetUniformLocationSRC == NULL ||
        glDeleteProgramSRC == NULL
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

	RGFW_window* window = RGFW_createWindow("tinygl", RGFW_RECT(WIDTH, HEIGHT, WIDTH, HEIGHT), RGFW_windowCenter);
    if (window == NULL) {
        fprintf(stderr, "failed to create RGFW window :(\n");
        return 1;
    }
    RGFW_window_makeCurrent(window);

    if (load_opengl_extensions() != 0) {
        fprintf(stderr, "failed to initialize extensions :(\n");
        return 1;
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
