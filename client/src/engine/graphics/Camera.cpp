#include "Camera.h"

Camera::Camera()
{
	position_ = new Vector3;
	rotation_ = new Vector3;
}

void Camera::Render()
{
	XMFLOAT3 up;
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	XMVECTOR up_vector;
	up_vector = XMLoadFloat3(&up);

	XMFLOAT3 position;
	position.x = position_->x;
	position.y = position_->y;
	position.z = position_->z;

	XMVECTOR position_vector;
	position_vector = XMLoadFloat3(&position);

	XMFLOAT3 look_at;
	look_at.x = 0.0f;
	look_at.y = 0.0f;
	look_at.z = 1.0f;

	XMVECTOR look_at_vector;
	look_at_vector = XMLoadFloat3(&look_at);

	float pitch = rotation_->x * 0.0174532925f;
	float yaw = rotation_->y * 0.0174532925f;
	float roll = rotation_->z * 0.0174532925f;

	XMMATRIX rotationMatrix;
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	look_at_vector = XMVector3TransformCoord(look_at_vector, rotationMatrix);
	up_vector = XMVector3TransformCoord(up_vector, rotationMatrix);

	look_at_vector = XMVectorAdd(position_vector, look_at_vector);

	view_matrix_ = XMMatrixLookAtLH(position_vector, look_at_vector, up_vector);
}

void Camera::GetViewMatrix(XMMATRIX& view_matrix)
{
	view_matrix = view_matrix_;
}