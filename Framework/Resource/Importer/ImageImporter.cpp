#include "Framework.h"
#include "ImageImporter.h"
#include "Util/FileSystem.h"
#include "Resource/Texture.h"

using namespace Framework;

ImageImporter::ImageImporter()
{
    SetErrorFuc();
}

ImageImporter::~ImageImporter()
{
}

bool ImageImporter::Load(std::wstring_view path, const std::shared_ptr<Texture>& texture)
{
	std::string path_A = FileSystem::ToString(path);

	if (!texture)
	{
		LOG_ERROR("Invalid Parameter"); 
		return false;
	}

	auto[format, r] = CheckPath(path_A);
	if (!r)
	{
		LOG_ERROR("Invalid File Path");
		return false;
	}

	FIBITMAP* bitmap = FreeImage_Load(format, path_A.data()); // 1 Load
	{
		bitmap = ApplyBitmapCorrections(bitmap);   // 2 Correcting
		if (!bitmap)
		{
			LOG_ERROR("Failed to apply bitmap correction");
			return false;
		}

		bitmap = Rescale(bitmap, texture);
		if (!bitmap)
		{
			LOG_ERROR("Failed to apply bitmap correction");
			return false;
		}

		if (!GenerateMipmap(bitmap, texture))
		{
			LOG_ERROR("Failed to apply bitmap correction");
			texture->Clear();
			return false;
		}

		texture->SetBpp(FreeImage_GetBPP(bitmap));
		texture->SetBpc(ComputeBitsPerChannel(bitmap));
		texture->SetWidth(FreeImage_GetWidth(bitmap));
		texture->SetHeight(FreeImage_GetHeight(bitmap));
		texture->SetChannel(ComputeChannelCount(bitmap));
		texture->SetIsTransparent(FreeImage_IsTransparent(bitmap));
		texture->SetIsGrayed(IsVisuallyGrayScaled(bitmap));
	}
	FreeImage_Unload(bitmap);

	return true;
}

void ImageImporter::SetErrorFuc()
{
    auto FreeImageErrorHandler = [](FREE_IMAGE_FORMAT fif, const char* message)
    {
        char buff[1024];

        const char* format = (fif != FIF_UNKNOWN) ? FreeImage_GetFormatFromFIF(fif) : "Unknown Error";
        const char* text = (message != nullptr) ? message : "Unknown Error";
        snprintf(buff, sizeof(buff), "%s , Format : %s.", text, format);
        LOG_ERROR(buff);
    };


    FreeImage_SetOutputMessage(FreeImageErrorHandler);
}

std::tuple<FREE_IMAGE_FORMAT, bool> ImageImporter::CheckPath(std::string_view path)
{
	if (!FileSystem::IsExistFile(path))
	{
		LOG_ERROR("Path \"" + std::string(path) + "\" is invalid");
		return { FIF_UNKNOWN, false };
	}

	// 확장자로 검색하고 파일 이름으로 검색하고 두번함.
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path.data(), 0);
	format = (format == FIF_UNKNOWN) ? FreeImage_GetFIFFromFilename(path.data()) : format;

	if (!FreeImage_FIFSupportsReading(format))
	{
		LOG_ERROR("Unknown or unsupported format");
		return { FIF_UNKNOWN, false };
	}

	return { format, true };
}

FIBITMAP* ImageImporter::ApplyBitmapCorrections(FIBITMAP* bitmap)
{
	if (!bitmap)
	{
		LOG_ERROR("bitmap is null, you failed to create FIBITMAP");
		return nullptr;
	}

	unsigned int channels = ComputeChannelCount(bitmap);

	if (channels == 1)
	{
		int bpp = ComputeBitsPerChannel(bitmap);
		if (bpp == 16)
		{
			FIBITMAP* previousBitmap = bitmap;
			bitmap = FreeImage_ConvertTo8Bits(previousBitmap);
			FreeImage_Unload(previousBitmap);
		}

	}

	if (FreeImage_GetBPP(bitmap) < 32)
	{
		FIBITMAP* previousBitmap = bitmap;
		bitmap = FreeImage_ConvertTo32Bits(previousBitmap);
		FreeImage_Unload(previousBitmap);
	}
	if (FreeImage_GetBPP(bitmap) == 32)
	{
		//argb => abgr
		if (FreeImage_GetRedMask(bitmap) == 0x00ff0000 && ComputeChannelCount(bitmap) >= 2)
		{
			bool swapped = SwapRedBlue32(bitmap);  // 내장함수 아님 알아서 구현하면 됨
			
			if (!swapped)			
				LOG_ERROR("Failed to swap red with blue channel");
		}

	}

	//수직으로 뒤집기  -> 이건 뒤집어서 나오니까 일단 이래보자 심정임
	FreeImage_FlipVertical(bitmap);
	
	return bitmap;
}


FIBITMAP* ImageImporter::Rescale(FIBITMAP* bitmap, const std::shared_ptr<Texture>& texture)
{
	if (!bitmap || !texture)
	{
		LOG_ERROR("Invalid arguement");
		return nullptr;
	}

	const bool userDefineDimension = (texture->GetWidth() != 0 && texture->GetHeight() != 0);
	const bool dimensionMisMatch = (FreeImage_GetWidth(bitmap) != texture->GetWidth() && FreeImage_GetHeight(bitmap) != texture->GetHeight());
	if (userDefineDimension && dimensionMisMatch)
	{
		FIBITMAP* previousBitmap = bitmap;
		bitmap = FreeImage_Rescale(previousBitmap, texture->GetWidth(), texture->GetHeight());
		if (!bitmap)
		{
			LOG_ERROR("Failed");
			return previousBitmap;
		}
		FreeImage_Unload(previousBitmap);
	}

	return bitmap;
}


bool ImageImporter::GenerateMipmap(FIBITMAP* bitmap, const std::shared_ptr<Texture>& texture)
{
	uint width = FreeImage_GetWidth(bitmap);
	uint height = FreeImage_GetHeight(bitmap);
	FIBITMAP* _cur = bitmap;
	FIBITMAP* _prev = nullptr;

	CopyBitsFromBITMAP(texture->AddMipChain(), _cur);
	if (!texture->GetIsMipChained())
		return true;  // 원본만 만듬

	while (width > 64 && height > 64)
	{
		width /= 16; height /= 16;
		_prev = _cur;
		_cur = FreeImage_Rescale(_prev, width, height);
		FreeImage_Unload(_prev);

		CopyBitsFromBITMAP(texture->AddMipChain(), _cur);
		width /= 2; height /= 2;
		
		if (!bitmap)
		{
			LOG_ERROR("Failed to generating map");
			return false;
		}
	}
	//FreeImage_Unload(_prev);
	FreeImage_Unload(_cur);
	return true;
}

bool ImageImporter::CopyBitsFromBITMAP(std::vector<std::byte>& data, FIBITMAP* bitmap)
{
	uint width = FreeImage_GetWidth(bitmap);
	uint height = FreeImage_GetHeight(bitmap);
	uint channel = ComputeChannelCount(bitmap);
	if (width == 0 || height == 0 || channel == 0)
	{
		LOG_ERROR("Invalid parameter");
		return false;
	}

	uint size = width * height * channel * (ComputeBitsPerChannel(bitmap) / 8);
	if (size != data.size())
	{
		data.clear();
		data.reserve(size);
		data.resize(size);
	}

	BYTE* bits = FreeImage_GetBits(bitmap);
	memcpy(data.data(), bits, size);

	return true;
}

unsigned int ImageImporter::ComputeChannelCount(FIBITMAP* bitmap)
{
	if (!bitmap)
	{
		LOG_ERROR("invalid argument");
		return 0;
	}

	unsigned int channels = FreeImage_GetBPP(bitmap) / ComputeBitsPerChannel(bitmap);

	return channels;
}

unsigned int ImageImporter::ComputeBitsPerChannel(FIBITMAP* bitmap)
{
	if (!bitmap)
	{
		LOG_ERROR("Invalid parameter");
		return 0;
	}

	FREE_IMAGE_TYPE type = FreeImage_GetImageType(bitmap);
	unsigned int size = 0;

	if (type == FIT_BITMAP) // 1
		size = sizeof(BYTE);
	else if (type == FIT_UINT16 || type == FIT_RGB16 || type == FIT_RGBA16)  // 2
		size = sizeof(WORD);
	else if (type == FIT_FLOAT || type == FIT_RGBF || type == FIT_RGBAF)  // 4
		size = sizeof(float);

	return size * 8;
}

bool ImageImporter::SwapRedBlue32(FIBITMAP* bitmap)
{
	if (FreeImage_GetImageType(bitmap) != FIT_BITMAP)
		return false;

	const unsigned bytesperpixel = FreeImage_GetBPP(bitmap) / 8;
	if (bytesperpixel > 4 || bytesperpixel < 3)
		return false;

	const uint height = FreeImage_GetHeight(bitmap);
	const uint pitch = FreeImage_GetPitch(bitmap);

	BYTE* line = FreeImage_GetBits(bitmap);
	for (uint y = 0; y < height; ++y, line += pitch) {
		for (BYTE* pixel = line; pixel < line + pitch; pixel += bytesperpixel)
			INPLACESWAP(pixel[0], pixel[2]);
	}

	return true;
}

bool ImageImporter::IsVisuallyGrayScaled(FIBITMAP* bitmap)
{
	if (!bitmap)
	{
		LOG_ERROR("Invalid parameter");
		return false;
	}

	switch (FreeImage_GetBPP(bitmap))
	{
	case 1:
	case 4:
	case 8:
	{
		const auto rgb = FreeImage_GetPalette(bitmap);

		if ((rgb->rgbRed != rgb->rgbGreen) || (rgb->rgbRed != rgb->rgbBlue))
			return false;

		return true;
	}
	default:
		return (FreeImage_GetColorType(bitmap) == FIC_MINISBLACK);
	}
}

