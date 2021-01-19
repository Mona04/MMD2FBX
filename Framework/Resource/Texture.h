#pragma once
#include <vector>
#include <cstddef>
#include <memory>
#include "IResource.h"

namespace Framework
{
	class Texture : public IResource, public std::enable_shared_from_this<Texture>
	{
	public:
		Texture(Context* context);
		virtual ~Texture();

		Texture(Texture& rhs) = delete;
		Texture(Texture&& rhs) = delete;
		Texture& operator=(Texture& rhs) = delete;
		Texture& operator=(Texture&& rhs) = delete;

		virtual bool LoadFromFile(std::wstring_view path) override;
		virtual bool SaveToFile(std::wstring_view path) const  override;
		virtual void Clear() override;

		void Clear_MipChain();

		bool Create();

		std::vector<std::vector<std::byte>>& GetMipChains() { return m_mipChains; }
		std::vector<std::byte>& AddMipChain() { return m_mipChains.emplace_back(std::vector<std::byte>()); }

	public:
		void SetBpp(unsigned int var) { m_bpp = var; }
		unsigned int GetBpp() { return m_bpp; }

		void SetBpc(unsigned int var) { m_bpc = var; }
		unsigned int GetBpc() { return m_bpc; }

		void SetWidth(unsigned int var) { m_width = var; }
		unsigned int GetWidth() { return m_width; }

		void SetHeight(unsigned int var) { m_height = var; }
		unsigned int GetHeight() { return m_height; }

		void SetChannel(unsigned int var) { m_channel = var; }
		unsigned int GetChannel() { return m_channel; }

		void SetIsGrayed(bool var) { m_is_grayed = var; }
		unsigned int GetIsGrayed() { return m_is_grayed; }

		void SetIsTransparent(bool var) { m_is_transparent = var; }
		unsigned int GetIsTransparent() { return m_is_transparent; }

		void SetIsMipChained(bool var) { m_is_mip_chained = var; }
		unsigned int GetIsMipChained() { return m_is_mip_chained; }

	protected:
		std::vector<std::vector<std::byte>> m_mipChains;

		unsigned int m_bpp     = 0;
		unsigned int m_bpc     = 0;
		unsigned int m_width   = 0;
		unsigned int m_height  = 0; 
		unsigned int m_channel = 0;
		bool m_is_grayed       = false;
		bool m_is_transparent  = false;
		bool m_is_mip_chained  = false;
	};
}