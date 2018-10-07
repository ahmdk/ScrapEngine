#include "EngineManager.h"

#include "../Rendering/Base/SimpleTestData.h"

ScrapEngine::EngineManager::EngineManager(std::string app_name, int app_version, uint32_t window_WIDTH, uint32_t window_HEIGHT, bool fullscreen)
	: received_base_game_info(app_name, app_version, window_WIDTH, window_HEIGHT, fullscreen)
{
	initializeEngine();
}

ScrapEngine::EngineManager::~EngineManager()
{
	if (!cleanupDone) { //Check if the cleanup has already been done
		cleanupEngine();
	}
}

void ScrapEngine::EngineManager::StartGameLoop()
{
	//Execute Game Objects start events
	ScrapLogicManager->ExecuteGameObjectsStartEvent();
	//Execute game loop until end
	mainGameLoop();
	//Execution ended, close the engine
	cleanupEngine();
	DebugLog::printToConsoleLog("---Engine Execution ended---");
}

const ScrapEngine::GameWindow* ScrapEngine::EngineManager::getGameWindow() const
{
	return ScrapRenderManager->getGameWindow();
}

void ScrapEngine::EngineManager::initializeEngine()
{
	DebugLog::printToConsoleLog("---initializeEngine()---");
	initializeRenderManager(&received_base_game_info);
	initializeLogicManager();
	DebugLog::printToConsoleLog("---initializeEngine() completed---");
}

void ScrapEngine::EngineManager::initializeRenderManager(const ScrapEngine::game_base_info* game_info)
{
	ScrapRenderManager = new RenderManager(game_info);
}

void ScrapEngine::EngineManager::initializeLogicManager()
{
	ScrapLogicManager = new LogicManager();
}

void ScrapEngine::EngineManager::mainGameLoop()
{
	DebugLog::printToConsoleLog("---mainGameLoop() started---");
	const ScrapEngine::GameWindow* window_ref = ScrapRenderManager->getGameWindow();
	while (!window_ref->checkWindowShouldClose()) {
		ScrapLogicManager->ExecuteGameObjectsUpdateEvent();
		ScrapRenderManager->drawFrame();
	}
	ScrapRenderManager->waitDeviceIdle();
	DebugLog::printToConsoleLog("---mainGameLoop() ended---");
}

void ScrapEngine::EngineManager::cleanupEngine()
{
	DebugLog::printToConsoleLog("---cleanupEngine()---");
	
	delete ScrapRenderManager;
	delete ScrapLogicManager;
	
	cleanupDone = true;
	DebugLog::printToConsoleLog("---cleanupEngine() completed---");
}
