#pragma once

#include "Scene/Scene.h"

#include "Core/Raycast.h"
#include "Instancing/RenderInstanced.h"
#include "Player/Player.h"
#include "Player/PlayerController.h"
#include "Terrain/TerrainVoxel.h"


class SceneVoxelTerrain : public Scene
{

public:
	SceneVoxelTerrain();
	virtual ~SceneVoxelTerrain() override;

	virtual void Update(float timestep, Window* mainWindow) override;
	virtual void UpdateImGui(float timestep, Window* mainWindow) override;
	virtual void Render(Window* mainWindow, glm::mat4 projectionMatrix, std::string passType,
		std::map<std::string, H2M::RefH2M<MoravaShader>> shaders, std::map<std::string, int> uniforms) override;
	void UpdateCooldown(float timestep, Window* mainWindow);
	void Release();
	inline Raycast* GetRaycast() const { return m_Raycast; };
	std::vector<glm::vec3> GetRayIntersectPositions(float timestep, Camera* camera);
	void Dig(bool* keys, float timestep);
	void CastRay(bool* keys, bool* buttons, float timestep);
	bool IsRayIntersectPosition(glm::vec3 position);
	void OnClick(bool* keys, bool* buttons, float timestep);
	bool IsPositionVacant(glm::ivec3 queryPosition);
	void AddVoxel();
	void DeleteVoxel();

private:
	virtual void SetCamera() override;
	virtual void SetupTextures() override;
	virtual void SetupTextureSlots() override;
	virtual void SetupMeshes() override;
	bool IsTerrainConfigChanged();

	TerrainVoxel* m_TerrainVoxel;
	RenderInstanced* m_RenderInstanced;
	glm::ivec3 m_TerrainScale;
	glm::ivec3 m_TerrainScalePrev;
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
	bool m_DeleteMode; // add or delete

	glm::ivec3 m_IntersectPosition;

	glm::vec4 m_CubeColor;
	unsigned int m_DeleteVoxelCodeGLFW;

};
