#include "texture.h"

#include <string>
#include "extern/DirectXTex.h"
#include "render/directx.h"

#if _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib");
#else
#pragma comment(lib, "DirectXTex_Release.lib");
#endif

using namespace DirectX;

void TextureLoader2D::Initialize(ID3D11Device* device)
{
	for (int i = 0; i < MAX_TEXTURES; i++) {
		m_textures[i].shader_resource_view = nullptr;
	}
	m_texture_count = 0;
	m_device = device;
}

void TextureLoader2D::Finalize(void)
{
	for (int i = 0; i < m_texture_count; i++) {
		// m_textures[i].shader_resource_view->Release();
		// m_textures[i] = NULL;
		SAFE_RELEASE(m_textures[i].shader_resource_view);
	}
}

int TextureLoader2D::Load(const std::wstring& texture_filename)
{
	// すでに同名のテクスチャが読み込まれていないか確認する
	for (int i = 0; i < m_texture_count; i++) {
		if (m_textures[i].filename == texture_filename) {
			return i; // すでに読み込まれていたのでidをわたす
		}
	}
	// テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(texture_filename.c_str(), WIC_FLAGS_NONE, &metadata, image);
	CreateShaderResourceView(m_device, image.GetImages(), image.GetImageCount(), metadata, &m_textures[m_texture_count].shader_resource_view);
	m_textures[m_texture_count].width = (int)metadata.width;
	m_textures[m_texture_count].height = (int)metadata.height;
	m_textures[m_texture_count].filename = texture_filename;

	if (!m_textures[m_texture_count].shader_resource_view) {
		MessageBoxW(nullptr, L"ファイルが読み込めなかった", texture_filename.c_str(), MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}

	return m_texture_count++;
}

ID3D11ShaderResourceView* TextureLoader2D::GetTexture(int id)
{
	if (id < 0 || id >= m_texture_count) {
		return nullptr;
	}
	return m_textures[id].shader_resource_view;
}

int TextureLoader2D::GetTextureWidth(int id) const
{
	return m_textures[id].width;
}

int TextureLoader2D::GetTextureHeight(int id) const
{
	return m_textures[id].height;
}
