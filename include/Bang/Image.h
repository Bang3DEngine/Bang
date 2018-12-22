#ifndef IMAGE_H
#define IMAGE_H

#include "Bang/Array.h"
#include "Bang/AspectRatioMode.h"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/Color.h"
#include "Bang/String.h"

namespace Bang
{
class Path;

enum class ImageResizeMode
{
    NEAREST,
    LINEAR
};

class Image : public Asset
{
    ASSET(Image)

public:
    Image();
    Image(int width, int height);
    ~Image() override = default;

    void Create(int width, int height);
    void Create(int width, int height, const Color &backgroundColor);
    void SetPixel(int x, int y, const Color &color);

    Image GetSubImage(const AARecti &subImageCoordsPx) const;
    void Copy(const Image &image, const Vector2i &pos);
    void Copy(const Image &image,
              const AARecti &dstRect,
              ImageResizeMode resizeMode = ImageResizeMode::LINEAR);
    void Copy(const Image &image,
              const AARecti &srcCopyRect,
              const AARecti &dstCopyRect,
              ImageResizeMode resizeMode = ImageResizeMode::LINEAR);

    void AddMargins(const Vector2i &margins,
                    const Color &marginColor = Color::Zero(),
                    AspectRatioMode arMode = AspectRatioMode::IGNORE);

    void AddMarginsToMatchAspectRatio(const Vector2i &aspectRatioSizes,
                                      const Color &marginColor = Color::Zero());
    void AddMarginsToMatchAspectRatio(float aspectRatio,
                                      const Color &marginColor = Color::Zero());

    void ResizeToMatchAspectRatio(
        const Vector2i &aspectRatioSizes,
        bool makeBigger = false,
        ImageResizeMode resizeMode = ImageResizeMode::LINEAR);
    void ResizeToMatchAspectRatio(
        float aspectRatio,
        bool makeBigger = false,
        ImageResizeMode resizeMode = ImageResizeMode::LINEAR);

    float GetAspectRatio() const;

    void Resize(const Vector2i &newSize,
                ImageResizeMode resizeMode = ImageResizeMode::LINEAR,
                AspectRatioMode arMode = AspectRatioMode::IGNORE);
    void Resize(int newWidth,
                int newHeight,
                ImageResizeMode resizeMode = ImageResizeMode::LINEAR,
                AspectRatioMode arMode = AspectRatioMode::IGNORE);

    Image Rotated90DegreesRight() const;
    Image Rotated180DegreesRight() const;
    Image Rotated270DegreesRight() const;

    void FillTransparentPixels(const Color &color);

    Byte *GetData();
    const Byte *GetData() const;
    Color GetPixel(int x, int y) const;
    int GetWidth() const;
    int GetHeight() const;
    const Vector2i &GetSize() const;

    Image InvertedVertically();
    Image InvertedHorizontally();

    void Export(const Path &filepath) const;
    static Image LoadFromData(int width,
                              int height,
                              const Array<Byte> &rgbaByteData);

    // Asset
    void Import(const Path &imageFilepath) override;

private:
    Vector2i m_size = Vector2i::Zero();
    Array<Byte> m_pixels;
};
}  // namespace Bang

#endif  // IMAGE_H
