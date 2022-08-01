#pragma once

#include "GraphicsEngineExports.h"
#include "Common/Singleton.h"
#include "Common/VectorMathTypes.h"

struct FIBITMAP;

class GRAPHICSENGINE_API ImageLoader : public Singleton<ImageLoader>
{
friend class Singleton<ImageLoader>;

public:
	class GRAPHICSENGINE_API ImageFile
	{
	friend class ImageLoader;

	public:
		ImageFile(FIBITMAP* fiBitmap);
		~ImageFile();

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		
		tVec4b GetPixelColor(int x, int y);

		const void* GetPixelData() const;		
	
	protected:
		FIBITMAP* GetFiBitmap();

	private:
		FIBITMAP* _fiBitmap;
	};

	~ImageLoader();

	ImageFile* LoadImageFile(const std::string& filename);
	void UnLoadImageFile(ImageFile* imageFile);

protected:
	ImageLoader();
};