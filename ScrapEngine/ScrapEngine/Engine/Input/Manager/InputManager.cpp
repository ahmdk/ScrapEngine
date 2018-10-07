#include "InputManager.h"
#include "../../Rendering/Window/GameWindow.h"
#include <stb_image.h>


ScrapEngine::InputManager::InputManager(GLFWwindow* window) : windowRef(window)
{
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
}

ScrapEngine::InputManager::~InputManager()
{
	glfwDestroyCursor(cursor);
}

ScrapEngine::MouseLocation ScrapEngine::InputManager::getLastMouseLocation()
{
	MouseLocation cursorPos;
	glfwGetCursorPos(windowRef, &cursorPos.xpos, &cursorPos.ypos);
	return cursorPos;
}

void ScrapEngine::InputManager::SetCursorInputMode(ScrapEngine::CursorMode NewMode)
{
	switch (NewMode) {
	case ScrapEngine::CursorMode::cursor_normal_mode:
		glfwSetInputMode(windowRef, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case ScrapEngine::CursorMode::cursor_hidden_mode:
		glfwSetInputMode(windowRef, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		break;
	case ScrapEngine::CursorMode::cursor_grabbed_mode:
		glfwSetInputMode(windowRef, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	}
}

GLFWimage ScrapEngine::InputManager::loadIcon(const std::string & path_to_file)
{
	GLFWimage icon;
	icon.pixels = stbi_load(path_to_file.c_str(), &icon.width, &icon.height, 0, 4);
	return icon;
}

void ScrapEngine::InputManager::LoadNewCursor(const std::string & path_to_file, int xhot, int yhot)
{
	glfwSetCursor(windowRef, NULL);
	glfwDestroyCursor(cursor);

	cursor = glfwCreateCursor(&loadIcon(path_to_file), xhot, yhot);

	glfwSetCursor(windowRef, cursor);
}

void ScrapEngine::InputManager::LoadSystemCursor(ScrapEngine::SystemCursorShapes NewShape)
{
	glfwSetCursor(windowRef, NULL);
	glfwDestroyCursor(cursor);

	switch (NewShape) {
	case ScrapEngine::SystemCursorShapes::cursor_regular_arrow:
		cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		break;
	case ScrapEngine::SystemCursorShapes::cursor_crosshair_shape:
		cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		break;
	case ScrapEngine::SystemCursorShapes::cursor_hand_shape:
		cursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
		break;
	case ScrapEngine::SystemCursorShapes::cursor_horizontal_resize_arrow_shape:
		cursor = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		break;
	case ScrapEngine::SystemCursorShapes::cursor_vertical_resize_arrow_shape:
		cursor = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		break;
	case ScrapEngine::SystemCursorShapes::cursor_input_beam_shape:
		cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		break;
	}

	glfwSetCursor(windowRef, cursor);
}

void ScrapEngine::InputManager::ResetCursorToSystemDefault()
{
	LoadSystemCursor(ScrapEngine::SystemCursorShapes::cursor_regular_arrow);
}

void ScrapEngine::InputManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
}

void ScrapEngine::InputManager::cursor_position_callback(GLFWwindow * window, double xpos, double ypos)
{
}
