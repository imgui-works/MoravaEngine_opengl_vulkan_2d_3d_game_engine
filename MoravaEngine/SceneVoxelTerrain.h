#pragma once

#include "Scene.h"

#include "TerrainVoxel.h"
#include "RenderInstanced.h"
#include "Player.h"
#include "PlayerController.h"
#include "Raycast.h"


class SceneVoxelTerrain : public Scene
{

public:
	SceneVoxelTerrain();
	virtual ~SceneVoxelTerrain() override;

	virtual void Update(float timestep, Window& mainWindow) override;
	virtual void UpdateImGui(float timestep, Window& mainWindow) override;
	virtual void Render(Window& mainWindow, glm::mat4 projectionMatrix, std::string passType,
		std::map<std::string, Shader*> shaders, std::map<std::string, GLint> uniforms) override;
	void UpdateCooldown(float timestep, Window& mainWindow);
	void Release();
	inline Raycast* GetRaycast() const { return m_Raycast; };
	std::vector<glm::vec3> GetRayIntersectPositions(float timestep, Camera* camera);
	void Dig(bool* keys, float timestep);
	void CastRay(bool* keys, bool* buttons, float timestep);
	bool IsRayIntersectPosition(glm::vec3 position);
	void OnClick(bool* keys, bool* buttons, float timestep);
	bool IsPositionVacant(glm::vec3 queryPosition);
	void AddVoxel();
	void DeleteVoxel();

private:
	virtual void SetCamera() override;
	virtual void SetupTextures() override;
	virtual void SetupTextureSlots() override;
	virtual void SetupMeshes() override;
	bool IsTerrainConfigChanged();

	TerrainVoxel* m_TerrainVoxel;
	glm::mat4 m_Transform;
	RenderInstanced* m_RenderInstanced;
	glm::i32vec3 m_TerrainScale;
	glm::i32vec3 m_TerrainScalePrev;
	float m_TerrainNoiseFactor;
	float m_TerrainNoiseFactorPrev;

	EventCooldown m_UpdateCooldown;
	EventCooldown m_DigCooldown;
	EventCooldown m_RayIntersectCooldown;
	EventCooldown m_RayCastCooldown;
	EventCooldown m_OnClickCooldown;

	Player* m_Player;
	PlayerController* m_PlayerController;
	bool m_TerrainSettingsChanged;
	Pivot* m_PivotScene;
	float m_DigDistance;
	Raycast* m_Raycast;

	// Scene settings
	bool m_DrawGizmos;
	bool m_UnlockRotation;
	bool m_UnlockRotationPrev;

	// mouse cursor intersection
	std::vector<glm::vec3> m_IntersectPositionVector;
	glm::vec3 m_IntersectPosition;
	bool m_DeleteMode; // add or delete
	int m_IntersectPositionIndex;

	glm::vec4 m_CubeColor;
	unsigned int m_DeleteVoxelCodeGLFW;

};