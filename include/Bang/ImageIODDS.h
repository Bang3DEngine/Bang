#ifndef IMAGEIODDS_H
#define IMAGEIODDS_H

#include "Bang/BangDefines.h"

namespace Bang
{
class Path;
class Texture2D;

class ImageIODDS
{
public:
    static void ImportDDS(const Path &filepath, Texture2D *tex, bool *_ok);

    ImageIODDS() = delete;
};
}

#endif  // IMAGEIODDS_H
