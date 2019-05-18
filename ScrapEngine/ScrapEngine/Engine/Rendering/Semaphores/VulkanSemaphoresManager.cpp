#include "VulkanSemaphoresManager.h"

#include <stdexcept>
#include "../Base/StaticTypes.h"

ScrapEngine::Render::VulkanSemaphoresManager::VulkanSemaphoresManager(unsigned short int INPUT_MAX_FRAMES_IN_FLIGHT)
	: MAX_FRAMES_IN_FLIGHT(INPUT_MAX_FRAMES_IN_FLIGHT)
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	vk::SemaphoreCreateInfo semaphoreInfo;

	vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (VulkanDevice::static_logic_device_ref->createSemaphore(&semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != vk::Result::eSuccess ||
			VulkanDevice::static_logic_device_ref->createSemaphore(&semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != vk::Result::eSuccess ||
			VulkanDevice::static_logic_device_ref->createFence(&fenceInfo, nullptr, &inFlightFences[i]) != vk::Result::eSuccess) {
			throw std::runtime_error("Failed to create VulkanSemaphoresManager for a frame!");
		}
	}
}

ScrapEngine::Render::VulkanSemaphoresManager::~VulkanSemaphoresManager()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VulkanDevice::static_logic_device_ref->destroySemaphore(renderFinishedSemaphores[i]);
		VulkanDevice::static_logic_device_ref->destroySemaphore(imageAvailableSemaphores[i]);
		VulkanDevice::static_logic_device_ref->destroyFence(inFlightFences[i]);
	}
}

int ScrapEngine::Render::VulkanSemaphoresManager::getMaxFramesInFlight() const
{
	return MAX_FRAMES_IN_FLIGHT;
}

const std::vector<vk::Semaphore>* ScrapEngine::Render::VulkanSemaphoresManager::getImageAvailableSemaphoresVector()
{
	return &imageAvailableSemaphores;
}

const std::vector<vk::Semaphore>* ScrapEngine::Render::VulkanSemaphoresManager::getRenderFinishedSemaphoresVector()
{
	return &renderFinishedSemaphores;
}

const std::vector<vk::Fence>* ScrapEngine::Render::VulkanSemaphoresManager::getInFlightFencesVector()
{
	return &inFlightFences;
}
