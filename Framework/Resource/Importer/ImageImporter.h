#pragma once
#include <FreeImage/FreeImage.h>
#include <FreeImage/Utilities.h>

namespace Framework
{
	class ImageImporter final
	{
	public:
		ImageImporter();
		~ImageImporter();

		bool Load(std::wstring_view path, const std::shared_ptr<class Texture>& texture);

	protected:
		void SetErrorFuc();
		std::tuple<FREE_IMAGE_FORMAT, bool> CheckPath(std::string_view path);
		FIBITMAP* ApplyBitmapCorrections(FIBITMAP* bitmap);
		FIBITMAP* Rescale(FIBITMAP* bitmap, const std::shared_ptr<Texture>& texture);
		bool GenerateMipmap(FIBITMAP* bitmap, const std::shared_ptr<Texture>& texture);
		
		bool CopyBitsFromBITMAP(std::vector<std::byte>& data, FIBITMAP* bitmap);

		unsigned int ComputeChannelCount(FIBITMAP* bitmap);
		unsigned int ComputeBitsPerChannel(FIBITMAP* bitmap);
		bool SwapRedBlue32(FIBITMAP* bitmap);
		bool IsVisuallyGrayScaled(FIBITMAP* bitmap);
	};
}