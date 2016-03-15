#include "Texture.h"

Texture::Texture(android_app* pApplication,const char* pPath):
mResource(pApplication, pPath), mTextureId(0), mWidth(0), mHeight(0), mPixelsPtr(0)
{}

const char* Texture::getPath()
{
	return mResource.getPath();
}

int32_t Texture::getHeight()
{
	return mHeight;
}

int32_t Texture::getWidth()
{
	return mWidth;
}

uint8_t* Texture::loadImage()
{
	png_byte header[8];
	png_structp pngPtr = NULL;
	png_infop infoPtr = NULL;
	png_byte* imageBuffer = NULL;
	png_bytep* rowPtrs = NULL;
	png_int_32 rowSize;
	bool transparency;

	// Opens and checks image signature (first 8 bytes).
	if (mResource.open() != 1
	|| mResource.read(header, sizeof(header)) != 1
	|| png_sig_cmp(header, 0, 8) != 0)
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);

	// Creates required structures.

	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!pngPtr)
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);

	infoPtr = png_create_info_struct(pngPtr);

	if (!infoPtr)
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);

	// Prepares reading operation by setting-up a read callback.
	png_set_read_fn(pngPtr, &mResource, callback_read);
	// Set-up error management. If an error occurs while reading,
	// code will come back here and jump
	if (setjmp(png_jmpbuf(pngPtr)))
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);

	// Ignores first 8 bytes already read and processes header.
	png_set_sig_bytes(pngPtr, 8);
	png_read_info(pngPtr, infoPtr);
	// Retrieves PNG info and updates PNG struct accordingly.
	png_int_32 depth, colorType;
	png_uint_32 width, height;

	png_get_IHDR(pngPtr, infoPtr, &width, &height,
	&depth, &colorType, NULL, NULL, NULL);

	mWidth = width;
	mHeight = height;

	// Creates a full alpha channel if transparency is encoded as
	// an array of palette entries or a single transparent color.
	transparency = false;

	if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(pngPtr);
		transparency = true;
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);
	}
	// Expands PNG with less than 8bits per channel to 8bits.
	if (depth < 8) {
		png_set_packing (pngPtr);
	// Shrinks PNG with 16bits per color channel down to 8bits.
	} else if (depth == 16) {
		png_set_strip_16(pngPtr);
	}
	// Indicates that image needs conversion to RGBA if needed.
	switch (colorType) {
	case PNG_COLOR_TYPE_PALETTE:
		png_set_palette_to_rgb(pngPtr);
		mFormat = transparency ? GL_RGBA : GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB:
		mFormat = transparency ? GL_RGBA : GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGBA:
		mFormat = GL_RGBA;
		break;
	case PNG_COLOR_TYPE_GRAY:
		png_set_expand_gray_1_2_4_to_8(pngPtr);
		mFormat = transparency ? GL_LUMINANCE_ALPHA:GL_LUMINANCE;
		break;
	case PNG_COLOR_TYPE_GA:
		png_set_expand_gray_1_2_4_to_8(pngPtr);
		mFormat = GL_LUMINANCE_ALPHA;
		break;
	}
	// Validates all tranformations.
	png_read_update_info(pngPtr, infoPtr);

	// Get row size in bytes.
	rowSize = png_get_rowbytes(pngPtr, infoPtr);
	if (rowSize <= 0)
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);
	// Ceates the image buffer that will be sent to OpenGL.
	imageBuffer = new png_byte[rowSize * height];
	if (!imageBuffer)
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);

	// Pointers to each row of the image buffer. Row order is
	// inverted because different coordinate systems are used by
	// OpenGL (1st pixel is at bottom left) and PNGs (top-left).
	rowPtrs = new png_bytep[height];
	if (!rowPtrs)
		return handleError(pngPtr, rowPtrs, imageBuffer, infoPtr);

	for (int32_t i = 0; i < height; ++i)
	{
		rowPtrs[height - (i + 1)] = imageBuffer + i * rowSize;
	}
	// Reads image content.
	png_read_image(pngPtr, rowPtrs);

	// Frees memory and resources.
	mResource.close();
	png_destroy_read_struct(&pngPtr, &infoPtr, NULL);
	delete[] rowPtrs;
	mPixelsPtr = imageBuffer;
	return imageBuffer;
}

void Texture::callback_read(png_structp pngStruct, png_bytep data, png_size_t size)
{
	Resource* resource = ((Resource*) png_get_io_ptr(pngStruct));
	if (resource->read(data, size) != 1)
	{
		resource->close();
	}
}

int Texture::load()
{
	uint8_t* imageBuffer = loadImage();
	if (imageBuffer == NULL)
	{
		LOGW("BUFFER_NULL");
		return 0;
	}

	// Creates a new OpenGL texture.
	GLenum lErrorResult;
	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	// Set-up texture properties.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			GL_REPEAT/*GL_CLAMP_TO_EDGE*/);
	// Loads image data into OpenGL.
	glTexImage2D(GL_TEXTURE_2D, 0, mFormat, mWidth, mHeight, 0,
				 mFormat, GL_UNSIGNED_BYTE, imageBuffer);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	delete[] imageBuffer;
	if (glGetError() != GL_NO_ERROR)
	{
		LOGW("GL_ERR");
		unload();
		return 0;
	}

	return 1;
}

void Texture::unload()
{
	if (mTextureId != 0)
	{
		glDeleteTextures(1, &mTextureId);
		mTextureId = 0;
	}
	mWidth = 0;
	mHeight = 0;
	mFormat = 0;
}

void Texture::apply()
{
	glActiveTexture( GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTextureId);
}


uint8_t* Texture::handleError(png_structp& pngPtr,png_bytep* rowPtrs, png_byte* imageBuffer,png_infop& infoPtr)
{
	LOGW("ERR");
	mResource.close();
	delete[] rowPtrs;
	delete[] imageBuffer;

	if (pngPtr != NULL)
	{
		png_infop* infoPtrP = infoPtr != NULL ? &infoPtr: NULL;
		png_destroy_read_struct(&pngPtr, infoPtrP, NULL);
	}
	return NULL;
}
