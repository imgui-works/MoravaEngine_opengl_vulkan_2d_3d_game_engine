#include "Scene/SceneDeferredOGL.h"


SceneDeferredOGL::SceneDeferredOGL()
{
    sceneSettings.cameraPosition = glm::vec3(0.0f, 0.0f, 2.0f);
    sceneSettings.cameraStartYaw = -90.0f;
    sceneSettings.cameraStartPitch = 0.0f;
    sceneSettings.cameraMoveSpeed = 1.0f;

    // directional light
    sceneSettings.directionalLight.base.enabled = true;
    sceneSettings.directionalLight.base.ambientIntensity = 0.6f;
    sceneSettings.directionalLight.base.diffuseIntensity = 0.4f;

    ResourceManager::Init();

    SetCamera();
    SetLightManager();

    m_RenderTarget = (int)RenderTarget::Forward;
}

SceneDeferredOGL::~SceneDeferredOGL()
{
}

void SceneDeferredOGL::Update(float timestep, Window* mainWindow)
{
    Scene::Update(timestep, mainWindow);
}

void SceneDeferredOGL::UpdateImGui(float timestep, Window* mainWindow)
{
    bool p_open = true;
    ShowExampleAppDockSpace(&p_open, mainWindow);

	ImGui::Begin("Render Targets");
	{
        ImGui::Text("Forward Rendering");
		ImGui::RadioButton("Forward Rendering", &m_RenderTarget, (int)RenderTarget::Forward);

        ImGui::Separator();

        ImGui::Text("Deferred Rendering");
        ImGui::RadioButton("Deferred - Position", &m_RenderTarget, (int)RenderTarget::Deferred_Position);
        ImGui::RadioButton("Deferred - Diffuse",  &m_RenderTarget, (int)RenderTarget::Deferred_Diffuse);
        ImGui::RadioButton("Deferred - Normal",   &m_RenderTarget, (int)RenderTarget::Deferred_Normal);
        ImGui::RadioButton("Deferred - TexCoord", &m_RenderTarget, (int)RenderTarget::Deferred_TexCoord);
        ImGui::RadioButton("Deferred - SSAO",     &m_RenderTarget, (int)RenderTarget::Deferred_SSAO);
	}
	ImGui::End();
}

void SceneDeferredOGL::Render(Window* mainWindow, glm::mat4 projectionMatrix, std::string passType,
	std::map<std::string, H2M::RefH2M<MoravaShader>> shaders, std::map<std::string, int> uniforms)
{
}
