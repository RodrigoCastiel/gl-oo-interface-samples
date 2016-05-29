#include "glut_program.h"

void GlutProgram::Init(int* argc, char* argv[], const char* windowTitle)
{
  std::cout << "Initializing GLUT..." << std::endl;
  glutInit(argc,argv);

  std::cout << "Initializing OpenGL..." << std::endl;
  #ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL | GLUT_MULTISAMPLE);
  #else
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  #endif
  glutInitWindowSize(mWindowWidth, mWindowHeight);
  glutInitWindowPosition(0, 0);  
  glutCreateWindow(windowTitle);

  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
  std::cout << "Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

bool GlutProgram::LoadShaders(const char *shaderBasePath)
{
  std::cout << "---------------------------------------------\n";
  std::cout << "Loading shaders ...\n";
  std::cout << "---------------------------------------------\n";
  mPipelineProgram = new BasicPipelineProgram();
  int success = mPipelineProgram->Init(shaderBasePath);
  mProgramHandle = mPipelineProgram->GetProgramHandle();
  mPipelineProgram->Bind();
  std::cout << "---------------------------------------------\n";

  return success;
}

void GlutProgram::ReshapeFunc(int w, int h)
{
  mWindowWidth  = w;
  mWindowHeight = h;
}

void GlutProgram::PassiveMotionFunc(int x, int y)
{
  // Update current mouse coordinates after movement
  mMouse.mPos[0] = x;
  mMouse.mPos[1] = y;
}

void GlutProgram::MouseFunc(int button, int state, int x, int y)
{
  // NOTE: Next section from starter code.
  // a mouse button has has been pressed or depressed
  // keep track of the mouse button state, in leftMouseButton, middleMouseButton, rightMouseButton variables
  if (button == GLUT_LEFT_BUTTON)
  {
    mMouse.mLftButton = (state == GLUT_DOWN);
  }
  else if (button == GLUT_RIGHT_BUTTON)
  {
    mMouse.mRgtButton = (state == GLUT_DOWN);
  }
  else 
  {
    mMouse.mMiddleButton = (state == GLUT_DOWN);
  }

  // Update current mouse coordinates after movement
  mMouse.mPos[0] = x;
  mMouse.mPos[1] = y;
}

void GlutProgram::MotionFunc(int x, int y)
{
  // Update current mouse coordinates after movement
  mMouse.mPos[0] = x;
  mMouse.mPos[1] = y;
}

void GlutProgram::KeyboardFunc(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27: // ESC key
      exit(0);
    break;
  }
}
