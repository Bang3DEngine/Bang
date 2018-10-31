#ifndef IMAGEEFFECTS_H
#define IMAGEEFFECTS_H

#include "Bang/BangDefines.h"
#include "Bang/ImageIO.h"

namespace Bang
{
class ImageEffects
{
public:
    // Zero  Color (0,0,0,0): background
    // Other Color:           foreground
    static void SignedDistanceField(const Image &inputImageBW,
                                    Image *distanceFieldOutputImage,
                                    int distanceRadius);

    // Zero  Color (0,0,0,0): background
    // Other Color:           foreground
    static void Outline(const Image &inputImageBW, Image *outlineOutputImageBW);

    static bool Albedo(const Image &inputImage1,
                       const Image &inputImage2,
                       Image *diffOutputImage,
                       bool diffAlpha = false);

    ImageEffects() = delete;
};
}  // namespace Bang

#endif  // DISTANCEFIELDER_H
