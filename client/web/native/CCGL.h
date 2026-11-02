#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#ifndef GL_BGRA
#define GL_BGRA GL_BGRA_EXT
#endif
#ifndef GL_DEPTH24_STENCIL8
#define GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#endif
#define CC_GL_DEPTH24_STENCIL8 GL_DEPTH24_STENCIL8_OES
#define glClearDepth glClearDepthf
#define glBindVertexArray glBindVertexArrayOES
#define glDeleteVertexArrays glDeleteVertexArraysOES
#define glGenVertexArrays glGenVertexArraysOES
