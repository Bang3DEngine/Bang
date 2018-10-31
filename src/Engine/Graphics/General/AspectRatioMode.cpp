#include "Bang/AspectRatioMode.h"

#include "Bang/Math.h"

using namespace Bang;

Vector2i AspectRatio::GetAspectRatioedSize(const Vector2i &currentSize,
                                           const Vector2i &targetSize,
                                           AspectRatioMode aspectRatioMode)
{
    Vector2i finalSize = targetSize;
    if (aspectRatioMode != AspectRatioMode::IGNORE)
    {
        Vector2 aspectRatio(SCAST<float>(targetSize.x) / currentSize.x,
                            SCAST<float>(targetSize.y) / currentSize.y);

        bool keepExc = (aspectRatioMode == AspectRatioMode::KEEP_EXCEEDING);
        float ar = (aspectRatio.x < aspectRatio.y)
                       ? (keepExc ? aspectRatio.y : aspectRatio.x)
                       : (keepExc ? aspectRatio.x : aspectRatio.y);
        finalSize.x = Math::Round(currentSize.x * ar);
        finalSize.y = Math::Round(currentSize.y * ar);
    }
    return finalSize;
}
