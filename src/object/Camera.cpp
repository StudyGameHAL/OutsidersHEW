#include "Camera.h"
#include "render/DirectX.h" // TODO: window size
using namespace DirectX;

XMMATRIX Camera::GetViewMatrix() const
{
	return XMMatrixLookAtLH(
		m_Transform.GetPosition().ToXMVECTOR(),
		m_Target.ToXMVECTOR(),
		m_Up.ToXMVECTOR()
	);
}

XMMATRIX Camera::GetProjectionMatrix() const
{
	return XMMatrixPerspectiveFovLH(m_Fov,
		static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT,
		m_ZNear, m_ZFar);
}
