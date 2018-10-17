#ifndef IMAGEIO_H
#define IMAGEIO_H

#include "Bang/BangDefines.h"
#include "Bang/Image.h"

namespace Bang
{
class Path;
class Texture2D;

using Imageb = Image<Byte>;

class ImageIO
{
public:
    static void Export(const Path &filepath, const Imageb &img);
    static void Import(const Path &filepath, Imageb *img, bool *ok = nullptr);
    static void Import(const Path &filepath,
                       Imageb *img,
                       Texture2D *tex,
                       bool *ok = nullptr);

    ImageIO() = delete;

private:
    static void ExportBMP(const Path &filepath, const Imageb &img);
    static void ImportBMP(const Path &filepath, Imageb *img, bool *ok);

    static void ExportPNG(const Path &filepath, const Imageb &img);
    static void ImportPNG(const Path &filepath, Imageb *img, bool *ok);

    static void ExportJPG(const Path &filepath, const Imageb &img, int quality);
    static void ImportJPG(const Path &filepath, Imageb *img, bool *ok);

    static void ImportDDS(const Path &filepath, Texture2D *tex, bool *ok);

    static void ExportTGA(const Path &filepath, const Imageb &img);
    static void ImportTGA(const Path &filepath, Imageb *img, bool *ok);
};
}

#endif  // IMAGEREADER_H
