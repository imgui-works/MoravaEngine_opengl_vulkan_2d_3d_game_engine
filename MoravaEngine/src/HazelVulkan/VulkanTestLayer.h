#pragma once

#include "Hazel/Core/Timestep.h"
#include "Hazel/Events/Event.h"
#include "Hazel/Platform/Vulkan/VulkanPipeline.h"
#include "Hazel/Platform/Vulkan/VulkanShader.h"

#include "Core/Window.h"
#include "Scene/Scene.h"


class VulkanTestLayer
{
public:
	VulkanTestLayer();
	~VulkanTestLayer();

	void OnAttach();
	void OnDetach();

	void OnUpdate(Hazel::Timestep ts, glm::mat4 viewMatrix);
	void OnImGuiRender(Window* mainWindow, Scene* scene);

	void OnEvent(Event& event);

	void ShowExampleAppDockSpace(bool* p_open, Window* mainWindow);
	void OnRender(Window* mainWindow);

private:
	void BuildCommandBuffer(const glm::vec4& clearColor, glm::mat4 viewMatrix);

private:
	Hazel::Ref<Hazel::HazelMesh> m_Mesh;
	Hazel::Ref<Hazel::HazelShader> m_Shader;
	Hazel::Ref<Hazel::HazelShader> m_ShaderHazelPBR_Static;
	Hazel::Ref<Hazel::Pipeline> m_Pipeline;
	Hazel::Ref<Hazel::VertexBuffer> m_VertexBuffer;
	Hazel::Ref<Hazel::IndexBuffer> m_IndexBuffer;

	Hazel::EditorCamera m_Camera;

	std::vector<glm::vec4> m_RandomColors;

};