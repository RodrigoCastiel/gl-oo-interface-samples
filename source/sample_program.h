/******************************************+
*                                          *
*  Author: Rodrigo Castiel                 *
*                                          *
+*******************************************/

#pragma once

#include "scene.h"
#include "glut_program.h"
#include "video_recorder.h"

#include "object.h"

using namespace gloo;

enum ControlState { kROTATE, kTRANSLATE, kSCALE, kEDIT };

class SampleProgram : public GlutProgram
{
 public:
  SampleProgram();
  virtual ~SampleProgram()
  {
    delete mVideoRecorder;
    delete mScene;

    delete blah;
  }

  void Init(int* argc, char* argv[], const char *windowTitle);

  // NOTE: This method is based on the starter code function "initScene".
  void InitScene(int argc, char *argv[]);

  void DisplayFunc(void);          // Render function.
  void IdleFunc(void);             // Animation/update callback.
  void ReshapeFunc(int w, int h);  // Window resize callback.

  void PassiveMotionFunc(int x, int y);                 // Mouse motion callback.
  void MouseFunc(int button, int state, int x, int y);  // Mouse button callback.
  void MotionFunc(int x, int y);                        // Mouse drag callback.
  void KeyboardFunc(unsigned char key, int x, int y);   // Key pressed.

 private:
  Scene* mScene                 { nullptr };
  VideoRecorder *mVideoRecorder { nullptr };

  ControlState mControlState {kROTATE};

  obj::Object* blah;
};