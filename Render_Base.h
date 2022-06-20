#ifndef RENDER_BASE_H
#define RENDER_BASE_H

// Android
#include <android-base/logging.h>
#include <assert.h>

// EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>

// GLES
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>

// Math
#include <math/mat4.h>

// window
#include <WindowSurface.h>
#include <EGLUtils.h>

// error
#include <utils/Errors.h>

// internal
#include "glError.h"

using namespace android;

class Render_Base
{
public:
    virtual ~Render_Base() {
        delete windowSurface;
    };

    virtual bool activate() = 0;
    virtual void deactivate() = 0;

    virtual bool drawFrame() = 0;

protected:
    static bool prepareGL();
    // OpenGL state shared among all renderers
    static EGLDisplay   sDisplay;
    static EGLContext   sContext;
    static EGLSurface   sSurface;

    static GLuint     sWidth;
    static GLuint     sHeight;
    static GLfloat    sAspectRatio;

    // Window
    static EGLNativeWindowType sWindow;
        
    static GLint sDimension;
};

#endif // RENDER_BASE_H
