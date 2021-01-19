#include "Framework.h"
#include "Texture.h"
#include "Importer/ImageImporter.h"

#include "Util/FileStream.h"
#include "Util/FileSystem.h"

using namespace Framework;

Texture::Texture(Context* context) : IResource(context)
{
	_typecode = TypeCode::Texture;
}

Texture::~Texture()
{
}

bool Texture::LoadFromFile(std::wstring_view path)
{
	if (FileSystem::GetFileExtensionFromPath(path) != Extension_TextureW)
	{
		ImageImporter importer;
		importer.Load(path, this->shared_from_this()); // bit array 로 데이터를 불러 와서

		Create();  // ID3D11ShaderResourceView 를 위에서 불러온 bit array 를 이용해 만듬
	}
	else
	{
		FileStream stream;
		stream.Open(std::wstring(path), StreamMode::Read);
		{
			uint n_mipChain = 0;
			stream.Read(n_mipChain);
			m_mipChains.resize(n_mipChain);

			for (int i = 0; i < n_mipChain; i++)
			{
				std::vector<std::byte>& mipChain = m_mipChains[i];
				stream.Read(mipChain);
			}
			stream.Read(m_bpp);
			stream.Read(m_bpc);
			stream.Read(m_width);
			stream.Read(m_height);
			stream.Read(m_channel);
			stream.Read(m_is_grayed);
			stream.Read(m_is_transparent);
			stream.Read(m_is_mip_chained);
		}
		if (m_mipChains.size() == 1 && m_mipChains[0].size() == 0)
			int a = 0;

		Create();  // ID3D11ShaderResourceView 를 위에서 불러온 bit array 를 이용해 만듬
		Clear_MipChain();  // importer 로 들고온건 SaveToFile 로 mip 재이용해야함
	}

	

	return true;
}

bool Texture::SaveToFile(std::wstring_view path) const
{
	if (FileSystem::GetFileExtensionFromPath(path) != Extension_TextureW)
		return true;

	FileStream stream;
	uint n_mipChain = m_mipChains.size();
	
	if (n_mipChain == 0)  // already cleared mipchain
		return false;

	stream.Open(std::wstring(path), StreamMode::Write);
	{		
		stream.Write(n_mipChain);

		for (int i = 0; i < n_mipChain; i++)
		{
			const std::vector<std::byte>& mipChain = m_mipChains[i];
			stream.Write(mipChain);
		}
		stream.Write(m_bpp);
		stream.Write(m_bpc);
		stream.Write(m_width);
		stream.Write(m_height);
		stream.Write(m_channel);
		stream.Write(m_is_grayed);
		stream.Write(m_is_transparent);
		stream.Write(m_is_mip_chained);
	}
	return true;
}


void Texture::Clear()
{
	m_bpp = 0;
	m_bpc = 0;
	m_width = 0;
	m_height = 0;
	m_channel = 0;
	m_is_grayed = false;
	m_is_transparent = false;
	m_is_mip_chained = true;

	Clear_MipChain();
}

void Texture::Clear_MipChain()
{
	for (auto& m_mipChain : m_mipChains)
	{
		m_mipChain.clear();
		m_mipChain.shrink_to_fit();
	}
	m_mipChains.clear();
	m_mipChains.shrink_to_fit();
}

bool Texture::Create()
{	
	return true;
}
