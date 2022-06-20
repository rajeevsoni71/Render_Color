#ifndef TEXWRAPPER_H
#define TEXWRAPPER_H

#include <GLES2/gl2.h>


class TexWrapper {
public:
    TexWrapper(GLuint textureId, unsigned width, unsigned height, GLuint strideData);
    virtual ~TexWrapper();

    GLuint glId()       { return id; };
    unsigned width()    { return w; };
    unsigned height()   { return h; };
    GLuint strideData() {return sData;};

protected:
    TexWrapper();

    GLuint id;
    unsigned w;
    unsigned h;
    GLuint sData;
};


TexWrapper* createTextureFromPng(const char* filename);

#endif // TEXWRAPPER_H