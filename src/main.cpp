#include "vulkan/vkEditor.h"
#include "vulkan/vkEngine.h"
#include "core/Engine.h"
#include "utils/Logger.h"

int main()
{
	try{
		CORE_INFO("Initializing Graphics API...");
		VKEngine* engine = new VKEngine();

		if (engine->Setup() != 0)  return -1;

		CORE_INFO("API initialized successfully.");

		/* CORE_INFO("Creating Editor..."); */
		/* GLEditor editor(engine); */
		/* VulkanEditor editor(engine); */
		/* CORE_INFO("Initializing Editor...");
		editor.Init();
		CORE_INFO("Editor initialized successfully."); */
		
		/* ==== RENDER LOOP ==== */
		 while (engine->IsRunning())
		{ 
			/* editor.BeginFrame(); */
			engine->BeginFrame();

			engine->ProcessInput();
			engine->Run();
			/* editor.Render(); */

			engine->EndFrame();
		} 
		CORE_INFO("Shutting down engine (and editor)...");
		engine->Shutdown();
		/* editor.Shutdown(); */
		/* ==== END ==== */
		return 0;
	} catch (const std::exception& e)
	{
		CORE_ERROR(e.what());
	}
	
}