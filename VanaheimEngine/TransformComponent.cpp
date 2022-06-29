#include "VanaheimPCH.h"
#include "TransformComponent.h"

#include "yaml-cpp\yaml.h"

TransformComponent::TransformComponent(const DirectX::XMFLOAT3& position, 
									   const DirectX::XMFLOAT4& rotation, 
									   const DirectX::XMFLOAT3& scale)
				   : Component()
				   , m_UpdateWorldMatrix(false)
				   //, m_Pitch(0)
				   //, m_Yaw(0)
				   //, m_Roll(0)
				   , m_Position(position)
				   , m_WorldPosition({0, 0, 0})
				   , m_Scale(scale)
				   , m_WorldScale({ 1, 1, 1 })
				   , m_Rotation(rotation)
				   , m_WorldRotation({ 0, 0, 0, 1 })
				   , m_Forward({ 0, 0, 1 })
				   , m_Right({ 1, 0, 0 })
				   , m_Up({ 0, 1, 0 })
				   , m_WorldUp({ 0, 1, 0 })
				   , m_World()
{
	DirectX::XMStoreFloat4x4(&m_World, DirectX::XMMatrixIdentity());
}

void TransformComponent::Initialize(Scene* /*pParentScene*/)
{}
void TransformComponent::PostInitialize(Scene* /*pParentScene*/)
{
	UpdateVectors(DirectX::XMLoadFloat4(&m_Rotation));
	UpdateWorld();
}
void TransformComponent::Update(const float /*elapsedSec*/)
{}
void TransformComponent::FixedUpdate(const float /*timeEachUpdate*/)
{}

void TransformComponent::Translate(const DirectX::XMFLOAT3& position)
{
	m_Position = position;
	m_UpdateWorldMatrix = true;
}
void TransformComponent::Translate(const DirectX::XMVECTOR& position)
{
	XMStoreFloat3(&m_Position, position);
	m_UpdateWorldMatrix = true;
}
void TransformComponent::Translate(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	m_UpdateWorldMatrix = true;
}
void TransformComponent::Rotate(const DirectX::XMFLOAT3& rotation)
{
	Rotate(rotation.x, rotation.y, rotation.z);
}
void TransformComponent::Rotate(const float x, const float y, const float z)
{
	// Wrap the angle if it goes under -360 or over 360 degrees
	const float xStore{ WrapAngle(x) };
	const float yStore{ WrapAngle(y) };
	const float zStore{ WrapAngle(z) };

	// Assign the values
	m_Rotation.x = DirectX::XMConvertToRadians(xStore);
	m_Rotation.y = DirectX::XMConvertToRadians(yStore);
	m_Rotation.z = DirectX::XMConvertToRadians(zStore);

	m_UpdateWorldMatrix = true;
}
void TransformComponent::Scale(const DirectX::XMFLOAT3& scale)
{
	Scale(scale.x, scale.y, scale.z);
}
void TransformComponent::Scale(const float x, const float y, const float z)
{
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;

	m_UpdateWorldMatrix = true;
}

void TransformComponent::UpdateVectors(const DirectX::XMVECTOR& rotationVector)
{
	const DirectX::XMVECTOR world_Forward{ DirectX::XMVectorSet(0, 0, 1, 0) };
	const DirectX::XMVECTOR world_Right{ DirectX::XMVectorSet(1, 0, 0, 0) };

	const DirectX::XMMATRIX rotMat = DirectX::XMMatrixRotationQuaternion(rotationVector);
	const DirectX::XMVECTOR forward = XMVector3TransformCoord(world_Forward, rotMat);
	const DirectX::XMVECTOR right = XMVector3TransformCoord(world_Right, rotMat);
	const DirectX::XMVECTOR up = DirectX::XMVector3Cross(forward, right);

	XMStoreFloat3(&m_Forward, forward);
	XMStoreFloat3(&m_Right, right);
	XMStoreFloat3(&m_Up, up);
}
void TransformComponent::UpdateWorld()
{
	const DirectX::XMVECTOR rotation{ DirectX::XMLoadFloat4(&m_Rotation) };
	const DirectX::XMMATRIX world = DirectX::XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z) *
									DirectX::XMMatrixRotationRollPitchYawFromVector(rotation) *
									DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	DirectX::XMStoreFloat4x4(&m_World, world);

	// Store the needed world components to their respective variables
	DirectX::XMVECTOR pos, scale, rot;
	if (XMMatrixDecompose(&scale, &rot, &pos, world))
	{
		XMStoreFloat3(&m_WorldPosition, pos);
		XMStoreFloat3(&m_WorldScale, scale);
		XMStoreFloat4(&m_WorldRotation, rot);
	}
}

const float TransformComponent::WrapAngle(const float angle) const
{
	if (angle < -360.f || angle > 360.f)
	{
		return 0.f;
	}

	return angle;
}

const DirectX::XMFLOAT4 TransformComponent::GetRotation(const bool getWorldRotation) const
{
	return {DirectX::XMConvertToDegrees((getWorldRotation != false) ? m_WorldRotation.x : m_Rotation.x),
			DirectX::XMConvertToDegrees((getWorldRotation != false) ? m_WorldRotation.y : m_Rotation.y),
			DirectX::XMConvertToDegrees((getWorldRotation != false) ? m_WorldRotation.z : m_Rotation.z), 1.f};
}

DirectX::XMFLOAT4X4 TransformComponent::GetWorld()
{
	if (m_UpdateWorldMatrix)
	{
		UpdateWorld();	
		UpdateVectors(DirectX::XMLoadFloat4(&m_WorldRotation));
	}

	return m_World;
}

//void TransformComponent::Serialize(YAML::Emitter& out)
//{
//	out << YAML::Key << "TransformComponent";
//	out << YAML::BeginMap;
//
//	out << YAML::Key << "Pitch" << YAML::Value << m_Pitch;
//	out << YAML::Key << "Yaw" << YAML::Value << m_Yaw;
//	out << YAML::Key << "Roll" << YAML::Value << m_Roll;
//
//	out << YAML::Key << "LocalPosition" << YAML::Value << m_Position;
//	out << YAML::Key << "LocalRotation" << YAML::Value << m_Rotation;
//	out << YAML::Key << "LocalScale" << YAML::Value << m_Scale;
//
//	out << YAML::Key << "WorldPosition" << YAML::Value << m_WorldPosition;
//	out << YAML::Key << "WorldRotation" << YAML::Value << m_WorldRotation;
//	out << YAML::Key << "WorldScale" << YAML::Value << m_WorldScale;
//
//	out << YAML::Key << "Forward" << YAML::Value << m_Forward;
//	out << YAML::Key << "Right" << YAML::Value << m_Right;
//	out << YAML::Key << "Up" << YAML::Value << m_Up;
//	out << YAML::Key << "WorldUp" << YAML::Value << m_WorldUp;
//
//	out << YAML::Key << "World" << YAML::Value << m_World;
//
//	out << YAML::EndMap;
//}