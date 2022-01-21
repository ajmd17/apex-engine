#ifndef OPENGL_H
#define OPENGL_H

#ifndef __APPLE__
#define USE_GLEW 1
#define USE_GLFW_ENGINE 1
#else
#define USE_MGL_ENGINE 1
#endif

#if USE_GLEW
#include <GL/glew.h>
#endif

#if USE_GLFW_ENGINE
#include <GLFW/glfw3.h>
#define ENGINE_BACKEND GlfwEngine
#endif

#if USE_MGL_ENGINE
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
#define ENGINE_BACKEND MglEngine
#endif

#endif
