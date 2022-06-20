#include "Render_Base.h"

// OpenGL state shared among all renderers
EGLDisplay          Render_Base::sDisplay       = EGL_NO_DISPLAY;
EGLContext          Render_Base::sContext       = EGL_NO_CONTEXT;
EGLSurface          Render_Base::sSurface       = EGL_NO_SURFACE;
GLuint              Render_Base::sWidth         = 0;
GLuint              Render_Base::sHeight        = 0;
GLfloat             Render_Base::sAspectRatio   = 0.0f;
EGLNativeWindowType Render_Base::sWindow        = 0;
GLint               Render_Base::sDimension     = 0;

bool Render_Base::prepareGL()
{
    // Just trivially return success if we're already prepared
    if (sDisplay != EGL_NO_DISPLAY) {
        return true;
    }

    EGLint configAttribs[] = {
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES3_BIT,
        EGL_RED_SIZE,           8,
        EGL_GREEN_SIZE,         8,
        EGL_BLUE_SIZE,          8,
        EGL_DEPTH_SIZE,         0,
        EGL_NONE};

    // get the surface
    WindowSurface windowSurface;
    EGLNativeWindowType window = windowSurface.getSurface();

    // Set up our OpenGL ES context associated with the default display
    // (though we won't be visible)
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY)
    {
        LOG(ERROR) << "Failed to get egl display";
        return false;
    }
    else
    {
        LOG(INFO) << "EGL display is avaialble";
    }

    EGLint majorVersion = 0;
    EGLint minorVersion = 0;
    // To initialize the internal data structure and return major and minor version of EGL implementation
    if (!eglInitialize(display, &majorVersion, &minorVersion))
    {
        LOG(ERROR) << "Failed to initialize EGL: " << getEGLError();
        return false;
    }
    else
    {
        LOG(INFO) << "Intiialized EGL at " << majorVersion << "." << minorVersion;
    }
    
    //window format
    GLint format;
    int err;
    if ((err = window->query(window, NATIVE_WINDOW_FORMAT, &format)) < 0)
    {
        LOG(ERROR) << "Failed to query supported window format error: " << err;
        return false;
    }

    // Select the configuration that "best" matches our desired characteristics
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs))
    {
        LOG(ERROR) << "eglChooseConfig() failed with error: " << getEGLError();
        return false;
    }
    if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
    {
        LOG(ERROR) << "eglGetConfigAttrib() returned error" << eglGetError();
        return false;
    }

    EGLSurface surface = eglCreateWindowSurface(display, config, window, NULL);
    if (window == EGL_NO_SURFACE)
    {
        switch (eglGetError())
        {
        case EGL_BAD_MATCH:
            // Check window and EGLConfig attributes to determine
            // compatibility, or verify that the EGLConfig
            // supports rendering to a window
            LOG(ERROR) << "EGL_BAD_MATCH";
            break;
        case EGL_BAD_CONFIG:
            // Verify that provided EGLConfig is valid
            LOG(ERROR) << "EGL_BAD_CONFIG";
            break;
        case EGL_BAD_NATIVE_WINDOW:
            // Verify that provided EGLNativeWindow is valid
            LOG(ERROR) << "EGL_BAD_NATIVE_WINDOW";
            break;
        case EGL_BAD_ALLOC:
            // Not enough resources available; handle and recover
            LOG(ERROR) << "EGL_BAD_ALLOC";
            break;
        }
        return false;
    }

    const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attribs);
    if (context == EGL_NO_CONTEXT)
    {
        if (eglGetError() == EGL_BAD_CONFIG)
        {
            LOG(ERROR) << "Failed to create OpenGL ES Context: " << getEGLError();
            // Handle error and recover
        }
        return false;
    }
    if (!eglMakeCurrent(display, surface, surface, context))
    {
        if (eglGetError() == EGL_BAD_CONFIG)
        {
            LOG(ERROR) << "Failed to Make current Context: " << getEGLError();
            // Handle error and recover
        }
        return false;
    }

    // Report the extensions available on this implementation
    const char* gl_extensions = (const char*) glGetString(GL_EXTENSIONS);
    LOG(INFO) << "GL EXTENSIONS:\n  " << gl_extensions;
    
    int w,h;
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    // Now that we're assured success, store object handles we constructed
    sDisplay = display;
    sContext = context;
    sSurface = surface;
    sWindow = window;
    sWidth = w;
    sHeight = h;
    sDimension = w < h ? w : h;

    LOG(INFO) << " OPENGL ES Successfully Intialised With Render Surface w: " << w << ", h: " << h;
    return true;
}
