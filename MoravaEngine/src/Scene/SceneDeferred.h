#pragma once

#include "Scene/Scene.h"

#include "../../LearnOpenGL/ModelSSAO.h"
#include "../../QuadSSAO.h"

#include "Framebuffer/Framebuffer.h"
#include "Mesh/Cube.h"


class SceneDeferred : public Scene
{

public:
	SceneDeferred();
	virtual ~SceneDeferred() override;

	virtual void Update(float timestep, Window* mainWindow) override;
	virtual void Render(Window* mainWindow, glm::mat4 projectionMatrix, std::string passType,
		std::map<std::string, Shader*> shaders, std::map<std::string, int> uniforms) override;

private:
	virtual void UpdateImGui(float timestep, Window* mainWindow) override;
	virtual void SetupMeshes() override;
	virtual void SetupModels() override;
	virtual void SetupFramebuffers() override;

	void SetupShaders();
	void SetupLights();

	void RenderPassGeometry(glm::mat4 projectionMatrix);
	void RenderPassLighting();
	void RenderPassForward(glm::mat4 projectionMatrix);

	// Managing screen resize
	void UpdateCooldown(float timestep);
	void ResetHandlers();
	void GenerateFramebuffers(int width, int height);

private:
	int m_Width;
	int m_Height;

	int m_WidthPrev;
	int m_HeightPrev;

	Shader* m_ShaderGeometryPass;
	Shader* m_ShaderLightingPass;
	Shader* m_ShaderLightBox;

	ModelSSAO* m_Backpack;
	std::vector<glm::vec3> m_ObjectPositions;

	unsigned int gBuffer;
	unsigned int gPosition;
	unsigned int gNormal;
	unsigned int gAlbedoSpec;
	std::vector<unsigned int> attachments;
	unsigned int rboDepth;

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> m_LightPositions;
	std::vector<glm::vec3> m_LightColors;

	QuadSSAO* m_Quad;
	Cube* m_Cube;

	EventCooldown m_UpdateCooldown;

};