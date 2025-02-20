#pragma once

#include "Scene/Scene.h"

#include "Material/MaterialWorkflowPBR.h"
#include "Mesh/Grid.h"
#include "Particle/ParticleSystemCherno.h"
#include "Texture/TextureCubemapFaces.h"


class SceneParticles : public Scene
{

public:
	SceneParticles();
	virtual void Update(float timestep, Window* mainWindow) override;
	virtual void UpdateImGui(float timestep, Window* mainWindow) override;
	virtual void Render(Window* mainWindow, glm::mat4 projectionMatrix, std::string passType,
		std::map<std::string, H2M::RefH2M<MoravaShader>> shaders, std::map<std::string, int> uniforms) override;
	virtual ~SceneParticles() override;

private:
	virtual void SetSkybox() override;
	virtual void SetupTextures() override;
	virtual void SetupMeshes() override;
	virtual void SetupModels() override;
	void SetupParticles();

	MaterialWorkflowPBR* m_MaterialWorkflowPBR;

	TextureCubemapFaces* m_TextureCubeMap;

	// Particle System
	ParticleSystemCherno m_ParticleSystem;
	ParticleProps m_Particle;

	H2M::RefH2M<MoravaShader> m_ShaderFBScene;
	H2M::RefH2M<MoravaShader> m_ShaderBackground;

	float m_MouseX;
	float m_MouseY;

	float m_NormalizedMouseX;
	float m_NormalizedMouseY;

};
