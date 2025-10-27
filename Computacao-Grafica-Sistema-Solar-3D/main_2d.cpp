
#ifdef _WIN32
//#define GLAD_GL_IMPLEMENTATION // Necessary for headeronly version.
#include <glad/glad.h>
#elif __APPLE__
#include <OpenGL/gl3.h>
#endif
#include <GLFW/glfw3.h>

#include "arcball.h"
#include "scene.h"
#include "state.h"
#include "camera3d.h"
#include "color.h"
#include "material.h"
#include "transform.h"
#include "error.h"
#include "shader.h"
#include "triangle.h"
#include "sphere.h"
#include "texture.h"
#include "quad.h"
#include "light.h"

#include <iostream>
#include <cassert>

static float viewer_pos[3] = {0.0f, 0.0f, 10.0f};

static ScenePtr scene;
static Camera3DPtr camera;
static ArcballPtr arcball;

class OrbitTranslation;
using OrbitTranslationPtr = std::shared_ptr<OrbitTranslation>;

class OrbitTranslation : public Engine
{
    TransformPtr m_trf;
    float m_radius;
    float m_angle;
    float m_speed;

protected:
    OrbitTranslation(TransformPtr trf, float radius, float speed)
        : m_trf(trf), m_radius(radius), m_speed(speed), m_angle(0.0f)
    {
    }

public:
    static OrbitTranslationPtr Make(TransformPtr trf, float radius, float speed)
    {
        return OrbitTranslationPtr(new OrbitTranslation(trf, radius, speed));
    }

    virtual void Update(float dt)
    {
        m_angle += m_speed * -dt;

        m_trf->LoadIdentity();
        m_trf->Rotate(m_angle, 0, 0, 1);
        m_trf->Translate(m_radius, 0.0f, 0.0f);
    }
};

class PlanetRotation;
using PlanetRotationPtr = std::shared_ptr<PlanetRotation>;

class PlanetRotation : public Engine
{
    TransformPtr m_trf;
    float m_speed;

protected:
    PlanetRotation(TransformPtr trf, float speed)
        : m_trf(trf), m_speed(speed)
    {
    }

public:
    static PlanetRotationPtr Make(TransformPtr trf, float speed)
    {
        return PlanetRotationPtr(new PlanetRotation(trf, speed));
    }

    virtual void Update(float dt)
    {
        m_trf->Rotate(m_speed * -dt, 0, 0, 1);
    }
};


static void initialize (void)
{
  // set background color: white 
  glClearColor(0, 0, 0, 0);
  // enable depth test 
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);  // cull back faces

  // create objects
  camera = Camera3D::Make(viewer_pos[0],viewer_pos[1],viewer_pos[2]);
  //camera->SetOrtho(true);
  arcball = camera->CreateArcball();

  AppearancePtr white = Material::Make(1.0f,1.0f,1.0f);
  AppearancePtr red = Material::Make(1.0f,0.5f,0.5f);
  AppearancePtr yellow = Material::Make(1.0f,0.8f,0.0f);
  AppearancePtr blue = Material::Make(0.5f,0.5f,1.0f);
  AppearancePtr black = Material::Make(0.0f,0.0f,0.0f);

  LightPtr sunLight = Light::Make(0.0f, 0.0f, 0.0f, 1.0f, "object");

  // create shader for sun that is allways fully lit
  ShaderPtr shd_sun = Shader::Make(nullptr, "world");
  shd_sun->AttachVertexShader("./shaders/ilum_vert/vertex_sun.glsl");
  shd_sun->AttachFragmentShader("./shaders/ilum_vert/fragment_sun.glsl");
  shd_sun->Link();

  // Define a different shader for texture mapping
  // An alternative would be to use only this shader with a "white" texture for untextured objects
  ShaderPtr shd_tex = Shader::Make(sunLight,"world");
  shd_tex->AttachVertexShader("./shaders/ilum_vert/vertex_texture.glsl");
  shd_tex->AttachFragmentShader("./shaders/ilum_vert/fragment_texture.glsl");
  shd_tex->Link();

  //Moon setup
  auto moonSpriteTex = Texture::Make("decal", "images/moonmap.jpg");
  auto moonTrf = Transform::Make();
  moonTrf->Scale(0.1f, 0.1f, 0.1f);
  auto moon = Node::Make(shd_tex, moonTrf, { moonSpriteTex, white }, { Sphere::Make() }); //General and Sprite Node

  //Earth Setup
  auto earthSpriteTex = Texture::Make("decal", "images/earth.jpg");
  auto earthSpriteTrf = Transform::Make();
  auto moonOrbitTrf = Transform::Make();
  earthSpriteTrf->Scale(0.4f, 0.4f, 0.4f);

  auto earthSprite = Node::Make(shd_tex, earthSpriteTrf, { earthSpriteTex, white }, { Sphere::Make() }); //Earth Sprite Node
  auto moonOrbit = Node::Make(moonOrbitTrf, { moon });
  auto earth = Node::Make({earthSprite, moonOrbit}); //General Earth Node

  //Mercury Setup
  auto mercurySpriteTex = Texture::Make("decal", "images/mercurymap.jpg");
  auto mercurySpriteTrf = Transform::Make();
  mercurySpriteTrf->Scale(0.2f, 0.2f, 0.2f);
  
  auto mercurySprite = Node::Make(shd_tex, mercurySpriteTrf, { mercurySpriteTex, white }, { Sphere::Make() }); //Mercury Sprite Node
  auto mercury = Node::Make({mercurySprite}); //General Mercury Node
  
  //Sun Setup
  auto sunSpriteTex = Texture::Make("decal", "images/sunmap.jpg");
  auto sunSpriteTrf = Transform::Make();
  auto earthOrbitTrf = Transform::Make();
  auto mercuryOrbitTrf = Transform::Make();

  auto sunSprite = Node::Make(shd_sun, sunSpriteTrf, { sunSpriteTex, white }, { Sphere::Make() }); //Sprite Node
  auto earthOrbit = Node::Make(earthOrbitTrf, {earth}); //Orbit Node
  auto mercuryOrbit = Node::Make(mercuryOrbitTrf, {mercury}); //Orbit Node

  auto sun = Node::Make({sunSprite, earthOrbit, mercuryOrbit}); //General Sun Node

  //Space Setup
  //auto spaceSpriteTex = Texture::Make("face", "images/space.jpg");
  //auto spaceTrf = Transform::Make();
  //spaceTrf->Translate(-5.0f, -5.0f, 0.0f);
  //spaceTrf->Scale(20.0f, 10.0f, 1.0f);
  //auto spaceSprite = Node::Make(spaceTrf, { black }, { Quad::Make() }); //Space Sprite Node
  //auto space = Node::Make( { sun, spaceSprite } ); //Space Node

  // build scene
  auto root = Node::Make({ sun });
  scene = Scene::Make(root);
  scene->AddEngine(OrbitTranslation::Make(earthOrbitTrf, 3.5f, 10.0f));
  scene->AddEngine(OrbitTranslation::Make(moonOrbitTrf, 0.8f, 20.0f));
  scene->AddEngine(OrbitTranslation::Make(mercuryOrbitTrf, 2.0f, 55.0f));
  scene->AddEngine(PlanetRotation::Make(earthSpriteTrf, 100.0f));
  scene->AddEngine(PlanetRotation::Make(moonTrf, 50.0f));
  scene->AddEngine(PlanetRotation::Make(mercurySpriteTrf, 80.0f));
  scene->AddEngine(PlanetRotation::Make(sunSpriteTrf, 15.0f));
}

static void display (GLFWwindow* win)
{ 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear window 
  Error::Check("before render");
  scene->Render(camera);
  Error::Check("after render");
}

static void error (int code, const char* msg)
{
  printf("GLFW error %d: %s\n", code, msg);
  glfwTerminate();
  exit(0);
}

static void keyboard (GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void resize (GLFWwindow* win, int width, int height)
{
  glViewport(0,0,width,height);
}

static void cursorpos (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->AccumulateMouseMotion(int(x),int(y));
}
static void cursorinit (GLFWwindow* win, double x, double y)
{
  // convert screen pos (upside down) to framebuffer pos (e.g., retina displays)
  int wn_w, wn_h, fb_w, fb_h;
  glfwGetWindowSize(win, &wn_w, &wn_h);
  glfwGetFramebufferSize(win, &fb_w, &fb_h);
  x = x * fb_w / wn_w;
  y = (wn_h - y) * fb_h / wn_h;
  arcball->InitMouseMotion(int(x),int(y));
  glfwSetCursorPosCallback(win, cursorpos);     // cursor position callback
}
static void mousebutton (GLFWwindow* win, int button, int action, int mods)
{
  if (action == GLFW_PRESS) {
    glfwSetCursorPosCallback(win, cursorinit);     // cursor position callback
  }
  else // GLFW_RELEASE 
    glfwSetCursorPosCallback(win, nullptr);      // callback disabled
}

static void update (float dt)
{
  scene->Update(dt);
}

int main ()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);       // required for mac os
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);  // option for mac os
#endif

    glfwSetErrorCallback(error);

    GLFWwindow* win = glfwCreateWindow(1024, 600, "Window title", nullptr, nullptr);
    assert(win);
    glfwSetFramebufferSizeCallback(win, resize);  // resize callback
    glfwSetKeyCallback(win, keyboard);            // keyboard callback
    glfwSetMouseButtonCallback(win, mousebutton); // mouse button callback

    glfwMakeContextCurrent(win);
#ifdef __glad_h_
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD OpenGL context\n");
        exit(1);
    }
#endif
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));

  initialize();

  float t0 = float(glfwGetTime());
  while(!glfwWindowShouldClose(win)) {
    float t = float(glfwGetTime());
    update(t-t0);
    t0 = t;
    display(win);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }
  glfwTerminate();
  return 0;
}

