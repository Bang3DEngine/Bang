#ifndef IMAGEIO_H
#define IMAGEIO_H

#include "Bang/BangDefines.h"

namespace Bang
{
class Path;
class Image;
class Texture2D;

class ImageIO
{
public:
    static void Export(const Path &filepath, const Image &img);
    static void Import(const Path &filepath, Image *img, bool *ok = nullptr);
    static void Import(const Path &filepath,
                       Image *img,
                       Texture2D *tex,
                       bool *ok = nullptr);

    ImageIO() = delete;

private:
    static void ExportBMP(const Path &filepath, const Image &img);
    static void ImportBMP(const Path &filepath, Image *img, bool *ok);

    static void ExportPNG(const Path &filepath, const Image &img);
    static void ImportPNG(const Path &filepath, Image *img, bool *ok);

    static void ExportJPG(const Path &filepath, const Image &img, int quality);
    static void ImportJPG(const Path &filepath, Image *img, bool *ok);

    static void ImportDDS(const Path &filepath, Texture2D *tex, bool *ok);

    static void ExportTGA(const Path &filepath, const Image &img);
    static void ImportTGA(const Path &filepath, Image *img, bool *ok);
};
}  // namespace Bang

#endif  // IMAGEREADER_H
