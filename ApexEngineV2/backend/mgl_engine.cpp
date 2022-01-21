#include "mgl_engine.h"

#if USE_MGL_ENGINE

#define GL_GLEXT_PROTOTYPES
#include <GL/glcorearb.h>

#include "../game.h"
#include "../input_manager.h"
#include "../util.h"
#include "../math/math_util.h"

#include <iostream>
#include <chrono>

extern "C" {
#include "MGLContext.h"
}
#include "MGLRenderer.h"
#define SWAP_BUFFERS MGLswapBuffers((GLMContext) glfwGetWindowUserPointer(window));

#define GLSL(version, shader) "#version " #version "\n" #shader

GLuint compileGLSLProgram(GLenum shader_count, ...)
{
    va_list argp;
    va_start(argp, shader_count);
    GLuint type;
    const char *src;
    GLuint shader;

    GLuint shader_program = glCreateProgram();

    for(int i=0; i<shader_count; i++)
    {
        type = va_arg(argp, GLuint);
        src = va_arg(argp, const char *);
        assert(src);

        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);
        glAttachShader(shader_program, shader);
    }
    
    glLinkProgram(shader_program);

    va_end(argp);

    return shader_program;
}


#define USE_CHRONO 1

namespace apex {

static apex::InputManager *inputmgr;

static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (action == GLFW_PRESS) {
        inputmgr->KeyDown(key);
    } else if (action == GLFW_RELEASE) {
        inputmgr->KeyUp(key);
    }
}

static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS) {
        inputmgr->MouseButtonDown(button);
    } else if (action == GLFW_RELEASE) {
        inputmgr->MouseButtonUp(button);
    }
}

static void ErrorCallback(int error, const char *description)
{
    std::cout << "Error: " << description << "\n";
}

static unsigned int bindVAO(unsigned int vao=0)
{
    if(vao)
    {
        glBindVertexArray(vao);
    }
    else
    {
        glCreateVertexArrays(1, &vao);
        glBindVertexArray(vao);
    }

    return vao;
}

bool MglEngine::InitializeGame(Game *game)
{
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit()) {
        return false;
    }

    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

    // glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);
    
    // glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    // glfwWindowHint(GLFW_DEPTH_BITS, 32);

    window = glfwCreateWindow(
        game->GetRenderer()->GetRenderWindow().width,
        game->GetRenderer()->GetRenderWindow().height,
        game->GetRenderer()->GetRenderWindow().title.c_str(),
        nullptr,
        nullptr
    );

    if (!window) {
        glfwTerminate();
        return false;
    }

    GLMContext glm_ctx = createGLMContext(GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, GL_DEPTH_COMPONENT, GL_FLOAT, 0, 0);
    void *renderer = CppCreateMGLRendererAndBindToContext (glfwGetCocoaWindow (window), glm_ctx); // FIXME should do something later with the renderer
    if (!renderer)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    MGLsetCurrentContext(glm_ctx);
    glfwSetWindowUserPointer(window, glm_ctx);

    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    // glfwMakeContextCurrent(window);

    Vector2 render_scale(Vector2::One()), prev_render_scale = game->GetRenderer()->GetRenderWindow().GetScale();

    //render_scale = Vector2::Max(render_scale, Vector2::One());

#ifdef USE_GLEW
    if (glewInit() != GLEW_OK) {
        throw "error initializing glew";
    }
#endif

    glfwSwapInterval(0);
    glfwGetError(nullptr);

    glClearDepth(1.0);
    glDepthMask(true);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glClearColor(1.0f, 0.5f, 0.5f, 1.0f);

    if (!glfwWindowShouldClose(window)) {
        glfwGetWindowContentScale(
            window,
            &render_scale.GetX(),
            &render_scale.GetY()
        );

        glfwGetWindowSize(
            window,
            &game->GetRenderer()->GetRenderWindow().width,
            &game->GetRenderer()->GetRenderWindow().height
        );
        
        game->GetRenderer()->GetRenderWindow().SetScale(Vector2::Max(render_scale, Vector2::One()));

        prev_render_scale = render_scale;
    }


    /** DUMMY SHADERS REQUIRED  */

    const char* vertex_shader =
    GLSL(450 core,
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec2 in_texcords;

        layout(location = 0) out vec2 out_texcoords;

        layout(binding = 0) uniform matrices
        {
            mat4 mvp;
        };

        void main() {
            gl_Position = mvp * vec4(position, 1.0);
            out_texcoords = in_texcords;
        }
    );

    const char* fragment_shader =
    GLSL(450 core,
        layout(location = 0) in vec2 in_texcords;

        layout(location = 0) out vec4 frag_colour;

        uniform sampler2D image;

        void main() {
            vec4 tex_color = texture(image, in_texcords);

            frag_colour = tex_color;
        }
    );

    GLuint shader_program;
    shader_program = compileGLSLProgram(2, GL_VERTEX_SHADER, vertex_shader, GL_FRAGMENT_SHADER, fragment_shader);
    glUseProgram(shader_program);

    unsigned int vao = bindVAO();

    game->Initialize();

    inputmgr = game->GetInputManager();

#if USE_CHRONO
    auto last = std::chrono::steady_clock::now();
#else
    double last = 0.0;
#endif
    while (!glfwWindowShouldClose(window)) {
#if USE_CHRONO
        auto current = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(current - last).count();
#else
        double current = glfwGetTime();
        double delta = current_time - last;
#endif

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwGetWindowContentScale(
            window,
            &render_scale.GetX(),
            &render_scale.GetY()
        );

        glfwGetWindowSize(
            window,
            &game->GetRenderer()->GetRenderWindow().width,
            &game->GetRenderer()->GetRenderWindow().height
        );

        if (prev_render_scale != render_scale) {
            game->GetRenderer()->GetRenderWindow().SetScale(Vector2::Max(render_scale, Vector2::One()));

            prev_render_scale = render_scale;
        }

        double mouse_x, mouse_y;

        glfwGetCursorPos(window, &mouse_x, &mouse_y);

        game->GetInputManager()->MouseMove(
            MathUtil::Clamp<double>(mouse_x, 0, game->GetRenderer()->GetRenderWindow().width),
            MathUtil::Clamp<double>(mouse_y, 0, game->GetRenderer()->GetRenderWindow().height)
        );

        game->Update(delta);
        game->Render();

        SWAP_BUFFERS;
        // glfwSwapBuffers(window);
        glfwPollEvents();

        last = current;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}

void MglEngine::SetCursorLocked(bool locked)
{
    if (locked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void MglEngine::Viewport(int x, int y, size_t width, size_t height)
{
    glViewport(x, y, width, height);
}

void MglEngine::Clear(int mask)
{
    glClear(mask);
}

void MglEngine::ClearColor(float red, float green, float blue, float alpha)
{
    glClearColor(red, green, blue, alpha);
}

void MglEngine::SetMousePosition(double x, double y)
{
    int window_x, window_y;
    glfwGetWindowPos(window, &window_x, &window_y);
    glfwSetCursorPos(window, x, y);
}

void MglEngine::Enable(int cap)
{
    glEnable(cap);
}

void MglEngine::Disable(int cap)
{
    glDisable(cap);
}

void MglEngine::DepthMask(bool mask)
{
    glDepthMask(mask);
}

void MglEngine::BlendFunc(int src, int dst)
{
    glBlendFunc(src, dst);
}

void MglEngine::GenBuffers(size_t count, unsigned int *buffers)
{
    glGenBuffers(count, buffers);
}

void MglEngine::DeleteBuffers(size_t count, unsigned int *buffers)
{
    glDeleteBuffers(count, buffers);
}

void MglEngine::BindBuffer(int target, unsigned int buffer)
{
    glBindBuffer(target, buffer);
}

void MglEngine::BufferData(int target, size_t size, const void *data, int usage)
{
    glBufferData(target, size, data, usage);
}

void MglEngine::BufferSubData(int target, size_t offset, size_t size, const void *data)
{
    glBufferSubData(target, offset, size, data);
}

void MglEngine::BindVertexArray(unsigned int target)
{
    glBindVertexArray(target);
}

void MglEngine::GenVertexArrays(size_t size, unsigned int *arrays)
{
    glGenVertexArrays(size, arrays);
}

void MglEngine::DeleteVertexArrays(size_t size, unsigned int *arrays)
{
    glDeleteVertexArrays(size, arrays);
}

void MglEngine::EnableVertexAttribArray(unsigned int index)
{
    glEnableVertexAttribArray(index);
}

void MglEngine::DisableVertexAttribArray(unsigned int index)
{
    glDisableVertexAttribArray(index);
}

void MglEngine::VertexAttribPointer(unsigned int index, int size, int type, bool normalized, size_t stride, void *ptr)
{
    glVertexAttribPointer(index, size, type, normalized, stride, ptr);
}

void MglEngine::DrawElements(int mode, size_t count, int type, const void *indices)
{
    glDrawElements(mode, count, type, indices);
}

void MglEngine::GenTextures(size_t n, unsigned int *textures)
{
    glGenTextures(n, textures);
}

void MglEngine::DeleteTextures(size_t n, const unsigned int *textures)
{
    glDeleteTextures(n, textures);
}

void MglEngine::TexParameteri(int target, int pname, int param)
{
    glTexParameteri(target, pname, param);
}

void MglEngine::TexParameterf(int target, int pname, float param)
{
    glTexParameterf(target, pname, param);
}

void MglEngine::TexImage2D(int target, int level, int ifmt, size_t width, size_t height,
    int border, int fmt, int type, const void *data)
{
    glTexImage2D(target, level, ifmt, width, height, border, fmt, type, data);
}

void MglEngine::BindTexture(int target, unsigned int texture)
{
    glBindTexture(target, texture);
}

void MglEngine::ActiveTexture(int i)
{
    glActiveTexture(i);
}

void MglEngine::GenerateMipmap(int target)
{
    glGenerateMipmap(target);
}

void MglEngine::GenFramebuffers(size_t n, unsigned int *ids)
{
    glGenFramebuffers(n, ids);
}

void MglEngine::DeleteFramebuffers(size_t n, const unsigned int *ids)
{
    glDeleteFramebuffers(n, ids);
}

void MglEngine::BindFramebuffer(int target, unsigned int framebuffer)
{
    glBindFramebuffer(target, framebuffer);
}

void MglEngine::FramebufferTexture(int target, int attachment, unsigned int texture, int level)
{
    glFramebufferTexture(target, attachment, texture, level);
}

void MglEngine::FramebufferTexture(int target, int attachment, unsigned int texture_target, unsigned int texture, int level)
{
    glFramebufferTexture2D(target, attachment, texture_target, texture, level);
}

void MglEngine::DrawBuffers(size_t n, const unsigned int *bufs)
{
    glDrawBuffers(n, bufs);
}

unsigned int MglEngine::CheckFramebufferStatus(int target)
{
    return glCheckFramebufferStatus(target);
}

unsigned int MglEngine::CreateProgram()
{
    return glCreateProgram();
}

unsigned int MglEngine::CreateShader(int type)
{
    return glCreateShader(type);
}

void MglEngine::ShaderSource(unsigned int shader, size_t count, const char **str, const int *len)
{
    glShaderSource(shader, count, str, len);
}

void MglEngine::CompileShader(unsigned int shader)
{
    glCompileShader(shader);
}

void MglEngine::AttachShader(unsigned int program, unsigned int shader)
{
    glAttachShader(program, shader);
}

void MglEngine::GetShaderiv(unsigned int shader, int pname, int *params)
{
    glGetShaderiv(shader, pname, params);
}

void MglEngine::GetShaderInfoLog(unsigned int shader, int max, int *len, char *info)
{
    glGetShaderInfoLog(shader, max, len, info);
}

void MglEngine::BindAttribLocation(unsigned int program, unsigned int index, const char *name)
{
    glBindAttribLocation(program, index, name);
}

void MglEngine::BindFragDataLocation(unsigned int program, unsigned int color_number, const char *name)
{
    glBindFragDataLocation(program, color_number, name);
}

void MglEngine::LinkProgram(unsigned int program)
{
    glLinkProgram(program);
}

void MglEngine::ValidateProgram(unsigned int program)
{
    glValidateProgram(program);
}

void MglEngine::GetProgramiv(unsigned int program, int pname, int *params)
{
    glGetProgramiv(program, pname, params);
}

void MglEngine::GetProgramInfoLog(unsigned int program, int max, int *len, char *log)
{
    glGetProgramInfoLog(program, max, len, log);
}

void MglEngine::DeleteProgram(unsigned int program)
{
    glDeleteProgram(program);
}

void MglEngine::DeleteShader(unsigned int shader)
{
    glDeleteShader(shader);
}

void MglEngine::UseProgram(unsigned int program)
{
    glUseProgram(program);
}

int MglEngine::GetUniformLocation(unsigned int program, const char *name)
{
    return glGetUniformLocation(program, name);
}

void MglEngine::Uniform1f(int location, float v0)
{
    glUniform1f(location, v0);
}

void MglEngine::Uniform2f(int location, float v0, float v1)
{
    glUniform2f(location, v0, v1);
}

void MglEngine::Uniform3f(int location, float v0, float v1, float v2)
{
    glUniform3f(location, v0, v1, v2);
}

void MglEngine::Uniform4f(int location, float v0, float v1, float v2, float v3)
{
    glUniform4f(location, v0, v1, v2, v3);
}

void MglEngine::Uniform1i(int location, int v0)
{
    glUniform1i(location, v0);
}

void MglEngine::Uniform2i(int location, int v0, int v1)
{
    glUniform2i(location, v0, v1);
}

void MglEngine::Uniform3i(int location, int v0, int v1, int v2)
{
    glUniform3i(location, v0, v1, v2);
}

void MglEngine::Uniform4i(int location, int v0, int v1, int v2, int v3)
{
    glUniform4i(location, v0, v1, v2, v3);
}

void MglEngine::UniformMatrix4fv(int location, int count, bool transpose, const float *value)
{
    glUniformMatrix4fv(location, count, transpose, value);
}

void MglEngine::VertexAttribDivisor(unsigned int index, unsigned int divisor)
{
    assert(0);
    //glVertexAttribDivisor(index, divisor);
}

void MglEngine::DrawArraysInstanced(int mode, int first, size_t count, size_t primcount)
{
    assert(0);
    //glDrawArraysInstanced(mode, first, count, primcount);
}

void MglEngine::BindImageTexture(unsigned int unit, unsigned int texture, int level, bool layered, int layer, unsigned int access, unsigned int format)
{
    assert(0);
    //glBindImageTexture(unit, texture, level, layered, layer, access, format);
}

void MglEngine::CopyTexSubImage2D(unsigned int target, int level, int xoffset, int yoffset, int x, int y, int width, int height)
{
    glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

void MglEngine::CullFace(int mode)
{
    glCullFace(mode);
}

} // namespace apex

#endif
