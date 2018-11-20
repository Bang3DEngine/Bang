#include "Bang/ImageIO.h"

#include <stdio.h>

#include <GL/glew.h>
// #include <jmorecfg.h>
#include <jpeglib.h>
#include <png.h>
#include <pngconf.h>
#include <setjmp.h>
#include <stdint.h>
#include <fstream>

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assert.h"
#include "Bang/Color.h"
#include "Bang/Debug.h"
#include "Bang/Image.h"
#include "Bang/ImageIODDS.h"
#include "Bang/ImageIOTGA.h"
#include "Bang/Path.h"
#include "Bang/StreamOperators.h"
#include "Bang/String.h"
#include "Bang/Texture2D.h"

using namespace Bang;

void ImageIO::Export(const Path &filepath, const Image &img)
{
    if (filepath.HasExtension("png"))
    {
        ImageIO::ExportPNG(filepath, img);
    }
    else if (filepath.HasExtension(Array<String>({"jpg", "jpeg"})))
    {
        ImageIO::ExportJPG(filepath, img, 10);
    }
    else if (filepath.HasExtension(Array<String>({"bmp"})))
    {
        ImageIO::ExportBMP(filepath, img);
    }
    else if (filepath.HasExtension(Array<String>({"tga"})))
    {
        ImageIO::ExportTGA(filepath, img);
    }
    else
    {
        Debug_Error("Unrecognized image format " << filepath.GetExtension());
    }
}

void ImageIO::Import(const Path &filepath, Image *img, bool *_ok)
{
    bool ok = false;

    if (filepath.HasExtension("png"))
    {
        ImageIO::ImportPNG(filepath, img, &ok);
    }
    else if (filepath.HasExtension(Array<String>({"jpg", "jpeg"})))
    {
        ImageIO::ImportJPG(filepath, img, &ok);
    }
    else if (filepath.HasExtension(Array<String>({"bmp"})))
    {
        ImageIO::ImportBMP(filepath, img, &ok);
    }
    else if (filepath.HasExtension(Array<String>({"tga"})))
    {
        ImageIO::ImportTGA(filepath, img, &ok);
    }
    else if (filepath.HasExtension(Array<String>({"dds"})))
    {
        // These textures are thought for GPU only, skip
    }
    else if (!filepath.IsEmpty())
    {
        Debug_Error("Unrecognized image format for '" << filepath.GetAbsolute()
                                                      << "'");
    }

    if (_ok)
    {
        *_ok = ok;
    }
}

void ImageIO::Import(const Path &filepath,
                     Image *img,
                     Texture2D *tex,
                     bool *_ok)
{
    bool ok = false;
    if (filepath.HasExtension("dds"))
    {
        ImageIODDS::ImportDDS2D(filepath, tex, _ok);
    }
    else
    {
        ImageIO::Import(filepath, img, _ok);
        tex->Import(*img);
    }

    if (_ok)
    {
        *_ok = ok;
    }
}

struct BMPFileHeader
{
    int16_t bfType;
    int32_t bfSize;
    int16_t bfReserved1;
    int16_t bfReserved2;
    int32_t bfOffBits;
};

struct BMPInfoHeader
{
    int32_t biSize;
    int64_t biWidth;
    int64_t biHeight;
    int16_t biPlanes;
    int16_t biBitCount;
    int32_t biCompression;
    int32_t biSizeImage;
    int64_t biXPelsPerMeter;
    int64_t biYPelsPerMeter;
    int32_t biClrUsed;
    int32_t biClrImportant;
};

void ImageIO::ExportBMP(const Path &filepath, const Image &img)
{
    ASSERT_MSG(false, "ExportBMP not implemented!");
}
void ImageIO::ImportBMP(const Path &filepath, Image *img, bool *ok)
{
    if (ok)
    {
        *ok = false;
    }
    Debug_Error("ImportBMP not implemented!");
    return;

    Byte *datBuff[2] = {nullptr, nullptr};  // Header buffers
    Byte *pixels = nullptr;                 // Pixels

    BMPFileHeader *bmpHeader = nullptr;  // Header
    BMPInfoHeader *bmpInfo = nullptr;    // Info

    // The file... We open it with it's constructor
    std::ifstream file(filepath.GetAbsolute().ToCString(), std::ios::binary);
    if (!file)
    {
        Debug_Error("Failure to open bitmap file " << filepath);
        return;
    }

    // Allocate byte memory that will hold the two headers
    datBuff[0] = new Byte[sizeof(BMPFileHeader)];
    datBuff[1] = new Byte[sizeof(BMPInfoHeader)];

    file.read((char *)datBuff[0], sizeof(BMPFileHeader));
    file.read((char *)datBuff[1], sizeof(BMPInfoHeader));

    // Construct the values from the buffers
    bmpHeader = (BMPFileHeader *)datBuff[0];
    bmpInfo = (BMPInfoHeader *)datBuff[1];

    // Check if the file is an actual BMP file
    if (bmpHeader->bfType != 0x4D42)
    {
        Debug_Error("File '" << filepath << "' isn't a bitmap file");
        return;
    }

    // First allocate pixel memory
    pixels = new Byte[bmpInfo->biSizeImage];

    // Go to where image data starts, then read in image data
    file.seekg(bmpHeader->bfOffBits);
    file.read((char *)pixels, bmpInfo->biSizeImage);

    // We're almost done. We have our image loaded, however it's not in the
    // right format.
    // .bmp files store image data in the BGR format, and we have to convert it
    // to RGB.
    // Since we have the value in bytes, this shouldn't be to hard to accomplish
    Byte tmpRGB = 0;  // Swap buffer
    for (unsigned long i = 0; i < bmpInfo->biSizeImage; i += 3)
    {
        tmpRGB = pixels[i];
        pixels[i] = pixels[i + 2];
        pixels[i + 2] = tmpRGB;
    }

    // Set width and height to the values loaded from the file
    int w = SCAST<int>(bmpInfo->biWidth);
    int h = SCAST<int>(bmpInfo->biHeight);

    int i = 0;
    Debug_Peek(w);
    Debug_Peek(h);
    img->Create(w, h);
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            Color color = Color(pixels[i * 3 + 0] / 255.0f,
                                pixels[i * 3 + 1] / 255.0f,
                                pixels[i * 3 + 2] / 255.0f,
                                1.0f);
            color = Color::Red();
            img->SetPixel(x, y, color);
            ++i;
        }
    }

    // Delete buffers.
    delete[] datBuff[0];
    delete[] datBuff[1];
    delete[] pixels;

    *ok = true;  // Return success code
}

void ImageIO::ExportPNG(const Path &filepath, const Image &img)
{
    FILE *fp = fopen(filepath.GetAbsolute().ToCString(), "wb");
    if (!fp)
    {
        return;
    }

    png_structp png =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);

    png_set_IHDR(png,
                 info,
                 img.GetWidth(),
                 img.GetHeight(),
                 8,
                 PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    png_bytep *rowPointers = new png_bytep[img.GetHeight()];
    for (int y = 0; y < img.GetHeight(); y++)
    {
        rowPointers[y] =
            new png_byte[png_get_rowbytes(png, info) / sizeof(png_byte)];
        for (int x = 0; x < img.GetWidth(); ++x)
        {
            rowPointers[y][x * 4 + 0] = SCAST<Byte>(img.GetPixel(x, y).r * 255);
            rowPointers[y][x * 4 + 1] = SCAST<Byte>(img.GetPixel(x, y).g * 255);
            rowPointers[y][x * 4 + 2] = SCAST<Byte>(img.GetPixel(x, y).b * 255);
            rowPointers[y][x * 4 + 3] = SCAST<Byte>(img.GetPixel(x, y).a * 255);
        }
    }
    png_write_image(png, rowPointers);
    png_write_end(png, NULL);

    for (int y = 0; y < img.GetHeight(); y++)
    {
        delete[] rowPointers[y];
    }
    delete[] rowPointers;

    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

void ImageIO::ImportPNG(const Path &filepath, Image *img, bool *ok)
{
    *ok = false;

    FILE *fp = fopen(filepath.GetAbsolute().ToCString(), "rb");
    if (!fp)
    {
        return;
    }

    png_structp png =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png)
    {
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_read_struct(&png, &info, NULL);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);
    png_read_info(png, info);

    png_byte bit_depth = png_get_bit_depth(png, info);
    if (bit_depth == 16)
    {
        png_set_strip_16(png);
    }

    png_byte colorType = png_get_color_type(png, info);
    if (colorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_palette_to_rgb(png);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    {
        png_set_expand_gray_1_2_4_to_8(png);
    }

    if (png_get_valid(png, info, PNG_INFO_tRNS))
    {
        png_set_tRNS_to_alpha(png);
    }

    // These color_type don't have an alpha channel then fill it with 0xff.
    if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY ||
        colorType == PNG_COLOR_TYPE_PALETTE)
    {
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }

    if (colorType == PNG_COLOR_TYPE_GRAY ||
        colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info);

    int height = png_get_image_height(png, info);
    png_bytep *rowPointers = new png_bytep[height];
    for (int y = 0; y < height; y++)
    {
        rowPointers[y] =
            new png_byte[png_get_rowbytes(png, info) / sizeof(png_byte)];
    }
    png_read_image(png, rowPointers);

    int width = png_get_image_width(png, info);
    img->Create(width, height);
    for (int y = 0; y < height; ++y)
    {
        png_bytep row = rowPointers[y];
        for (int x = 0; x < width; ++x)
        {
            Color c(row[x * 4 + 0] / 255.0f,
                    row[x * 4 + 1] / 255.0f,
                    row[x * 4 + 2] / 255.0f,
                    row[x * 4 + 3] / 255.0f);
            img->SetPixel(x, y, c);
        }
        delete[] row;
    }

    delete[] rowPointers;
    png_destroy_read_struct(&png, &info, nullptr);
    fclose(fp);

    *ok = true;
}

void ImageIO::ExportJPG(const Path &filepath, const Image &img, int quality)
{
    struct jpeg_error_mgr jerr;
    struct jpeg_compress_struct cinfo;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE *fp = fopen(filepath.GetAbsolute().ToCString(), "wb");
    if (!fp)
    {
        return;
    }

    jpeg_stdio_dest(&cinfo, fp);

    cinfo.image_width = img.GetWidth();
    cinfo.image_height = img.GetHeight();
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_EXT_RGBA;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    const int rowStride = img.GetWidth() * 4;
    while (SCAST<int>(cinfo.next_scanline) < img.GetHeight())
    {
        const Byte *rowPointer =
            &(img.GetData()[cinfo.next_scanline * rowStride]);
        Byte *_rowPointer = const_cast<Byte *>(rowPointer);
        jpeg_write_scanlines(&cinfo, &_rowPointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(fp);
    jpeg_destroy_compress(&cinfo);
}

void ImageIO::ImportJPG(const Path &filepath, Image *img, bool *ok)
{
    *ok = false;

    FILE *fp = fopen(filepath.GetAbsolute().ToCString(), "rb");
    if (!fp)
    {
        return;
    }

    jmp_buf setjmp_buffer;
    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct cinfo;
    cinfo.err = jpeg_std_error(&jerr);
    if (setjmp(setjmp_buffer))
    {
        jpeg_destroy_decompress(&cinfo);
        fclose(fp);
        return;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, fp);

    jpeg_read_header(&cinfo, TRUE);

    jpeg_start_decompress(&cinfo);
    const int numComponents = cinfo.output_components;
    const int rowStride = cinfo.output_width * numComponents;
    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)(
        (j_common_ptr)&cinfo, JPOOL_IMAGE, rowStride, 1);

    img->Create(cinfo.output_width, cinfo.output_height);
    while (SCAST<int>(cinfo.output_scanline) < img->GetHeight())
    {
        const int y = cinfo.output_scanline;
        jpeg_read_scanlines(&cinfo, buffer, 1);
        for (int x = 0; x < img->GetWidth(); ++x)
        {
            float a = numComponents == 4
                          ? buffer[0][x * numComponents + 3] / 255.0f
                          : 1.0f;
            Color c(buffer[0][x * numComponents + 0] / 255.0f,
                    buffer[0][x * numComponents + 1] / 255.0f,
                    buffer[0][x * numComponents + 2] / 255.0f,
                    a);
            img->SetPixel(x, y, c);
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);

    *ok = true;
}

void ImageIO::ImportDDS(const Path &filepath, Texture2D *tex, bool *ok)
{
    ImageIODDS::ImportDDS2D(filepath, tex, ok);
}

void ImageIO::ExportTGA(const Path &filepath, const Image &img)
{
    std::ofstream tgafile(filepath.GetAbsolute().ToCString(), std::ios::binary);
    if (!tgafile)
    {
        return;
    }

    // The image header
    Byte header[18] = {0};
    header[2] = 1;  // truecolor
    header[12] = img.GetWidth() & 0xFF;
    header[13] = (img.GetWidth() >> 8) & 0xFF;
    header[14] = img.GetHeight() & 0xFF;
    header[15] = (img.GetHeight() >> 8) & 0xFF;
    header[16] = 24;  // bits per pixel

    tgafile.write((const char *)header, 18);

    // The image data is stored bottom-to-top, left-to-right
    for (int y = img.GetHeight() - 1; y >= 0; y--)
    {
        for (int x = 0; x < img.GetWidth(); x++)
        {
            unsigned char r = SCAST<unsigned char>(img.GetPixel(x, y).r);
            unsigned char g = SCAST<unsigned char>(img.GetPixel(x, y).g);
            unsigned char b = SCAST<unsigned char>(img.GetPixel(x, y).b);
            tgafile.put(b);
            tgafile.put(g);
            tgafile.put(r);
        }
    }

    // The file footer. This part is totally optional.
    static const char footer[26] =
        "\0\0\0\0"          // no extension area
        "\0\0\0\0"          // no developer directory
        "TRUEVISION-XFILE"  // yep, this is a TGA file
        ".";
    tgafile.write(footer, 26);

    tgafile.close();
}

void ImageIO::ImportTGA(const Path &filepath, Image *img, bool *ok)
{
    FILE *file = fopen(filepath.GetAbsolute().ToCString(), "rb");
    if (file)
    {
        int size;
        fseek(file, 0, SEEK_END);
        size = ftell(file);
        fseek(file, 0, SEEK_SET);

        unsigned char *buffer = (unsigned char *)tgaMalloc(size);
        fread(buffer, 1, size, file);

        int *pixels = tgaRead(buffer, &TGA_READER_ARGB);
        int width = tgaGetWidth(buffer);
        int height = tgaGetHeight(buffer);

        img->Create(width, height);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const unsigned int coord = (y * width + x);
                Color c = Color(
                    SCAST<float>(pixels[coord] >> TGA_READER_ARGB.redShift),
                    SCAST<float>(pixels[coord] >> TGA_READER_ARGB.greenShift),
                    SCAST<float>(pixels[coord] >> TGA_READER_ARGB.blueShift),
                    SCAST<float>(pixels[coord] >> TGA_READER_ARGB.alphaShift));
                c /= 255.0f;

                img->SetPixel(x, y, c);
            }
        }

        *ok = true;
        fclose(file);
    }
    else
    {
        *ok = false;
    }
}
