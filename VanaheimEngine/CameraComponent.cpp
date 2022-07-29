#include "VanaheimPCH.h"
#include "CameraComponent.h"

#include "GameObject.h"
#include "Window.h"
#include "Scene.h"

CameraComponent::CameraComponent()
				: Component()
				, m_IsSceneCamera(false)
				// Dirty Flag pattern
				// Reference: https://gameprogrammingpatterns.com/dirty-flag.html
				, m_UpdateFlags(Update_Flags::NO_UPDATE)
				// Camera
				, m_Near(0.1f)
				, m_Far(5000.f)
				, m_FOV(DirectX::XM_PIDIV4)	// 45 degrees
				, m_View()
				, m_Projection()
				, m_ViewProjection()
				, m_ViewInverse()
				, m_ViewProjectionInverse()
{
	DirectX::XMStoreFloat4x4(&m_View, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_Projection, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ViewProjection, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ViewInverse, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ViewProjectionInverse, DirectX::XMMatrixIdentity());

	m_UpdateFlags |= Update_Flags::VIEW;
	m_UpdateFlags |= Update_Flags::PROJECTION;
	m_UpdateFlags |= Update_Flags::VIEWINVERSE;
	m_UpdateFlags |= Update_Flags::VIEWPROJECTION;
	m_UpdateFlags |= Update_Flags::VIEWPROJECTIONINVERSE;
}

void CameraComponent::Initialize(Scene* /*pParentScene*/)
{}
void CameraComponent::PostInitialize(Scene* /*pParentScene*/)
{}
void CameraComponent::Update(const float /*elapsedSec*/)
{}
void CameraComponent::FixedUpdate(const float /*timeEachUpdate*/)
{}

const DirectX::XMFLOAT4X4& CameraComponent::GetView()
{
	// To be able to do XMVECTOR + XMVECTOR
	// Reference: https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	if (HasFlag(Update_Flags::VIEW))
	{
		DirectX::XMMATRIX view{};
		{
			TransformComponent* pTransformComponent{ m_pParentObject->GetComponent<TransformComponent>() };

			// ********************************
			// Calculate the VIEW matrix
			// ********************************
			const DirectX::XMFLOAT3 worldPosition{ pTransformComponent->GetPosition() };
			const DirectX::XMFLOAT3 forward{ pTransformComponent->GetForward() };
			const DirectX::XMFLOAT3 up{ pTransformComponent->GetUp() };

			/* XMMatrixLookAtLH - Parameters */
			DirectX::XMVECTOR EyePosition{ DirectX::XMLoadFloat3(&worldPosition) };
			DirectX::XMVECTOR FocusPosition{ DirectX::XMLoadFloat3(&worldPosition) + DirectX::XMLoadFloat3(&forward) };
			DirectX::XMVECTOR UpDirection{ DirectX::XMLoadFloat3(&up) };

			// Explanation for all parameters in link below
			// Reference: https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixlookatlh
			view = DirectX::XMMatrixLookAtLH(EyePosition, FocusPosition, UpDirection);
		}
		XMStoreFloat4x4(&m_View, view);

		RemoveFlag(Update_Flags::VIEW);
	}

	return m_View;
}
const DirectX::XMFLOAT4X4& CameraComponent::GetProjection()
{
	if (HasFlag(Update_Flags::PROJECTION))
	{
		DirectX::XMMATRIX projection{};
		{
			// ********************************
			// Calculate the PROJECTION matrix
			// ********************************
			/* XMMatrixLookAtLH - Parameters */
			float FovAngleY{ m_FOV };
			float AspectRatio{ Locator::GetWindowService()->GetAscpectRatio() };
			float NearZ{ m_Near };
			float FarZ{ m_Far };

			// Explanation for all parameters in link below
			// Reference: https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixperspectivefovlh
			projection = DirectX::XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ);
		}
		XMStoreFloat4x4(&m_Projection, projection);

		RemoveFlag(Update_Flags::PROJECTION);
	}

	return m_Projection;
}
const DirectX::XMFLOAT4X4& CameraComponent::GetViewProjection()
{
	// To be able to do XMMATRIX * XMMATRIX
	// Reference: https://stackoverflow.com/questions/21688529/binary-directxxmvector-does-not-define-this-operator-or-a-conversion
	using namespace DirectX;

	if (HasFlag(Update_Flags::VIEWPROJECTION))
	{
		DirectX::XMMATRIX viewProjection{};
		{
			const DirectX::XMFLOAT4X4 view{ GetView() };
			const DirectX::XMFLOAT4X4 projection{ GetProjection() };

			const DirectX::XMMATRIX matrix_view{ DirectX::XMLoadFloat4x4(&view) };
			const DirectX::XMMATRIX matrix_projection{ DirectX::XMLoadFloat4x4(&projection) };
			viewProjection = matrix_view * matrix_projection;
		}
		XMStoreFloat4x4(&m_ViewProjection, viewProjection);

		RemoveFlag(Update_Flags::VIEWPROJECTION);
	}

	return m_ViewProjection;
}
const DirectX::XMFLOAT4X4& CameraComponent::GetViewInverse()
{
	if (HasFlag(Update_Flags::VIEWINVERSE))
	{
		DirectX::XMMATRIX viewInverse{};
		{
			// ********************************
			// Calculate the VIEW_INVERSE matrix
			// ********************************
			const DirectX::XMFLOAT4X4 view{ GetView() };
			const DirectX::XMMATRIX matrix_view{ DirectX::XMLoadFloat4x4(&view) };

			/* XMMatrixInverse - Parameters */
			DirectX::XMVECTOR* pDeterminant{ nullptr };
			DirectX::FXMMATRIX M{ matrix_view };

			// Explanation for all parameters in link below
			// Reference: https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixinverse
			viewInverse = DirectX::XMMatrixInverse(pDeterminant, M);
		}
		XMStoreFloat4x4(&m_ViewInverse, viewInverse);

		RemoveFlag(Update_Flags::VIEWINVERSE);
	}

	return m_ViewInverse;
}
const DirectX::XMFLOAT4X4& CameraComponent::GetViewProjectionInverse()
{
	if (HasFlag(Update_Flags::VIEWPROJECTIONINVERSE))
	{
		DirectX::XMMATRIX viewProjectionInverse{};
		{	
			// ********************************
			// Calculate the VIEW_PROJECTION_INVERSE matrix
			// ********************************
			const DirectX::XMFLOAT4X4 view{ GetView() };
			const DirectX::XMFLOAT4X4 projection{ GetProjection() };

			const DirectX::XMMATRIX matrix_view{ DirectX::XMLoadFloat4x4(&view) };
			const DirectX::XMMATRIX matrix_projection{ DirectX::XMLoadFloat4x4(&projection) };
			const DirectX::XMMATRIX matrix_viewProjection{ matrix_view * matrix_projection };

			/* XMMatrixInverse - Parameters */
			DirectX::XMVECTOR* pDeterminant{ nullptr };
			DirectX::FXMMATRIX M{ matrix_viewProjection };

			// Explanation for all parameters in link below
			// Reference: https://docs.microsoft.com/en-us/windows/win32/api/directxmath/nf-directxmath-xmmatrixinverse
			viewProjectionInverse = DirectX::XMMatrixInverse(pDeterminant, M);
		}
		XMStoreFloat4x4(&m_ViewProjectionInverse, viewProjectionInverse);

		m_UpdateFlags &= ~Update_Flags::VIEWPROJECTIONINVERSE;
	}

	return m_ViewProjectionInverse;
}

void CameraComponent::SetIsMainCamera(const bool isMainCamera)
{
	m_IsSceneCamera = isMainCamera;

	if (isMainCamera)
	{
		Locator::ProvideRenderCameraService(this);
		if (m_pParentObject && m_pParentObject->GetParentScene())
		{
			GameObject* pOldCameraObject{ m_pParentObject->GetParentScene()->GetSceneCamera() };
			m_pParentObject->GetParentScene()->SetSceneCamera(m_pParentObject);
			pOldCameraObject->GetComponent<CameraComponent>()->SetIsMainCamera(false);
		}
	}
	else
	{
		if (m_pParentObject == Locator::GetSceneCameraService()->GetParentObject())
		{
			m_IsSceneCamera = true;
		}
	}
}

void CameraComponent::SetUpdateFlags(const Update_Flags& flags)
{ m_UpdateFlags |= flags; }
bool CameraComponent::HasFlag(const Update_Flags& flag)
{ return (m_UpdateFlags & flag); }
void CameraComponent::RemoveFlag(const Update_Flags & flag)
{ m_UpdateFlags &= ~flag; }

//void CameraComponent::Serialize(YAML::Emitter& out)
//{
//	out << YAML::Key << "CameraComponent";
//	out << YAML::BeginMap;
//	out << YAML::Key << "IsMainCamera" << YAML::Value << m_IsMainCamera;
//
//	out << YAML::Key << "Near" << YAML::Value << m_Near;
//	out << YAML::Key << "Far" << YAML::Value << m_Far;
//	out << YAML::Key << "FOV" << YAML::Value << m_FOV;
//
//	out << YAML::Key << "View" << YAML::Value << m_View;
//	out << YAML::Key << "Projection" << YAML::Value << m_Projection;
//	out << YAML::Key << "ViewProjection" << YAML::Value << m_ViewProjection;
//	out << YAML::Key << "ViewInverse" << YAML::Value << m_ViewInverse;
//	out << YAML::Key << "ViewProjectionInverse" << YAML::Value << m_ViewProjectionInverse;
//	out << YAML::EndMap;
//}