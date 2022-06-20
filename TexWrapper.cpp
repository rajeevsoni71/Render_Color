#include "TexWrapper.h"
#include "glError.h"

#include <fcntl.h>
#include <malloc.h>
#include <png.h>

#include <android-base/logging.h>


/* Create an new empty GL texture that will be filled later */
TexWrapper::TexWrapper() {
    GLuint textureId;
    glGenTextures(1, &textureId);
    if (textureId <= 0) {
        LOG(ERROR) << "Didn't get a texture handle allocated: " << getEGLError();
    } else {
        // Store the basic texture properties
        id = textureId;
        w  = 0;
        h  = 0;
    }
}


/* Wrap a texture that already allocated.  The wrapper takes ownership. */
TexWrapper::TexWrapper(GLuint textureId, unsigned width, unsigned height, GLuint strideData) {
    // Store the basic texture properties
    id = textureId;
    w  = width;
    h  = height;
    sData = strideData;
}


TexWrapper::~TexWrapper() {
    // Give the texture ID back
    if (id > 0) {
        glDeleteTextures(1, &id);
    }
    id = -1;
}


/* Factory to build TexWrapper objects from a given PNG file */
TexWrapper* createTextureFromPng(const char * filename)
{
    // Open the PNG file
    FILE *inputFile = fopen(filename, "rb");
    if (inputFile == 0)
    {
        perror(filename);
        return nullptr;
    }

    // Read the file header and validate that it is a PNG
    static const int kSigSize = 8;
    png_byte header[kSigSize] = {0};
    fread(header, 1, kSigSize, inputFile);
    if (png_sig_cmp(header, 0, kSigSize)) {
        LOG(ERROR) << filename << " is not a PNG.\n";
        fclose(inputFile);
        return nullptr;
    }

    // Set up our control structure
    png_structp pngControl = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngControl)
    {
        LOG(ERROR) <<"png_create_read_struct failed.\n";
        fclose(inputFile);
        return nullptr;
    }

    // Set up our image info structure
    png_infop pngInfo = png_create_info_struct(pngControl);
    if (!pngInfo)
    {
        LOG(ERROR) <<"error: png_create_info_struct returned 0.\n";
        png_destroy_read_struct(&pngControl, nullptr, nullptr);
        fclose(inputFile);
        return nullptr;
    }

    // Install an error handler
    if (setjmp(png_jmpbuf(pngControl))) {
        LOG(ERROR) <<"libpng reported an error\n";
        png_destroy_read_struct(&pngControl, &pngInfo, nullptr);
        fclose(inputFile);
        return nullptr;
    }

    // Set up the png reader and fetch the remaining bits of the header
    png_init_io(pngControl, inputFile);
    png_set_sig_bytes(pngControl, kSigSize);
    png_read_info(pngControl, pngInfo);

    // Get basic information about the PNG we're reading
    int bitDepth;
    int colorFormat;
    png_uint_32 width;
    png_uint_32 height;
    png_get_IHDR(pngControl, pngInfo,
                 &width, &height,
                 &bitDepth, &colorFormat,
                 NULL, NULL, NULL);
    LOG(INFO) << "Image Width: " <<  width << ", Height: " <<height;
    GLint format;
    switch(colorFormat)
    {
        case PNG_COLOR_TYPE_RGB:
            format = GL_RGB;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            format = GL_RGBA;
            break;
        default:
            LOG(ERROR) << filename << " Unknown libpng color format" << colorFormat;
            return nullptr;
    }

    // Refresh the values in the png info struct in case any transformation shave been applied.
    png_read_update_info(pngControl, pngInfo);
    int stride = png_get_rowbytes(pngControl, pngInfo);
    stride += 3 - ((stride-1) % 4);   // glTexImage2d requires rows to be 4-byte aligned

    // Allocate storage for the pixel data
    png_byte * buffer = (png_byte*)malloc(stride * height);
    if (buffer == NULL)
    {
        LOG(ERROR) <<"error: could not allocate memory for PNG image data\n";
        png_destroy_read_struct(&pngControl, &pngInfo, nullptr);
        fclose(inputFile);
        return nullptr;
    }

    // libpng needs an array of pointers into the image data for each row
    png_byte ** rowPointers = (png_byte**)malloc(height * sizeof(png_byte*));
    if (rowPointers == NULL)
    {
        LOG(ERROR) <<"Failed to allocate temporary row pointers\n";
        png_destroy_read_struct(&pngControl, &pngInfo, nullptr);
        free(buffer);
        fclose(inputFile);
        return nullptr;
    }
    for (unsigned int r = 0; r < height; r++)
    {
        rowPointers[r] = buffer + r*stride;
    }


    // Read in the actual image bytes
    png_read_image(pngControl, rowPointers);
    png_read_end(pngControl, nullptr);


    // Set up the OpenGL texture to contain this image
    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Initialize the sampling properties (it seems the sample may not work if this isn't done)
    // The user of this texture may very well want to set their own filtering, but we're going
    // to pay the (minor) price of setting this up for them to avoid the dreaded "black image" if
    // they forget.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Send the image data to GL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

    // clean up
    png_destroy_read_struct(&pngControl, &pngInfo, nullptr);
    free(buffer);
    free(rowPointers);
    fclose(inputFile);

    glBindTexture(GL_TEXTURE_2D, textureId);


    // Return the texture
    return new TexWrapper(textureId, width, height, (GLuint)stride);
}
