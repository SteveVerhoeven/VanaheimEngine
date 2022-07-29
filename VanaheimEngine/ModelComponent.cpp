#include "VanaheimPCH.h"
#include "ModelComponent.h"

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Graphics.h"

ModelComponent::ModelComponent()
			   : ModelComponent(nullptr)
{}
ModelComponent::ModelComponent(const std::string& path)
			   : Component()
			   , m_pMesh(nullptr)
			   , m_pMaterial(nullptr)
			   , m_FilePath(path)
			   , m_pTextures(std::vector<Texture*>())
{}
ModelComponent::ModelComponent(Mesh* pMesh)
			   : Component()
			   , m_pMesh(pMesh)
			   , m_pMaterial(nullptr)
			   , m_FilePath("")
{}

void ModelComponent::Initialize(Scene* /*pParentScene*/)
{	
	if (!m_pMesh)
		m_pMesh = Locator::GetResourceManagerService()->Load3DMesh(m_pParentObject->GetComponent<NameComponent>()->GetName(), m_FilePath);

	m_pParentObject->GetComponent<RenderComponent>()->EnableRenderComponent();
}
void ModelComponent::PostInitialize(Scene* /*pParentScene*/)
{ 
	if (!m_pMesh)
		return;

	m_pMesh->PostInitialize(m_pMaterial); 
}
void ModelComponent::Update(const float /*elapsedSec*/)
{ 
	if (!m_pMaterial)
		return; 	

}
void ModelComponent::FixedUpdate(const float /*timeEachUpdate*/)
{}
void ModelComponent::Render()
{
	if (!m_pMesh || !m_pMaterial)
		return;

	m_pMaterial->Update(m_pParentObject);
	
	RenderComponent* pRenderComponent{ m_pParentObject->GetComponent<RenderComponent>() };
	pRenderComponent->Render3DMesh(Locator::GetGraphicsService()->GetDeviceContext(), m_pMesh, m_pMaterial);
}

void ModelComponent::AddMaterial(Material* pMaterial)
{
	m_pMaterial = Locator::GetResourceManagerService()->LoadMaterial(pMaterial);
	m_pMaterial->Initialize(Locator::GetGraphicsService()->GetDevice());
}