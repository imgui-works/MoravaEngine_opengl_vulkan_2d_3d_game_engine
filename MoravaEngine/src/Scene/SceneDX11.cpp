#define _CRT_SECURE_NO_WARNINGS

#include "Scene/SceneDX11.h"

#include "Hazel/Scene/Components.h"
#include "Hazel/Renderer/HazelTexture.h"
#include "Hazel/Renderer/RendererAPI.h"
#include "Hazel/Scene/Entity.h"

#include "../../ImGuizmo/ImGuizmo.h"

#include "Core/Application.h"
#include "Core/Input.h"
#include "Core/Math.h"
#include "Core/Timer.h"
#include "Core/Util.h"
#include "Mesh/Block.h"
#include "Shader/MoravaShader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

#include <yaml-cpp/yaml.h>


SceneDX11::SceneDX11()
{
    sceneSettings.cameraPosition     = glm::vec3(-125.0f, -16.0f, 50.0f);
    sceneSettings.cameraStartYaw     = 0.0f;
    sceneSettings.cameraStartPitch   = 0.0f;
    sceneSettings.cameraMoveSpeed    = 2.0f;
    sceneSettings.waterHeight        = 0.0f;
    sceneSettings.waterWaveSpeed     = 0.05f;
    sceneSettings.enablePointLights  = true;
    sceneSettings.enableSpotLights   = true;
    sceneSettings.enableOmniShadows  = true;
    sceneSettings.enableSkybox       = false;
    sceneSettings.enableShadows      = true;
    sceneSettings.enableWaterEffects = false;
    sceneSettings.enableParticles    = false;

    sceneSettings.nearPlane = 0.01f;
    sceneSettings.farPlane = 2000.0f;

    SetCamera();
    SetLightManager();
    SetupShaders();
    SetupTextureSlots();
    SetupTextures();
    SetupMaterials();
    SetupFramebuffers();
    SetupMeshes();
    SetupModels();
}

SceneDX11::~SceneDX11()
{
}

void SceneDX11::SetupShaders()
{
}

void SceneDX11::SetLightManager()
{
}

void SceneDX11::SetWaterManager(int width, int height)
{
}

void SceneDX11::SetupTextures()
{
}

void SceneDX11::SetupTextureSlots()
{
}

void SceneDX11::SetupMaterials()
{
}

void SceneDX11::SetupMeshes()
{
}

void SceneDX11::SetupModels()
{
}

void SceneDX11::SetupFramebuffers()
{
}

void SceneDX11::SetupUniforms()
{
}

void SceneDX11::Update(float timestep, Window* mainWindow)
{
    Scene::Update(timestep, mainWindow);
}

void SceneDX11::UpdateImGui(float timestep, Window* mainWindow)
{
}

void SceneDX11::ShowExampleAppDockSpace(bool* p_open, Window* mainWindow)
{
}

bool SceneDX11::OnKeyPressed(KeyPressedEvent& e)
{
    return false;
}

void SceneDX11::OnEntitySelected(Hazel::Entity entity)
{
}

void SceneDX11::Render(Window* mainWindow, glm::mat4 projectionMatrix, std::string passType,
    std::map<std::string, MoravaShader*> shaders, std::map<std::string, int> uniforms)
{
}