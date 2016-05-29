#include "scene.h"

namespace gloo
{

void Scene::Init(BasicPipelineProgram* pipelineProgram, GLuint programHandle)
{
  mPipelineProgram = pipelineProgram;
  mProgramHandle = programHandle;
  mInitialized = true;

  // Add a default camera.
  Camera* defaultCamera = new Camera(pipelineProgram, programHandle);
  defaultCamera->SetCameraType(Camera::EDITOR);
  mCameras.push_back(defaultCamera);
}

void Scene::Render()
{
  if (Scene::IsInitialized())
  { 
    int numLights = mLights.size();
    GLuint numLightsLoc = glGetUniformLocation(mProgramHandle, "numLights");
    glUniform1i(numLightsLoc, numLights);

    // Position current camera.
    mCameras[mCurrentCamera]->Position();
    OpenGLMatrix& currentView = mCameras[mCurrentCamera]->GetViewMatrix();

    // Set all lights.
    for (int i = 0; i < numLights; i++)
    {
      mLights[i]->Position(currentView, i);
    }

    // Render all objects.
    for (auto object : mObjects)
    {
      object->Render();
    }
  }
}

void Scene::ChangeCamera()
{
  int n = mCameras.size();
  mCurrentCamera = (mCurrentCamera + 1) % n;
}

void Scene::Animate()
{
  for (auto camera : mCameras)
  {
    camera->Animate();
  }

  for (auto light : mLights)
  {
    light->Animate();
  }

  for (auto object : mObjects)
  {
    object->Animate();
  }
}

void Scene::ReshapeScreen(int w, int h)
{
  glViewport(0, 0, w, h);

  // Update each camera's projection.
  for (auto camera : mCameras)
  {
    camera->Project(0, 0, w, h);
  }
}

void Scene::Clean()
{
  for (auto object : mObjects)
  {
    delete object;
  }

  for (auto light : mLights)
  {
    delete light;
  }

  for (auto camera : mCameras)
  {
    delete camera;
  }
}

SceneObject* Scene::SelectObject(int x, int y, int w, int h)
{
  // Cast ray from the camera to the environment.
  glm::vec3 r = mCameras[mCurrentCamera]->ComputeRayAt(x, y, w, h);
  glm::vec3 C = mCameras[mCurrentCamera]->GetCenterCoordinates();

  for (auto object : mObjects)
  {
    if(object->IntersectRay(r, C))
    {
      return object;
    }
  }

  return nullptr;
}

void Scene::Add(Camera::CameraType type)
{
  // Add a default camera.
  Camera* camera = new Camera(mPipelineProgram, mProgramHandle);
  camera->SetCameraType(type);
  mCameras.push_back(camera);
}

void Scene::Add(Camera* camera)
{
  if (camera) 
  {
    mCameras.push_back(camera);  
  }
}

void Scene::Add(SceneObject* object)
{
  if (object)
  {
    mObjects.push_back(object);
  }
}

void Scene::Add(Light* light)
{
  mLights.push_back(light);
}

}  // namespace gloo.