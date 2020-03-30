#pragma once

#include "Scene.h"

#include "btBulletDynamicsCommon.h"


class SceneBullet : public Scene
{

public:
	SceneBullet();
	virtual void Update(float timestep, Window& mainWindow) override;
	virtual void Render(glm::mat4 projectionMatrix, std::string passType,
		std::map<std::string, Shader*> shaders, std::map<std::string, GLint> uniforms) override;
	virtual void SetLightManager() override;
	virtual ~SceneBullet() override;

private:
	void BulletSetup();
	void BulletSimulation(float timestep);
	void BulletCleanup();
	virtual void SetSkybox() override;
	virtual void SetTextures() override;
	virtual void SetupModels() override;
	void Fire();
	void AddBoxRigidBody(glm::vec3 position, glm::vec3 scale, float mass, float bounciness);
	btTransform GetCollisionObjectTransform(int id);

private:
	btDiscreteDynamicsWorld* dynamicsWorld;
	btAlignedObjectArray<btCollisionShape*> m_CollisionShapes;
	btSequentialImpulseConstraintSolver* solver;
	btBroadphaseInterface* overlappingPairCache;
	btCollisionDispatcher* dispatcher;
	btDefaultCollisionConfiguration* collisionConfiguration;

	int m_SphereCount = 0;
	int m_SphereCountMax = 100;
	int m_GravityIntensity = -1;

	float m_LastTimestep = 0.0f;
	float m_FireCooldown = 0.2f;
	int m_SpheresOffset = 0;

	float m_Bounciness = 0.6f;
	float m_FireIntensity = 50.0f;
	bool m_FireEnabled = true;

	btRigidBody* m_LatestBulletBody;
	float m_TextureMultiplier = 4.0f;

};