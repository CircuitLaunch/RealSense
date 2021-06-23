#include <iostream>
#include "RealSense_Config.hpp"
#include <librealsense2/rs.hpp>
#include "Window.hpp"
#define GL_SILENCE_DEPRECATION
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <pcl/point_types.h>
#include <pcl/filters/passthrough.h>
#include <vector>

using namespace rs2;
using namespace std;
using namespace edj;
using namespace pcl;

class PointCloudWindow : public Window
{
  public:
    PointCloudWindow(int iWidth, int iHeight, const string &iTitle, float iYaw = 15.0, float iPitch = 15.0)
    : Window(iWidth, iHeight, iTitle),
      mYaw(iYaw), mPitch(iPitch),
      mLastX(0.0), mLastY(0.0),
      mMouseLeft(false),
      mOffsetX(0.0), mOffsetY(0.0),
      mTex(),
      mPipeline(),
      mPCLCloud(new PointCloud<PointXYZ>),
      mPCLFiltered(new PointCloud<PointXYZ>) { }

    virtual void setup() {
      mPipeline.start();
    }

    virtual void loop();

    virtual void onLeftMouse(bool iMouseLeft) {
      mMouseLeft = iMouseLeft;
    }

    virtual void onMouseScroll(double iOffsetX, double iOffsetY) {
      mOffsetX += iOffsetX;
      mOffsetY += iOffsetY;
    }

    virtual void onMouseMove(double iX, double iY) {
      if(mMouseLeft) {
        mYaw -= (iX - mLastX);
        if(mYaw < -120.0) mYaw = -120.0;
        if(mYaw > 120.0) mYaw = 120.0;
        mPitch -= (iY - mLastY);
        if(mPitch < -80.0) mPitch = -80.0;
        if(mPitch > 80.0) mPitch = 80.0;
      }
      mLastX = iX;
      mLastY = iY;
    }

    virtual void onKeyRelease(int iKey) {
      switch(iKey) {
        case 256:
          mYaw = mPitch = 0.0;
          mOffsetX = mOffsetY = 0.0;
          break;
        case 81:
          close();
          break;
      }
    }

    void initPCLCloud(points &iPointCloud);

    void setUp();
    void drawPointCloud(points &iPointCloud);
    void drawPCLPointClouds(vector<PointCloud<PointXYZ>::Ptr> &iPointClouds);
    void tearDown();

  protected:
    double mYaw;
    double mPitch;
    double mLastX;
    double mLastY;
    bool mMouseLeft;
    float mOffsetX;
    float mOffsetY;
    Texture mTex;

    pipeline mPipeline;

    PointCloud<PointXYZ>::Ptr mPCLCloud;
    PointCloud<PointXYZ>::Ptr mPCLFiltered;
};

void PointCloudWindow::initPCLCloud(points &iPointCloud)
{
  PointCloud<pcl::PointXYZ>::Ptr cloud(new PointCloud<PointXYZ>);

  auto sp = iPointCloud.get_profile().as<video_stream_profile>();

  mPCLCloud->width = sp.width();
  mPCLCloud->height = sp.height();
  mPCLCloud->is_dense = false;
  mPCLCloud->points.resize(iPointCloud.size());
  auto ptr = iPointCloud.get_vertices();
  for(auto &p : mPCLCloud->points) {
    p.x = ptr->x;
    p.y = ptr->y;
    p.z = ptr->z;
    ptr++;
  }
}

void PointCloudWindow::setUp()
{
  glLoadIdentity();
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  gluPerspective(60, mWidth / mHeight, 0.01f, 10.0f);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  gluLookAt(0, 0, 0, 0, 0, 1, 0, -1, 0);

  glTranslatef(0, 0, +0.5f + mOffsetY * 0.05f);
  glRotated(mPitch, 1, 0, 0);
  glRotated(mYaw, 0, 1, 0);
  glTranslatef(0, 0, -0.5f);
  glScalef(1.0, float(mWidth) / float(mHeight), 1.0);

  glPointSize(mWidth / 640);
}

void PointCloudWindow::drawPointCloud(points &iPointCloud)
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  mTex.bind();

  float texBorderColor[] = { 0.8f, 0.8f, 0.8f, 0.8f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texBorderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F); // GL_CLAMP_TO_EDGE
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F); // GL_CLAMP_TO_EDGE

  glBegin(GL_POINTS);

  auto vertices = iPointCloud.get_vertices();              // get vertices
  auto texCoords = iPointCloud.get_texture_coordinates(); // and texture coordinates
  for (int i = 0; i < iPointCloud.size(); i++) {
      if (vertices[i].z) {
          glVertex3fv(vertices[i]);
          glTexCoord2fv(texCoords[i]);
      }
  }

  glEnd();

  mTex.unbind();
}

typedef struct float3
{
  float3(float ix, float iy, float iz): x(ix), y(iy), z(iz) { }
  float3(const float3 &v): x(v.x), y(v.y), z(v.z) { }

  float x, y ,z;
} float3;

void PointCloudWindow::drawPCLPointClouds(vector<PointCloud<PointXYZ>::Ptr> &iPointClouds)
{
  glClear(GL_DEPTH_BUFFER_BIT);
  // glEnable(GL_DEPTH_TEST);
  // glEnable(GL_TEXTURE_2D);

  vector<float3> colors;
  colors.push_back(float3(1.0f, 0.0f, 0.0f));
  colors.push_back(float3(0.0f, 1.0f, 0.0f));

  int i = 0;
  for(auto &&pc : iPointClouds) {
    cout << "Rendering layer " << i << endl;
    glBegin(GL_POINTS);
    float3 &color = colors[i % iPointClouds.size()];
    glColor3f(color.x, color.y, color.z);

    for(int i = 0; i < pc->points.size(); i++) {
      auto &&p = pc->points[i];
      if(p.z) {
        glVertex3f(p.x, p.y, p.z);
      }
    }

    glEnd();

    i++;
  }
}

void PointCloudWindow::tearDown()
{
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glPopAttrib();
}

void PointCloudWindow::loop()
{
  auto frames = mPipeline.wait_for_frames();
  auto depth = frames.get_depth_frame();
  auto color = frames.get_color_frame();

  if(!color) color = frames.get_infrared_frame();

  pointcloud pc;
  points p;

  pc.map_to(color);
  p = pc.calculate(depth);

  setUp();

  GLint glIntFormat;
  GLenum glFormat, glType;

  switch(color.get_profile().format()) {
    case RS2_FORMAT_RGB8:
      glIntFormat = GL_RGB;
      glFormat = GL_RGB;
      glType = GL_UNSIGNED_BYTE;
      break;
    case RS2_FORMAT_RGBA8:
      glIntFormat = GL_RGBA;
      glFormat = GL_RGBA;
      glType = GL_UNSIGNED_BYTE;
      break;
    case RS2_FORMAT_Y8:
      glIntFormat = GL_RGB;
      glFormat = GL_LUMINANCE;
      glType = GL_UNSIGNED_BYTE;
      break;
    case RS2_FORMAT_Y10BPACK:
      glIntFormat = GL_LUMINANCE;
      glFormat = GL_LUMINANCE;
      glType = GL_UNSIGNED_SHORT;
      break;
  }

  int width = color.get_width();
  int height = color.get_height();

  mTex.upload(glIntFormat, glFormat, glType, width, height, color.get_data());

  drawPointCloud(p);

  initPCLCloud(p);
  PassThrough<PointXYZ> pass;
  pass.setInputCloud(mPCLCloud);
  pass.setFilterFieldName("z");
  pass.setFilterLimits(0.0, 0.5);
  pass.filter(*mPCLFiltered);
  vector<PointCloud<PointXYZ>::Ptr> layers;
  layers.push_back(mPCLFiltered);

  drawPCLPointClouds(layers);

  tearDown();
}

int main(int c, char **v)
{
  PointCloudWindow win(1280, 720, "PointCloud Test");
  win.start();
}
