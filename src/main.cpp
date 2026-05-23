#include "core/Editor.h"
#include "core/Engine.h"
#include "utils/Logger.h"

int main()
{
	CORE_INFO("Initializing Graphics API...");
	Engine* engine = new Engine();

	const int success = engine->Setup();
	if (success != 0) {
		return success;
	}
	CORE_INFO("API initialized successfully.");

	CORE_INFO("Creating Editor...");
	Editor editor(engine);
	CORE_INFO("Initializing Editor...");
	editor.Init();
	CORE_INFO("Editor initialized successfully.");
	
	/* ==== RENDER LOOP ==== */
	while (engine->IsRunning())
	{
		editor.BeginFrame();
		engine->BeginFrame();

		engine->ProcessInput();
		engine->Run();
		editor.Render();
		
		engine->EndFrame();
	}
	CORE_INFO("Shutting down engine and editor...");
	engine->Shutdown();
	editor.Shutdown();
	/* ==== END ==== */
	return 0;
}