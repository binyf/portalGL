#include "BindedTextureInfo.h"

BindedTextureInfo::BindedTextureInfo(GLuint texture): texture(texture)
{
}

GLuint BindedTextureInfo::get_texture()
{
    return this->texture;
}
