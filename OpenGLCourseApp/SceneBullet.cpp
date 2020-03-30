#include "SceneBullet.h"

#include "Sphere.h"
#include "SphereJoey.h"
#include "Block.h"

#include "ImGuiWrapper.h"


SceneBullet::SceneBullet()
{
	sceneSettings.enableShadows      = true;
	sceneSettings.enableOmniShadows  = false;
	sceneSettings.enablePointLights  = true;
	sceneSettings.enableSpotLights   = false;
	sceneSettings.enableWaterEffects = false;
	sceneSettings.enableSkybox       = true;
	sceneSettings.enableNormalMaps   = true;
	sceneSettings.cameraPosition = glm::vec3(0.0f, 10.0f, 40.0f);
	sceneSettings.cameraStartYaw = -90.0f;
	sceneSettings.cameraMoveSpeed = 5.0f;
	sceneSettings.nearPlane = 0.01f;
	sceneSettings.farPlane = 400.0f;
	sceneSettings.ambientIntensity = 0.2f;
	sceneSettings.diffuseIntensity = 0.4f;
	sceneSettings.lightDirection = glm::vec3(0.05f, -0.9f, 0.05f);
	sceneSettings.lightProjectionMatrix = glm::ortho(-60.0f, 60.0f, -60.0f, 60.0f, 0.1f, 60.0f);
	sceneSettings.pLight_0_color = glm::vec3(0.8f, 0.8f, 0.6f);
	sceneSettings.pLight_0_position = glm::vec3(0.0f, 20.0f, 0.0f);
	sceneSettings.pLight_0_diffuseIntensity = 2.0f;
	sceneSettings.pLight_1_color = glm::vec3(1.0f, 1.0f, 1.0f);
	sceneSettings.pLight_1_position = glm::vec3(8.92f, 2.75f, -0.85f);
	sceneSettings.pLight_1_diffuseIntensity = 2.0f;
	sceneSettings.pLight_2_color = glm::vec3(0.0f, 0.0f, 1.0f);
	sceneSettings.pLight_2_position = glm::vec3(10.0f, 2.0f, 10.0f);
	sceneSettings.pLight_2_diffuseIntensity = 2.0f;
	sceneSettings.shadowMapWidth = 2048;
	sceneSettings.shadowMapHeight = 2048;
	sceneSettings.shadowSpeed = 0.1f;
	sceneSettings.waterHeight = 6.0f; // 1.0f 5.0f
	sceneSettings.waterWaveSpeed = 0.1f;

	BulletSetup();
	SetLightManager();
	SetSkybox();
	SetTextures();
	SetupModels();
}

void SceneBullet::SetLightManager()
{
	Scene::SetLightManager();

	m_LightManager->pointLights[0].SetAmbientIntensity(2.0f);
	m_LightManager->pointLights[0].SetDiffuseIntensity(1.0f);
}

void SceneBullet::SetSkybox()
{
	skyboxFaces.push_back("Textures/skybox_4/right.png");
	skyboxFaces.push_back("Textures/skybox_4/left.png");
	skyboxFaces.push_back("Textures/skybox_4/top.png");
	skyboxFaces.push_back("Textures/skybox_4/bottom.png");
	skyboxFaces.push_back("Textures/skybox_4/back.png");
	skyboxFaces.push_back("Textures/skybox_4/front.png");
	m_Skybox = new Skybox(skyboxFaces);
}

void SceneBullet::SetTextures()
{
	textures.insert(std::make_pair("pyramid", new Texture("Textures/pyramid.png")));
	textures.insert(std::make_pair("texture_gray", new Texture("Textures/texture_gray.png")));
	textures.insert(std::make_pair("texture_orange", new Texture("Textures/texture_orange.png")));
	textures.insert(std::make_pair("texture_blue", new Texture("Textures/texture_blue.png")));
	textures.insert(std::make_pair("crate_diffuse", new Texture("Textures/crate.png")));
	textures.insert(std::make_pair("crate_normal", new Texture("Textures/crateNormal.png")));
	textures.insert(std::make_pair("rusted_iron_diffuse", new Texture("Textures/PBR/rusted_iron/albedo.png")));
	textures.insert(std::make_pair("rusted_iron_normal", new Texture("Textures/PBR/rusted_iron/normal.png")));
	textures.insert(std::make_pair("texture_chess", new Texture("Textures/texture_chess.png", false, GL_NEAREST)));
	textures.insert(std::make_pair("texture_checker", new Texture("Textures/texture_checker.png", false, GL_NEAREST)));
	textures.insert(std::make_pair("texture_wall_albedo", new Texture("Textures/PBR/wall/albedo.png", false, GL_LINEAR)));
	textures.insert(std::make_pair("texture_wall_normal", new Texture("Textures/PBR/wall/normal.png", false, GL_LINEAR)));
	

	
}

void SceneBullet::SetupModels()
{
	Block* block_floor = new Block(100.0f, 4.0f, 100.0f, m_TextureMultiplier);
	meshes.insert(std::make_pair("block_floor", block_floor));

	Block* block_wall_1 = new Block(100.0f, 20.0f, 4.0f, m_TextureMultiplier);
	meshes.insert(std::make_pair("block_wall_1", block_wall_1));

	Block* block_wall_2 = new Block(4.0f, 20.0f, 100.0f, m_TextureMultiplier);
	meshes.insert(std::make_pair("block_wall_2", block_wall_2));

	Sphere* sphere = new Sphere();
	sphere->Create();
	meshes.insert(std::make_pair("sphere", sphere));
}

void SceneBullet::BulletSetup()
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, btScalar(m_GravityIntensity), 0));

	// Floor
	AddBoxRigidBody(glm::vec3(0.0f, 0.0f, 0.0f),     glm::vec3(50.0f, 2.0f, 50.0f), 0.0f, m_Bounciness);
	// Wall 1
	AddBoxRigidBody(glm::vec3(0.0f, 10.0f, -50.0f),  glm::vec3(50.0f, 10.0f, 2.0f), 0.0f, m_Bounciness);
	// Wall 2
	AddBoxRigidBody(glm::vec3(0.0f, 10.0f, 50.0f),   glm::vec3(50.0f, 10.0f, 2.0f), 0.0f, m_Bounciness);
	// Wall 3
	AddBoxRigidBody(glm::vec3(-50.0f, 10.0f, 0.0f),  glm::vec3(2.0f, 10.0f, 50.0f), 0.0f, m_Bounciness);
	// Wall 4
	AddBoxRigidBody(glm::vec3(50.0f, 10.0f, 0.0f),   glm::vec3(2.0f, 10.0f, 50.0f), 0.0f, m_Bounciness);
	// Cube 1
	AddBoxRigidBody(glm::vec3(10.0f, 3.0f, 10.0f),   glm::vec3(3.0f), 20.0f, 0.2f);
	// Cube 2
	AddBoxRigidBody(glm::vec3(-10.0f, 8.0f, -10.0f), glm::vec3(4.0f), 40.0f, 0.2f);

	m_SpheresOffset += 7;

	printf("Bullet Setup complete.\n");
}

void SceneBullet::AddBoxRigidBody(glm::vec3 position, glm::vec3 scale, float mass, float bounciness)
{
	btTransform shapeTransform;
	shapeTransform.setIdentity();
	shapeTransform.setOrigin(btVector3(position.x, position.y, position.z));

	btCollisionShape* collisionShape = new btBoxShape(btVector3(btScalar(scale.x), btScalar(scale.y), btScalar(scale.z)));
	m_CollisionShapes.push_back(collisionShape);

	btScalar bodyMass = btScalar(mass);
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia = btVector3(0, 0, 0);
	if (isDynamic)
		collisionShape->calculateLocalInertia(mass, localInertia);
	//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(shapeTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfoF(mass, myMotionState, collisionShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfoF);
	body->setRestitution(bounciness);
	//add the body to the dynamics world
	dynamicsWorld->addRigidBody(body);
}

void SceneBullet::BulletSimulation(float timestep)
{
	dynamicsWorld->stepSimulation(timestep * 0.005f, 10);
}

void SceneBullet::Fire()
{
	if (!m_FireEnabled) return;
	if (m_SphereCount >= m_SphereCountMax) return;

	// Sphere 1 create a dynamic rigidbody
	btCollisionShape* colShape = new btSphereShape(btScalar(1.5));
	m_CollisionShapes.push_back(colShape);
	/// Create Dynamic Objects
	btTransform startTransform;
	startTransform.setIdentity();
	btScalar mass(2.0f);
	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (mass != 0.f);
	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		colShape->calculateLocalInertia(mass, localInertia);
	glm::vec3 origin = m_Camera->GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f) + m_Camera->GetFront() * 2.0f;
	startTransform.setOrigin(btVector3(origin.x, origin.y, origin.z));
	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
	m_LatestBulletBody = new btRigidBody(rbInfo);
	m_LatestBulletBody->setRestitution(m_Bounciness);
	dynamicsWorld->addRigidBody(m_LatestBulletBody);
	m_SphereCount++;

	// apply the force
	glm::vec3 fireImpulse = m_Camera->GetDirection() * m_FireIntensity;
	m_LatestBulletBody->applyCentralImpulse(btVector3(fireImpulse.x, fireImpulse.y, fireImpulse.z));
	// printf("\rSceneBullet::Fire: BOOOM! m_SphereCount: %i", m_SphereCount);
}

void SceneBullet::Update(float timestep, Window& mainWindow)
{
	if (mainWindow.getMouseButtons()[GLFW_MOUSE_BUTTON_LEFT])
	{
		if (timestep - m_LastTimestep > m_FireCooldown)
		{
			Fire();
			m_LastTimestep = timestep;
		}
	}

	glm::vec3 lightDirection = m_LightManager->directionalLight.GetDirection();

	if (m_LatestBulletBody != nullptr)
	{
		btTransform bulletTransform;
		m_LatestBulletBody->getMotionState()->getWorldTransform(bulletTransform);
		glm::vec3 bulletPosition = glm::vec3(
			bulletTransform.getOrigin().getX(),
			bulletTransform.getOrigin().getY(),
			bulletTransform.getOrigin().getZ()
		);
		m_LightManager->pointLights[0].SetPosition(bulletPosition);
	}

	// Point light for sphere bullet
	glm::vec3 PL0_Position  = m_LightManager->pointLights[0].GetPosition();
	glm::vec3 PL0_Color     = m_LightManager->pointLights[0].GetColor();
	float PL0_AmbIntensity  = m_LightManager->pointLights[0].GetAmbientIntensity();
	float PL0_DiffIntensity = m_LightManager->pointLights[0].GetDiffuseIntensity();

	ImGui::SliderFloat3("DirLight Direction", glm::value_ptr(lightDirection), -1.0f, 1.0f);
	ImGui::ColorEdit3("PL0 Color",            glm::value_ptr(PL0_Color));
	ImGui::SliderFloat3("PL0 Position",       glm::value_ptr(PL0_Position), -20.0f, 20.0f);
	ImGui::SliderFloat("PL0 Amb Intensity",   &PL0_AmbIntensity, -20.0f, 20.0f);
	ImGui::SliderFloat("PL0 Diff Intensity",  &PL0_DiffIntensity, -20.0f, 20.0f);
	ImGui::SliderInt("Gravity Intensity",     &m_GravityIntensity, -10, 10);
	ImGui::SliderFloat("Bouncincess",         &m_Bounciness, 0.0f, 2.0f);
	ImGui::Checkbox("Fire Enabled",           &m_FireEnabled);
	ImGui::SliderFloat("Fire Intensity",      &m_FireIntensity, 0.0f, 100.0f);
	std::string bulletsText = "Bullets: " + std::to_string(m_SphereCount) + "/" + std::to_string(m_SphereCountMax);
	ImGui::Text(bulletsText.c_str());

	m_LightManager->directionalLight.SetDirection(lightDirection);

	m_LightManager->pointLights[0].SetColor(PL0_Color);
	m_LightManager->pointLights[0].SetAmbientIntensity(PL0_AmbIntensity);
	m_LightManager->pointLights[0].SetDiffuseIntensity(PL0_DiffIntensity);

	dynamicsWorld->setGravity(btVector3(0, btScalar(m_GravityIntensity), 0));

	BulletSimulation(timestep);
}

void SceneBullet::Render(glm::mat4 projectionMatrix, std::string passType,
	std::map<std::string, Shader*> shaders, std::map<std::string, GLint> uniforms)
{
	glm::mat4 model;
	btTransform sphereTrans;
	for (int i = 0; i < m_SphereCount; i++)
	{
		/* Sphere bullet */
		sphereTrans = GetCollisionObjectTransform(i + m_SpheresOffset);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(
			float(sphereTrans.getOrigin().getX()),
			float(sphereTrans.getOrigin().getY()),
			float(sphereTrans.getOrigin().getZ())
		));
		model = glm::rotate(model, sphereTrans.getRotation().getX(), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, sphereTrans.getRotation().getY(), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, sphereTrans.getRotation().getZ(), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(1.5f));
		glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
		textures["rusted_iron_diffuse"]->Bind(textureSlots["diffuse"]);
		textures["rusted_iron_normal"]->Bind(textureSlots["normal"]);
		materials["superShiny"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
		meshes["sphere"]->Render();
	}

	btTransform cubeTrans;

	/* Cube 1 */
	cubeTrans = GetCollisionObjectTransform(5);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(
		float(cubeTrans.getOrigin().getX()),
		float(cubeTrans.getOrigin().getY()),
		float(cubeTrans.getOrigin().getZ())
	));
	model = glm::rotate(model, cubeTrans.getRotation().getX(), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, cubeTrans.getRotation().getY(), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, cubeTrans.getRotation().getZ(), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(6.0f, 6.0f, 6.0f));
	glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
	textures["crate_diffuse"]->Bind(textureSlots["diffuse"]);
	textures["crate_normal"]->Bind(textureSlots["normal"]);
	materials["superShiny"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
	meshes["cube"]->Render();

	/* Cube 2 */
	cubeTrans = GetCollisionObjectTransform(6);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(
		float(cubeTrans.getOrigin().getX()),
		float(cubeTrans.getOrigin().getY()),
		float(cubeTrans.getOrigin().getZ())
	));
	model = glm::rotate(model, cubeTrans.getRotation().getX(), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, cubeTrans.getRotation().getY(), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, cubeTrans.getRotation().getZ(), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::scale(model, glm::vec3(8.0f, 8.0f, 8.0f));
	glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
	textures["crate_diffuse"]->Bind(textureSlots["diffuse"]);
	textures["crate_normal"]->Bind(textureSlots["normal"]);
	materials["superShiny"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
	meshes["cube"]->Render();

	if (passType == "main")
	{
		/* Floor */
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f));
		glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
		textures["texture_chess"]->Bind(textureSlots["diffuse"]);
		textures["normalMapDefault"]->Bind(textureSlots["normal"]);
		materials["dull"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
		meshes["block_floor"]->Render();

		/* Wall 1 */
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, -50.0f));
		glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
		textures["texture_chess"]->Bind(textureSlots["diffuse"]);
		textures["normalMapDefault"]->Bind(textureSlots["normal"]);
		materials["dull"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
		meshes["block_wall_1"]->Render();

		/* Wall 2 */
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 10.0f, 50.0f));
		glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
		textures["texture_chess"]->Bind(textureSlots["diffuse"]);
		textures["normalMapDefault"]->Bind(textureSlots["normal"]);
		materials["dull"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
		meshes["block_wall_1"]->Render();

		/* Wall 3 */
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-50.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
		textures["texture_chess"]->Bind(textureSlots["diffuse"]);
		textures["normalMapDefault"]->Bind(textureSlots["normal"]);
		materials["dull"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
		meshes["block_wall_2"]->Render();

		/* Wall 4 */
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(50.0f, 10.0f, 0.0f));
		glUniformMatrix4fv(uniforms["model"], 1, GL_FALSE, glm::value_ptr(model));
		textures["texture_chess"]->Bind(textureSlots["diffuse"]);
		textures["normalMapDefault"]->Bind(textureSlots["normal"]);
		materials["dull"]->UseMaterial(uniforms["specularIntensity"], uniforms["shininess"]);
		meshes["block_wall_2"]->Render();
	}
}

btTransform SceneBullet::GetCollisionObjectTransform(int id)
{
	btTransform transform;
	btCollisionObject* collisionObject = dynamicsWorld->getCollisionObjectArray()[id];
	btRigidBody* rigidBody = btRigidBody::upcast(collisionObject);
	if (rigidBody && rigidBody->getMotionState())
		rigidBody->getMotionState()->getWorldTransform(transform);
	else
		transform = rigidBody->getWorldTransform();
	return transform;
}

void SceneBullet::BulletCleanup()
{
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < m_CollisionShapes.size(); j++)
	{
		btCollisionShape* shape = m_CollisionShapes[j];
		m_CollisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	m_CollisionShapes.clear();

	printf("Bullet cleanup complete.\n");
}

SceneBullet::~SceneBullet()
{
	BulletCleanup();
}