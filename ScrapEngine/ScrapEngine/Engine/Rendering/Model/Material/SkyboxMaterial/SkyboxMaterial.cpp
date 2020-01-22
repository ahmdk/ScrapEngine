#include <Engine/Rendering/Model/Material/SkyboxMaterial/SkyboxMaterial.h>
#include <Engine/Debug/DebugLog.h>
#include <Engine/Rendering/Pipeline/SkyboxPipeline/SkyboxVulkanGraphicsPipeline.h>
#include <Engine/Rendering/Texture/Texture/SkyboxTexture/SkyboxTexture.h>
#include <Engine/Rendering/Device/VulkanDevice.h>
#include <Engine/Rendering/Descriptor/DescriptorPool/StandardDescriptorPool/StandardDescriptorPool.h>

ScrapEngine::Render::SkyboxMaterial::~SkyboxMaterial()
{
	delete vulkan_texture_sampler_;
	delete vulkan_texture_image_view_;
	delete skybox_texture_;
	delete vulkan_render_descriptor_pool_;
}

void ScrapEngine::Render::SkyboxMaterial::create_pipeline(const std::string& vertex_shader_path,
                                                          const std::string& fragment_shader_path,
                                                          VulkanSwapChain* swap_chain)
{
	vulkan_render_graphics_pipeline_ = std::make_shared<SkyboxVulkanGraphicsPipeline>(vertex_shader_path.c_str(),
	                                                                                  fragment_shader_path.c_str(),
	                                                                                  &swap_chain->
	                                                                                  get_swap_chain_extent(),
	                                                                                  vulkan_render_descriptor_set_->
	                                                                                  get_descriptor_set_layout(),
	                                                                                  VulkanDevice::get_instance()->
	                                                                                  get_msaa_samples());
	Debug::DebugLog::print_to_console_log("VulkanGraphicsPipeline created");
}

void ScrapEngine::Render::SkyboxMaterial::create_skybox_texture(const std::array<std::string, 6>& textures_path)
{
	skybox_texture_ = new SkyboxTexture(textures_path);
	Debug::DebugLog::print_to_console_log("SkyboxTexture created");
	vulkan_texture_image_view_ = new TextureImageView(skybox_texture_->get_texture_image(),
	                                                  skybox_texture_->get_mip_levels(), true, 6);
	Debug::DebugLog::print_to_console_log("TextureImageView created");
	vulkan_texture_sampler_ = new TextureSampler(skybox_texture_->get_mip_levels(), vk::Filter::eLinear,
	                                             vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
	                                             vk::SamplerAddressMode::eClampToEdge,
	                                             vk::SamplerAddressMode::eClampToEdge,
	                                             vk::SamplerAddressMode::eClampToEdge,
	                                             false, vk::CompareOp::eNever, true, 16,
	                                             vk::BorderColor::eFloatOpaqueWhite);
}

void ScrapEngine::Render::SkyboxMaterial::create_descriptor_sets(VulkanSwapChain* swap_chain,
                                                                 SkyboxUniformBuffer* uniform_buffer)
{
	const size_t size = swap_chain->get_swap_chain_images_vector()->size();
	vulkan_render_descriptor_pool_ = new StandardDescriptorPool(size);
	vulkan_render_descriptor_set_->create_descriptor_sets(vulkan_render_descriptor_pool_->get_descriptor_pool(),
	                                                      size,
	                                                      uniform_buffer->get_uniform_buffers(),
	                                                      vulkan_texture_image_view_->get_texture_image_view(),
	                                                      vulkan_texture_sampler_->get_texture_sampler(),
	                                                      sizeof(SkyboxUniformBufferObject));
}
