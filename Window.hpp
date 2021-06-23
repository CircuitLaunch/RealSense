#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <string>
#include <map>
#include "Texture.hpp"

using namespace std;

namespace edj
{

  class Window
  {
    public:
      Window(int iWidth, int iHeight, const string &iTitle);
      virtual ~Window();

      void close();

      float width() const { return mWidth; }
      float height() const { return mHeight; }
      operator GLFWwindow *() { return mWin; }

      void start();

      virtual void setup();
      virtual void loop();

      virtual void onLeftMouse(bool);
      virtual void onMouseScroll(double, double);
      virtual void onMouseMove(double, double);
      virtual void onKeyPress(int);
      virtual void onKeyRepeat(int);
      virtual void onKeyRelease(int);

    protected:
      GLFWwindow *mWin;
      int mWidth, mHeight;
  };

}

#endif
