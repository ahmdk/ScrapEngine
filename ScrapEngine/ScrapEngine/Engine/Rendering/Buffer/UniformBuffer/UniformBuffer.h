#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <Engine/LogicCore/Math/Transform/STransform.h>
#include <Engine/Rendering/Camera/Camera.h>
#include <glm/mat4x4.hpp>

namespace ScrapEngine
{
	namespace Render
	{
		struct UniformBufferObject
		{
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};

		class UniformBuffer
		{
		private:
			std::vector<vk::Buffer> uniform_buffers_;
			std::vector<vk::DeviceMemory> uniform_buffers_memory_;
			std::vector<void*> mapped_memory_;

			vk::Extent2D swap_chain_extent_;
			size_t swap_chain_images_size_;
		public:
			UniformBuffer(const std::vector<vk::Image>* swap_chain_images, const vk::Extent2D& input_swap_chain_extent);
			~UniformBuffer();

			void update_uniform_buffer(const uint32_t& current_image, const Core::STransform& object_transform,
			                           Camera* render_camera);
			const std::vector<vk::Buffer>* get_uniform_buffers() const;
			const std::vector<vk::DeviceMemory>* get_uniform_buffers_memory() const;
		};
	}
}