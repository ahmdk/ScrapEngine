#include "VertexBuffer.h"

#include "Engine/Rendering/Buffer/BaseBuffer.h"
#include "Engine/Rendering/Buffer/StagingBuffer/StagingBuffer.h"

ScrapEngine::Render::VertexBuffer::VertexBuffer(const std::vector<ScrapEngine::Vertex>* vertices)
{
	const vk::DeviceSize buffer_size(sizeof((*vertices)[0]) * vertices->size());

	ScrapEngine::Render::StagingBuffer* staging = new StagingBuffer(buffer_size, vertices);

	BaseBuffer::create_buffer(buffer_size,
	                          vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
	                          vk::MemoryPropertyFlagBits::eDeviceLocal, vertex_buffer_, vertex_buffer_memory_);

	BaseBuffer::copy_buffer(staging->get_staging_buffer(), vertex_buffer_, buffer_size);

	delete staging;
}

ScrapEngine::Render::VertexBuffer::~VertexBuffer()
{
	VulkanDevice::static_logic_device_ref->destroyBuffer(vertex_buffer_);
	VulkanDevice::static_logic_device_ref->freeMemory(vertex_buffer_memory_);
}

vk::Buffer* ScrapEngine::Render::VertexBuffer::get_vertex_buffer()
{
	return &vertex_buffer_;
}
