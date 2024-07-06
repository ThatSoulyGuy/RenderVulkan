#include "RenderVulkan/Core/Settings.hpp"
#include "RenderVulkan/Core/Window.hpp"
#include "RenderVulkan/Engine.hpp"

int main()
{
	try
	{
		RenderVulkan::Core::Settings::GetInstance()->Set<String>("defaultDomain", "RenderVulkan");
		RenderVulkan::Engine::GetInstance()->PreInitialize();
	
		RenderVulkan::Core::Window::GetInstance()->Create(RenderVulkan::Core::Settings::GetInstance()->Get<String>("windowTitle"), RenderVulkan::Core::Settings::GetInstance()->Get<Vector2i>("windowDimensions"));

		RenderVulkan::Engine::GetInstance()->Initialize();

		while (!RenderVulkan::Core::Window::GetInstance()->ShouldClose())
		{
			RenderVulkan::Engine::GetInstance()->Update();

			RenderVulkan::Core::Window::GetInstance()->Update();
			RenderVulkan::Engine::GetInstance()->Render();
		}

		RenderVulkan::Engine::GetInstance()->CleanUp();
		RenderVulkan::Core::Window::GetInstance()->CleanUp();
	}
	catch (const std::exception& exception)
	{
		std::cerr << "\n============================================================================\n                  Exception Occured! " << exception.what() << "\n============================================================================\n" << std::endl;
	}

	return 0;
}