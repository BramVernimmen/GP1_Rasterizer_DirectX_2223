#pragma once
#include <SDL_surface.h>
#include <string>
#include "ColorRGB.h"

namespace dae
{
	struct Vector2;

	class Texture
	{
	public:
		~Texture();

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) = delete;
		Texture& operator=(Texture&& other) = delete;

		// Use LoadFromFile -> we want to be able to use a string, this simplifies things
		static Texture* LoadFromFile(ID3D11Device* pDevice, const std::string& path);
		ID3D11ShaderResourceView* GetSRV() const;

	private:
		Texture(ID3D11Device* pDevice, SDL_Surface* pSurface);

		ID3D11Texture2D* m_pResource{};
		ID3D11ShaderResourceView* m_pSRV{};
	};
}