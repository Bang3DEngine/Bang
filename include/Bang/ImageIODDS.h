#ifndef IMAGEIODDS_H
#define IMAGEIODDS_H

#include "Bang/BangDefines.h"

NAMESPACE_BANG_BEGIN

FORWARD class Path;
FORWARD class Texture2D;

class ImageIODDS
{
public:
    static void ImportDDS(const Path &filepath, Texture2D *tex, bool *_ok);

    ImageIODDS() = delete;
};

NAMESPACE_BANG_END

#endif // IMAGEIODDS_H

