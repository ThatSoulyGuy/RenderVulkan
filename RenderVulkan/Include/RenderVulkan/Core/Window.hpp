#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "RenderVulkan/Core/Logger.hpp"

namespace RenderVulkan
{
	namespace Core
	{
		class Window
		{

		public:

			Window(const Window&) = delete;
			Window& operator=(const Window&) = delete;

			void Create(const String& title, const Vector2i& dimensions)
			{
				Logger_WriteConsole("Creating window...", LogLevel::INFORMATION);

				if (!glfwInit())
					Logger_ThrowException("Failed to initialize GLFW", true);
				

				glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
				glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

				handle = glfwCreateWindow(dimensions.x, dimensions.y, title.c_str(), nullptr, nullptr);

				if (!handle)
				{
					glfwTerminate();
					Logger_ThrowException("Failed to create GLFW window", true);
				}
				
				glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* window, int width, int height)
				{
					Settings::GetInstance()->Get<Function<void(Vector2i)>>("windowResizeCallback")({width, height});
				});
			}

			void Update()
			{
				glfwPollEvents();
			}

			bool ShouldClose()
			{
				return glfwWindowShouldClose(handle);
			}

			GLFWwindow* GetHandle()
			{
				return handle;
			}
			
			void SetDimensions(const Vector2i& dimensions)
			{
				glfwSetWindowSize(handle, dimensions.x, dimensions.y);
			}

			Vector2i GetDimensions()
			{
				Vector2i dimensions = {};

				glfwGetWindowSize(handle, &dimensions.x, &dimensions.y);

				return dimensions;
			}

			void SetPosition(const Vector2i& position)
			{
				glfwSetWindowPos(handle, position.x, position.y);
			}

			Vector2i GetPosition()
			{
				Vector2i position = {};

				glfwGetWindowPos(handle, &position.x, &position.y);

				return position;
			}

			void CleanUp()
			{
				if (handle)
				{
					glfwDestroyWindow(handle);
					glfwTerminate();
				}
			}

			static Shared<Window> GetInstance()
			{
				class EnabledWindow : public Window { };
				
				static Shared<Window> instance = std::make_shared<EnabledWindow>();

				return instance;
			}

		private:

			Window() = default;

			GLFWwindow* handle = nullptr;

		};
	}
}