#include "Render_Color.h"
#define FILE_PATH "/system/etc/Car_Image/Car_Image.png"

bool Render_Color::activate(){
    // Ensure GL is ready to go...
    if (!prepareGL()) {
        LOG(ERROR) << "Error initializing GL";
        return false;
    }

    // Load our shader program if we don't have it already
    // if (!mShaderProgram) {
    //     mShaderProgram = buildShaderProgram(vShaderStr,
    //                                         fShaderStr,
    //                                         "simpleTexture");
    //     if (!mShaderProgram) {
    //         LOG(ERROR) << "Error building shader program";
    //         return false;
    //     }
    // }

    // load three texture buffers but use them on six OGL|ES texture surfaces
    // if (mTexWrapper == nullptr)
    // {
    //     mTexWrapper = createTextureFromPng(FILE_PATH);
    //     if (mTexWrapper == nullptr)
    //     {
    //         LOG(ERROR) << "Failed to load the Image as an Texture.";
    //         return false;
    //     }
    // }
    return true;
}

void Render_Color::deactivate(){
    mTexWrapper = nullptr;
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
            = glGetError()) {
        fprintf(stderr, "after %s() glError (0x%x)\n", op, error);
    }
}

bool Render_Color::drawFrame(){
    //render a rectangle with color
    glViewport(0,0,sWidth,sHeight);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_CROP_RECT_OES, mCrop);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterx(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_TEXTURE_2D);
    glColor4f(1, 1, 1, 1);
    
    uint32_t t32[]  = { 
            0xFF000000, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000, 
            0xFF00FF00, 0xFFFF0000, 0xFF000000, 0xFF0000FF, 
            0xFF00FFFF, 0xFF00FF00, 0x00FF00FF, 0xFFFFFF00, 
            0xFF000000, 0xFFFF00FF, 0xFF00FFFF, 0xFFFFFFFF
    };
    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, t32);
    glDrawTexiOES(0, 0, 0, sDimension, sDimension);
    eglSwapBuffers(sDisplay, sSurface);

    sleep(2);
    return true;
}
