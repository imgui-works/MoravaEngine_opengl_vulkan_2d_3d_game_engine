#pragma once

#include "Hazel/Core/Layer.h"

#include "Core/Window.h"
#include "Scene/Scene.h"


/***
 * Slightly modified version of Hazel::Layer to be used in MoravaEngine environment
 */
class MoravaLayer : public Hazel::Layer
{
public:
	MoravaLayer(const std::string& name = "MoravaLayer");
	virtual ~MoravaLayer();

	virtual void OnAttach() = 0;
	virtual void OnDetach() = 0;
	virtual void OnUpdate(Hazel::Timestep ts) = 0;
	virtual void OnEvent(Event& event) = 0;
	virtual void OnRender() override {}; // Hazel version
	virtual void OnRender(Window* mainWindow, Scene* scene) = 0; // MoravaEngine version
	virtual void OnImGuiRender() override {}; // Hazel version
	virtual void OnImGuiRender(Window* mainWindow, Scene* scene) = 0; // MoravaEngine version

};