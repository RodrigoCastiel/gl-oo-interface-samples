#include "video_recorder.h"


void VideoRecorder::Start()
{
  if (!mIsRecording)
  {
    mIsRecording = true;
  }
}

void VideoRecorder::Stop()
{
  if (mIsRecording)
  {
    mCurrentVideoID++;
    mCurrentFrame = 0;
    mIsRecording = false;
  }
}

void VideoRecorder::ToggleRecord()
{
  if (!mIsRecording)
  {
    mIsRecording = true;
  }
  else if (mIsRecording)
  {
    mCurrentVideoID++;
    mIsRecording = false;
  }
}

void VideoRecorder::Update()
{
  if ((mIsRecording) && (mCurrentFrame%5 == 1))
  {
    char name[30];
    sprintf(name, "videos/%03d.jpg", mCurrentFrame/5);
    VideoRecorder::SaveScreenshot(name);
  }
  mCurrentFrame++;
}

void VideoRecorder::UpdateSize(int windowWidth, int windowHeight)
{
  mWindowWidth  = windowWidth;
  mWindowHeight = windowHeight;
}

void VideoRecorder::TakeScreenshot()
{
  char name[30];
  sprintf(name, "videos/screenshot_%03d.jpg", mCurrentScreenshot);
  VideoRecorder::SaveScreenshot(name);
  mCurrentScreenshot++;
}

// NOTE: This function was provided in the starter code.
// write a screenshot to the specified filename
void VideoRecorder::SaveScreenshot(const std::string& filename)
{
  unsigned char * screenshotData = new unsigned char[mWindowWidth * mWindowHeight * 3];

  glReadPixels(0, 0, mWindowWidth, mWindowHeight, GL_RGB, GL_UNSIGNED_BYTE, screenshotData);

  ImageIO screenshotImg(mWindowWidth, mWindowHeight, 3, screenshotData);

  if (screenshotImg.save(filename.c_str(), ImageIO::FORMAT_JPEG) == ImageIO::OK)
    std::cout << "File " << filename << " saved successfully." << std::endl;
  else std::cout << "Failed to save file " << filename << '.' << std::endl;

  delete [] screenshotData;
}