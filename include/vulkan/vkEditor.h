#pragma once
#include "core/Editor.h"

class VulkanEditor : public Editor {
  public:
    VulkanEditor(Engine* engine);
    ~VulkanEditor() override;

  protected:
    void OnInitBackend() override;
    void OnBeginFrameBackend() override;
    void OnRenderBackend() override;
    void OnShutdownBackend() override;

  private:
    
};