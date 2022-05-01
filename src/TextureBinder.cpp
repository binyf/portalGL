#include "TextureBinder.h"

BindedTextureInfo* TextureBinder::bind(const char* image_path)
{
    GLuint texture = cg_create_texture(image_path, true);

    return new BindedTextureInfo(texture);
}
