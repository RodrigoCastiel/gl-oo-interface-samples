#pragma once

#include <iostream>
#include <cstdio>

#include "openGLHeader.h"
#include "glutHeader.h"

#include "imageIO.h"

class VideoRecorder
{
public:
  void Start();
  void Stop();
  void ToggleRecord();

  void Update();

  void UpdateSize(int windowWidth, int windowHeight);

  void TakeScreenshot();

private:
  int mWindowWidth  {1280};
  int mWindowHeight { 720};

  int mCurrentVideoID {0};
  int mCurrentFrame   {0};
  bool mIsRecording {false};

  int mCurrentScreenshot {0};

  // NOTE: This method was adapted from "saveScreenshot" function provided in starter code.
  void SaveScreenshot(const std::string& filename = "screenshot.jpg");
};