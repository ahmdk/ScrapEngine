#include <imgui.h>
#include <Engine/Rendering/Gui/VulkanImGui.h>
#include <Engine/Rendering/Texture/Texture/BaseTexture.h>
#include <Engine/Rendering/Buffer/StagingBuffer/ImageStagingBuffer/ImageStagingBuffer.h>
#include <Engine/Rendering/Device/VulkanDevice.h>
#include <Engine/Rendering/Descriptor/DescriptorPool/GuiDescriptorPool/GuiDescriptorPool.h>

ScrapEngine::Render::VulkanImGui::VulkanImGui()
{
	ImGui::CreateContext();

	//Create empty buffers, 2 for 2 command buffers
	for (int i = 0; i < 2; i++)
	{
		vertex_buffer_vector_.push_back(new GenericBuffer());
		index_buffer_vector_.push_back(new GenericBuffer());
		vertex_count_vector_.push_back(0);
		index_count_vector_.push_back(0);
	}
}

ScrapEngine::Render::VulkanImGui::~VulkanImGui()
{
	ImGui::DestroyContext();
	// Release all Vulkan resources required for rendering imGui
	vk::Device* device = VulkanDevice::get_instance()->get_logical_device();

	//Clear buffers
	for (int i = 0; i < 2; i++)
	{
		delete vertex_buffer_vector_[i];
		delete index_buffer_vector_[i];
	}
	vertex_buffer_vector_.clear();
	index_buffer_vector_.clear();
	//Clear other resources
	device->destroyImage(front_image_);
	delete front_view_;
	device->freeMemory(font_memory_);
	delete sampler_;
	delete pipeline_;
	delete descriptor_pool_;
	delete descriptor_set_;
}

void ScrapEngine::Render::VulkanImGui::init(const float width, const float height)
{
	// Dimensions
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2(width, height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

void ScrapEngine::Render::VulkanImGui::init_resources(VulkanSwapChain* swap_chain)
{
	ImGuiIO& io = ImGui::GetIO();

	// Create font texture
	unsigned char* font_data;
	int tex_width, tex_height;
	io.Fonts->GetTexDataAsRGBA32(&font_data, &tex_width, &tex_height);
	const vk::DeviceSize upload_size = tex_width * tex_height * 4 * sizeof(char);

	// Create target image for copy
	BaseTexture::create_image(tex_width, tex_height, vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal,
	                          vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
	                          vk::MemoryPropertyFlagBits::eDeviceLocal,
	                          front_image_, font_memory_, 1, vk::SampleCountFlagBits::e1);

	// Image view
	front_view_ = new TextureImageView(&front_image_, 1);

	// Prepare for transfer
	BaseTexture::transition_image_layout(&front_image_, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined,
	                                     vk::ImageLayout::eTransferDstOptimal, 1);
	//Prepare buffer
	ImageStagingBuffer* staginf_buffer_ref = new ImageStagingBuffer(upload_size, font_data);
	//Set copy region data
	vk::BufferImageCopy buffer_copy_region;
	vk::ImageSubresourceLayers image_subresource_layer;
	image_subresource_layer.setAspectMask(vk::ImageAspectFlagBits::eColor);
	image_subresource_layer.setLayerCount(1);
	buffer_copy_region.setImageSubresource(image_subresource_layer);
	buffer_copy_region.setImageExtent(vk::Extent3D(tex_width, tex_height, 1));
	//Copy
	ImageStagingBuffer::copy_buffer_to_image(staginf_buffer_ref->get_staging_buffer(), &front_image_, tex_width,
	                                         tex_height, &buffer_copy_region, 1,
	                                         vk::ImageLayout::eTransferDstOptimal);
	delete staginf_buffer_ref;
	staginf_buffer_ref = nullptr;
	// Prepare for shader read
	BaseTexture::transition_image_layout(&front_image_, vk::Format::eR8G8B8A8Unorm,
	                                     vk::ImageLayout::eTransferDstOptimal,
	                                     vk::ImageLayout::eShaderReadOnlyOptimal, 1);
	// Font texture Sampler
	sampler_ = new TextureSampler(1, vk::Filter::eLinear, vk::Filter::eLinear, vk::SamplerMipmapMode::eLinear,
	                              vk::SamplerAddressMode::eClampToEdge, vk::SamplerAddressMode::eClampToEdge,
	                              vk::SamplerAddressMode::eClampToEdge, false, vk::CompareOp::eNever, false, 16,
	                              vk::BorderColor::eFloatOpaqueWhite);

	// Descriptor pool
	descriptor_pool_ = new GuiDescriptorPool(swap_chain->get_swap_chain_images_vector());
	// Descriptor set
	descriptor_set_ = new GuiDescriptorSet();
	descriptor_set_->create_descriptor_sets(descriptor_pool_->get_descriptor_pool(),
	                                        swap_chain->get_swap_chain_images_vector(),
	                                        sampler_->get_texture_sampler(), front_view_->get_texture_image_view());

	//Create pipeline
	pipeline_ = new GuiVulkanGraphicsPipeline("../assets/shader/compiled_shaders/ui.vert.spv",
	                                          "../assets/shader/compiled_shaders/ui.frag.spv",
	                                          &swap_chain->get_swap_chain_extent(),
	                                          descriptor_set_->get_descriptor_set_layout(), sizeof(PushConstBlock));

	//Empty frame initialization
	generate_gui_frame();
}

void ScrapEngine::Render::VulkanImGui::init_reference(Core::LogicManager* logic_manager_ref)
{
	logic_manager_ref_ = logic_manager_ref;
}

void ScrapEngine::Render::VulkanImGui::update_buffers(short int index)
{
	ImDrawData* im_draw_data = ImGui::GetDrawData();

	const vk::DeviceSize vertex_buffer_size = im_draw_data->TotalVtxCount * sizeof(ImDrawVert);
	const vk::DeviceSize index_buffer_size = im_draw_data->TotalIdxCount * sizeof(ImDrawIdx);

	if (vertex_buffer_size == 0 || index_buffer_size == 0)
	{
		return;
	}

	// Vertex buffer
	GenericBuffer* vertex_buffer = vertex_buffer_vector_[index];
	if ((vertex_buffer->get_buffer() == nullptr) || (vertex_count_vector_[index] != im_draw_data->TotalVtxCount))
	{
		vertex_buffer->unmap();
		vertex_buffer->destroy();
		//Create it
		vk::BufferCreateInfo vertex_buffer_info;
		vertex_buffer_info.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
		vertex_buffer_info.setSize(vertex_buffer_size);
		vertex_buffer->create_buffer(vertex_buffer_info);
		//Update data
		vertex_count_vector_[index] = im_draw_data->TotalVtxCount;
		vertex_buffer->map();
	}

	// Index buffer
	GenericBuffer* index_buffer = index_buffer_vector_[index];
	if ((index_buffer->get_buffer() == nullptr) || (index_count_vector_[index] < im_draw_data->TotalIdxCount))
	{
		index_buffer->unmap();
		index_buffer->destroy();
		vk::BufferCreateInfo index_buffer_info;
		index_buffer_info.setUsage(vk::BufferUsageFlagBits::eIndexBuffer);
		index_buffer_info.setSize(index_buffer_size);
		index_buffer->create_buffer(index_buffer_info);
		index_count_vector_[index] = im_draw_data->TotalIdxCount;
		index_buffer->map();
	}

	// Upload data
	ImDrawVert* vtx_dst = reinterpret_cast<ImDrawVert*>(vertex_buffer->get_mapped_memory());
	ImDrawIdx* idx_dst = reinterpret_cast<ImDrawIdx*>(index_buffer->get_mapped_memory());

	for (int n = 0; n < im_draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = im_draw_data->CmdLists[n];
		memcpy(vtx_dst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		vtx_dst += cmd_list->VtxBuffer.Size;
		idx_dst += cmd_list->IdxBuffer.Size;
	}

	// Flush to make writes visible to GPU
	vertex_buffer->flush();
	index_buffer->flush();
}

void ScrapEngine::Render::VulkanImGui::generate_gui_frame() const
{
	//prepare the ongui() event generating a new gui event
	ImGui::NewFrame();
	//Call ongui
	if (logic_manager_ref_)
	{
		logic_manager_ref_->execute_game_objects_ongui_event();
	}
	//Render to generate draw buffers
	ImGui::Render();
}

ScrapEngine::Render::GuiDescriptorSet* ScrapEngine::Render::VulkanImGui::get_descriptor_set() const
{
	return descriptor_set_;
}

ScrapEngine::Render::GuiVulkanGraphicsPipeline* ScrapEngine::Render::VulkanImGui::get_pipeline() const
{
	return pipeline_;
}

ScrapEngine::Render::GenericBuffer* ScrapEngine::Render::VulkanImGui::get_vertex_buffer(short int index) const
{
	return vertex_buffer_vector_[index];
}

ScrapEngine::Render::GenericBuffer* ScrapEngine::Render::VulkanImGui::get_index_buffer(short int index) const
{
	return index_buffer_vector_[index];
}

ScrapEngine::Render::VulkanImGui::PushConstBlock* ScrapEngine::Render::VulkanImGui::get_push_const_block()
{
	return &push_const_block_;
}
