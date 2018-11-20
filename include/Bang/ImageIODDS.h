#ifndef IMAGEIODDS_H
#define IMAGEIODDS_H

#include "Bang/BangDefines.h"

namespace Bang
{
class Path;
class Texture2D;
class Texture3D;

class ImageIODDS
{
public:
    static void ImportDDS2D(const Path &filepath, Texture2D *tex, bool *_ok);
    static void ImportDDS3D(const Path &filepath, Texture3D *tex, bool *_ok);

    ImageIODDS() = delete;
};
}

#endif  // IMAGEIODDS_H
