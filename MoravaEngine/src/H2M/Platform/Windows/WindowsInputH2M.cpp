/**
 * @package H2M (Hazel to Morava)
 * @author  Yan Chernikov (TheCherno)
 * @licence Apache License 2.0
 */

#include "Core/Input.h"

#include "Core/CommonValues.h"
#include "Core/Application.h"


bool Input::IsKeyPressed(KeyCodeH2M key)
{
	auto window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetHandle());
	auto state = glfwGetKey(window, static_cast<int32_t>(key));
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsMouseButtonPressed(MouseCodeH2M button)
{
	auto window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetHandle());
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_PRESS;
}

bool Input::IsMouseButtonReleased(MouseCodeH2M button)
{
	auto window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetHandle());
	auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
	return state == GLFW_RELEASE;
}

std::pair<float, float> Input::GetMousePosition()
{
	auto window = static_cast<GLFWwindow*>(Application::Get()->GetWindow()->GetHandle());
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	return { (float)xpos, (float)ypos };
}

float Input::GetMouseX()
{
	auto [x, y] = GetMousePosition();
	return x;
}

float Input::GetMouseY()
{
	auto [x, y] = GetMousePosition();
	return y;
}

// TODO: A better way to do this is to handle it internally, and simply move the cursor the opposite side
//              of the screen when it reaches the edge
void Input::SetCursorMode(CursorModeH2M mode)
{
	Window* window = Application::Get()->GetWindow();
	glfwSetInputMode(static_cast<GLFWwindow*>(window->GetHandle()), GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}

CursorModeH2M Input::GetCursorMode()
{
	Window* window = Application::Get()->GetWindow();
	return (CursorModeH2M)(glfwGetInputMode(static_cast<GLFWwindow*>(window->GetHandle()), GLFW_CURSOR) - GLFW_CURSOR_NORMAL);
}
