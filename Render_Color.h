#ifndef RENDER_COLOR_H
#define RENDER_COLOR_H

#include "Render_Base.h"
#include "TexWrapper.h"
#include "shader.h"
#include "shader_simpleTex.h"

class Render_Color : public Render_Base {
public:
    virtual bool activate() override;
    virtual void deactivate() override;

    virtual bool drawFrame() override;
protected:

    // Texture object handle
    TexWrapper *mTexWrapper = nullptr;

    //Shader
    GLuint mShaderProgram = 0;

    //Area Crop for color
    GLint mCrop[4]       = {0, 4, 4, -4};
};
#endif // RENDER_COLOR_H
