#include "core/Engine.h"
#include "vulkan/vkEditor.h"
#include "core/Event.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>
#include "utils/Logger.h"

VulkanEditor::VulkanEditor(Engine* eng) : Editor(eng) {}

VulkanEditor::~VulkanEditor() { Shutdown(); }

void VulkanEditor::OnInitBackend() {}
void VulkanEditor::OnBeginFrameBackend() {}
void VulkanEditor::OnRenderBackend() {}
void VulkanEditor::OnShutdownBackend() {}


