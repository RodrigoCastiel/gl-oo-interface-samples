/* ----------------------------------------------------
*   This base class provides an objected-oriented 
*   interface for GLUT programs.
*
*   Student: Rodrigo Castiel <castielr@usc.edu>
*  ----------------------------------------------------
*/

#pragma once

#include <iostream>

#include "openGLHeader.h"
#include "glutHeader.h"

#include "basicPipelineProgram.h"

struct MouseState
{
  int mPos[2] = {0, 0};          // (x,y) coordinates of the mouse position
  int mLftButton {0};              // 1 if pressed, 0 if not 
  int mRgtButton {0};              // 1 if pressed, 0 if not
  int mMiddleButton {0};           // 1 if pressed, 0 if not
};

class GlutProgram
{
public:
  GlutProgram() { }

  virtual ~GlutProgram()
  {
    delete mPipelineProgram;
  }

  bool LoadShaders(const char *shaderBasePath);
  GLuint GetProgramHandle() { return mProgramHandle; }

  // Pure virtual methods - derived class must implement.
  virtual void Init(int* argc, char* argv[], const char* windowTitle);
  virtual void DisplayFunc(void) = 0;          // Render function.
  virtual void IdleFunc(void) = 0;             // Animation/update callback.
  virtual void ReshapeFunc(int w, int h);      // Window resize callback.


  virtual void PassiveMotionFunc(int x, int y);                 // Mouse motion callback.
  virtual void MouseFunc(int button, int state, int x, int y);  // Mouse button callback.
  virtual void MotionFunc(int x, int y);                        // Mouse drag callback.
  
  virtual void KeyboardFunc(unsigned char key, int x, int y);  // Key pressed.
  virtual void Run() { glutMainLoop(); }

protected:
  BasicPipelineProgram *mPipelineProgram { nullptr };
  GLuint mProgramHandle { 0 };

  MouseState mMouse;
  int mWindowWidth   {1280};
  int mWindowHeight  { 720};
};