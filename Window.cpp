#include "Window.hpp"

namespace edj
{
  Window::Window(int iWidth, int iHeight, const string &iTitle)
  : mWidth(iWidth), mHeight(iHeight)
  {
    glfwInit();
    mWin = glfwCreateWindow(mWidth, mHeight, iTitle.c_str(), nullptr, nullptr);
    if(!mWin) throw runtime_error("Unable to open OpenGL window.");
    glfwMakeContextCurrent(mWin);
    glfwSetWindowUserPointer(mWin, this);
    glfwSetMouseButtonCallback(mWin, [](GLFWwindow *w, int b, int a, int m) {
        auto self = (Window *) glfwGetWindowUserPointer(w);
        switch(b) {
          case 0: self->onLeftMouse(a == GLFW_PRESS); break;
          default: break;
        }
      });
    glfwSetScrollCallback(mWin, [](GLFWwindow *w, double x, double y) {
        auto self = (Window *) glfwGetWindowUserPointer(w);
        self->onMouseScroll(x, y);
      });
    glfwSetCursorPosCallback(mWin, [](GLFWwindow *w, double x, double y) {
        auto self = (Window *) glfwGetWindowUserPointer(w);
        self->onMouseMove(x, y);
      });
    glfwSetKeyCallback(mWin, [](GLFWwindow *w, int k, int s, int a, int m) {
        auto self = (Window *) glfwGetWindowUserPointer(w);
        switch(a) {
          case GLFW_PRESS: self->onKeyPress(k); break;
          case GLFW_REPEAT: self->onKeyRepeat(k); break;
          case GLFW_RELEASE: self->onKeyRelease(k); break;
          default: break;
        }
      });
    glfwSetWindowSizeCallback(mWin, [](GLFWwindow *w, int width, int height) {
        auto self = (Window *) glfwGetWindowUserPointer(w);
        self->mWidth = width;
        self->mHeight = height;
      });
  }

  Window::~Window()
  {
    glfwDestroyWindow(mWin);
    glfwTerminate();
  }

  void Window::close()
  {
    glfwSetWindowShouldClose(mWin, 1);
  }

  void Window::start()
  {
    setup();
    while(!glfwWindowShouldClose(mWin)) {
      glPopMatrix();
      glfwSwapBuffers(mWin);
      glfwPollEvents();
      glfwGetFramebufferSize(mWin, &mWidth, &mHeight);
      glClear(GL_COLOR_BUFFER_BIT);
      glViewport(0, 0, mWidth, mHeight);
      glPushMatrix();
      glfwGetWindowSize(mWin, &mWidth, &mHeight);
      glOrtho(0, mWidth, mHeight, 0, -1, 1);
      loop();
    }
  }
  void Window::setup() { }
  void Window::loop() { }
  void Window::onLeftMouse(bool) { }
  void Window::onMouseScroll(double, double) { }
  void Window::onMouseMove(double, double) { }
  void Window::onKeyPress(int) { }
  void Window::onKeyRepeat(int) { }
  void Window::onKeyRelease(int) { }
}
