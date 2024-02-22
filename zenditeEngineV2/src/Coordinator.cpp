#include "Coordinator.h"
#include "ECS/Components.h"
#include "Camera.h"
#include "../API_Rendering/OpenGl_Renderer.h"

Coordinator::Coordinator(std::string API_Type, std::string Render_Type, std::shared_ptr<Camera> camera)
{
	m_ECSCoord = std::make_shared<ECSCoordinator>();

	if (API_Type == "opengl")
	{
		m_APImanager = std::make_shared<OpenGL_Manager>();
	}
	else
	{
		//default:
		m_APImanager = std::make_shared<OpenGL_Manager>();
	}

	if(Render_Type == "opengl")
	{		
		m_Renderer = std::make_shared<OpenGL_Renderer>(camera);
	}
	else
	{
		m_Renderer = std::make_shared<OpenGL_Renderer>(camera);
	}
}

void Coordinator::RegisterComponents()
{
	m_ECSCoord->RegisterComponent<c_Transform>();
	m_ECSCoord->RegisterComponent<c_RenderableComponent>();
	m_ECSCoord->RegisterComponent<c_Texture>();
	m_ECSCoord->RegisterComponent<c_AABB>();
	m_ECSCoord->RegisterComponent<c_Modified>();
	m_ECSCoord->RegisterComponent<c_PointLightEmitter>();
	m_ECSCoord->RegisterComponent<c_SpotLightEmitter>();
	m_ECSCoord->RegisterComponent<c_LightResponderMesh>();

}

void Coordinator::RegisterSystems() //And add them to the system manager list
{
	m_RenderableSystem = std::static_pointer_cast<RenderableSystem>(m_ECSCoord->RegisterSystem<RenderableSystem>());

	m_SetupPointLightSystem = std::static_pointer_cast<SetupPointLightSystem>(m_ECSCoord->RegisterSystem<SetupPointLightSystem>());
	m_SetupSpotLightSystem = std::static_pointer_cast<SetupSpotLightSystem>(m_ECSCoord->RegisterSystem<SetupSpotLightSystem>());
	m_RenderLightingSystem = std::static_pointer_cast<RenderLightingSystem>(m_ECSCoord->RegisterSystem<RenderLightingSystem>());
	m_CollisionDetectionAABBSystem = std::static_pointer_cast<CollisionDetectionAABBSystem>(m_ECSCoord->RegisterSystem<CollisionDetectionAABBSystem>());
	m_RenderAABBSystem = std::static_pointer_cast<RenderAABBSystem>(m_ECSCoord->RegisterSystem<RenderAABBSystem>());
}

void Coordinator::SetUpSystemBitsets()
{

	Signature RenerableSysSig;
	RenerableSysSig.set(m_ECSCoord->GetComponentBitsetPos<c_Transform>());
	RenerableSysSig.set(m_ECSCoord->GetComponentBitsetPos<c_RenderableComponent>());
	RenerableSysSig.set(m_ECSCoord->GetComponentBitsetPos<c_Modified>());
	m_ECSCoord->SetSystemBitsetSignature<RenderableSystem>(RenerableSysSig);

	Signature CollisionDetectionAABBSystemSig;
	CollisionDetectionAABBSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Transform>());
	CollisionDetectionAABBSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_AABB>());
	CollisionDetectionAABBSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Modified>());
	m_ECSCoord->SetSystemBitsetSignature<CollisionDetectionAABBSystem>(CollisionDetectionAABBSystemSig);

	Signature RenderAABBSystemSig;
	RenderAABBSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Transform>());
	RenderAABBSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_AABB>());
	RenderAABBSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Modified>());
	m_ECSCoord->SetSystemBitsetSignature<RenderAABBSystem>(RenderAABBSystemSig);

	Signature RenderLightingSystemSig;
	RenderLightingSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Transform>());
	RenderLightingSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_LightResponderMesh>());
	RenderLightingSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Modified>());
	m_ECSCoord->SetSystemBitsetSignature<RenderLightingSystem>(RenderLightingSystemSig);

	Signature SetupPointLightSystemSig;
	SetupPointLightSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Transform>());
	SetupPointLightSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_PointLightEmitter>());
	SetupPointLightSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Modified>());
	m_ECSCoord->SetSystemBitsetSignature<SetupPointLightSystem>(SetupPointLightSystemSig);

	Signature SetupSpotLightSystemSig;
	SetupPointLightSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Transform>());
	SetupPointLightSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_SpotLightEmitter>());
	SetupPointLightSystemSig.set(m_ECSCoord->GetComponentBitsetPos<c_Modified>());
	m_ECSCoord->SetSystemBitsetSignature<SetupSpotLightSystem>(SetupSpotLightSystemSig);
}

Signature Coordinator::GetEntitySignature(Entity EID)
{
	return m_ECSCoord->GetEntitySignature(EID);
}

void Coordinator::SetUpRenderData(Entity EID)
{
	// if ((entitySig & sysSig) == sysSig)

	if ((m_ECSCoord->GetEntitySignature(EID) & m_ECSCoord->GetSystemBitsetSignature<RenderableSystem>()) == m_ECSCoord->GetSystemBitsetSignature<RenderableSystem>())
	{
		m_APImanager->SetupRenderData(EID, m_ECSCoord);
	}
	else
	{
		std::cout << "\nNOTIFY: Attempting to Set Up Render Data on EID: " << EID << " Which is without a renderable component" << std::endl;
	}
}

Entity Coordinator::CreateEntity()
{
	return m_ECSCoord->CreateEntity();
}

void Coordinator::setShaderForEntity(Entity EID, std::shared_ptr<Shader> shader)
{
	if ((m_ECSCoord->GetEntitySignature(EID) & m_ECSCoord->GetSystemBitsetSignature<RenderableSystem>()) == m_ECSCoord->GetSystemBitsetSignature<RenderableSystem>())
	{
		m_APImanager->SetShaderForEntity(EID, shader);
	}
	else
	{
		std::cout << "NOTIFY: Attempting to set shader for EID: " << EID << " which is without a renderable component" << std::endl;
	}
}

void Coordinator::StoreShaderInEntityDataHandle(Entity EID)
{
	if ((m_ECSCoord->GetEntitySignature(EID) & m_ECSCoord->GetSystemBitsetSignature<RenderableSystem>()) == m_ECSCoord->GetSystemBitsetSignature<RenderableSystem>())
	{
		m_APImanager->SetShaderForDataHandle(EID);
	}
	else
	{
		std::cout << "NOTIFY: Attempting to set shader for EID: " << EID << " which is without a renderable component" << std::endl;
	}
}

unsigned short int Coordinator::GenerateTexUnit(std::string texFilePath, std::string fileType)
{
	return m_APImanager->GenerateTexUnit(texFilePath, fileType);
}

void Coordinator::runAllSystems(float deltaTime, std::vector<Entity>* entities)
{
	m_RenderableSystem->Render(m_Renderer, m_APImanager, m_ECSCoord);
	m_SetupPointLightSystem->Setup(m_APImanager, m_ECSCoord);
	m_SetupSpotLightSystem->Setup(m_APImanager, m_ECSCoord);
	m_RenderLightingSystem->Render(m_Renderer, m_APImanager, m_ECSCoord);
	m_CollisionDetectionAABBSystem->checkCollisions(m_APImanager, m_ECSCoord);
	m_RenderAABBSystem->RenderAABBs(m_Renderer, m_APImanager, m_ECSCoord);

	for (auto const& EID : *entities)
	{
		m_ECSCoord->GetComponentDataFromEntity<c_Modified>(EID).isModifed = false;
	}
	
}


