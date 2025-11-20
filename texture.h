#pragma once
/*==============================================================================

   テクスチャ管理 [texture.h]
														 Author : Youhei Sato
														 Date   : 2024/06/04
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <d3d11.h>

class TextureLoader2D
{
public:
	void Initialize(ID3D11Device* device);
	void Finalize();
	int Load(const std::wstring& texture_filename);
	ID3D11ShaderResourceView* GetTexture(int id);
	int GetTextureWidth(int id) const;
	int GetTextureHeight(int id) const;
private:
	struct TextureData
	{
		std::wstring filename{};
		ID3D11ShaderResourceView* shader_resource_view{ nullptr };
		int width{ 0 };
		int height{ 0 };
	};
	static constexpr int MAX_TEXTURES{ 256 };
	TextureData m_textures[MAX_TEXTURES]{};
	int m_texture_count{ 0 };
	// ※外部からもらうデバイスのポインタなのでReleaseはしちゃダメ
	ID3D11Device* m_device{ nullptr };
};

#endif // TEXTURE_H
