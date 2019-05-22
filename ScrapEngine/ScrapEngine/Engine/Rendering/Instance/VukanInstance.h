#pragma once

#include <vulkan/vulkan.hpp>
#include <Engine/Rendering/ValidationLayers/VulkanValidationLayers.h>
#include <string>
#include <vector>

namespace ScrapEngine
{
	namespace Render
	{
		class VukanInstance
		{
		private:
			static VukanInstance* instance_;

			vk::Instance vulkan_instance_;
			//Vulkan Instance of the engine - connection between the application and the Vulkan library

			//Reference to ValidationLayersManager that display vulkan warning/errors
			ScrapEngine::Render::VulkanValidationLayers* validation_layers_manager_ = nullptr;

			VukanInstance() = default;
		public:
			void init(const std::string& app_name, int app_version, const std::string& engine_name = "ScrapEngine",
			          int engine_version = 1);

			//Destroy the Vulkan Instance
			~VukanInstance();

			static VukanInstance* get_instance();

			//Create the Vulkan Instance with the given data
			void create_vulkan_instance(std::string app_name, int app_version, std::string engine_name = "ScrapEngine",
			                            int engine_version = 1);

			//Return the current Vulkan Instance (if created)
			vk::Instance* get_vulkan_instance();
		private:
			std::vector<const char*> get_required_extensions() const;
		};
	}
}
