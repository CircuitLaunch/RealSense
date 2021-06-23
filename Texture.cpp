#include "Texture.hpp"

namespace edj
{
  Texture::Texture() {
    glGenTextures(1, &mHandle);
  }

  Texture::~Texture() {
    if(mHandle != 0) {
      glDeleteTextures(1, &mHandle);
    }
  }

  void Texture::upload(GLint iInternalFormat, GLenum iFormat, GLenum iComponentType, int iWidth, int iHeight, const void *iData) {
    glBindTexture(GL_TEXTURE_2D, mHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, iFormat, iWidth, iHeight, 0, iInternalFormat, iComponentType, iData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Texture::bind()
  {
    glBindTexture(GL_TEXTURE_2D, mHandle);
  }

  void Texture::unbind()
  {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
