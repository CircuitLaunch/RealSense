#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

namespace edj
{

  class Texture
  {
    public:
      Texture();
      virtual ~Texture();

      void upload(GLint iInternalFormat, GLenum iFormat, GLenum iComponentType, int iWidth, int iHeight, const void *iData);
      void bind();
      void unbind();
      
    protected:
      GLuint mHandle;
  };

}

#endif
