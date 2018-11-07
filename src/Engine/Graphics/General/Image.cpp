#include "Bang/Image.h"

#include "Bang/AARect.h"
#include "Bang/Debug.h"
#include "Bang/ImageIO.h"

namespace Bang
{
Image::Image()
{
}

Image::Image(int width, int height)
{
    Create(width, height);
}

void Image::Create(int width, int height)
{
    m_size = Vector2i(width, height);
    m_pixels.Resize(m_size.x * m_size.y * 4);
}

void Image::Create(int width, int height, const Color &backgroundColor)
{
    Create(width, height);
    for (int i = 0; i < GetHeight(); ++i)
    {
        for (int j = 0; j < GetWidth(); ++j)
        {
            SetPixel(j, i, backgroundColor);
        }
    }
}

Image Image::GetSubImage(const AARecti &subCoords) const
{
    Vector2i subSize = subCoords.GetSize();
    Image subImage(subSize.x, subSize.y);
    for (int y = 0; y < subSize.y; ++y)
    {
        for (int x = 0; x < subSize.x; ++x)
        {
            subImage.SetPixel(
                x,
                y,
                GetPixel(x + subCoords.GetMin().x, y + subCoords.GetMin().y));
        }
    }
    return subImage;
}

void Image::Copy(const Image &image, const Vector2i &pos)
{
    Copy(image, AARecti(pos, pos + image.GetSize()));
}

void Image::Copy(const Image &image,
                 const AARecti &dstRect,
                 ImageResizeMode resizeMode)
{
    Image resizedImage = image;

    Vector2i dstSize = dstRect.GetSize();
    resizedImage.Resize(dstSize, resizeMode);
    for (int y = 0; y < dstSize.y; ++y)
    {
        for (int x = 0; x < dstSize.x; ++x)
        {
            SetPixel(dstRect.GetMin().x + x,
                     dstRect.GetMin().y + y,
                     resizedImage.GetPixel(x, y));
        }
    }
}

void Image::Copy(const Image &image,
                 const AARecti &srcCopyRect,
                 const AARecti &dstCopyRect,
                 ImageResizeMode resizeMode)
{
    Image subImageSrc = image.GetSubImage(srcCopyRect);
    subImageSrc.Resize(dstCopyRect.GetSize(), resizeMode);
    Copy(subImageSrc, dstCopyRect);
}

void Image::AddMargins(const Vector2i &margins,
                       const Color &marginColor,
                       AspectRatioMode arMode)
{
    Image original = *this;

    Vector2i newSize = AspectRatio::GetAspectRatioedSize(
        GetSize(), (margins * 2) + GetSize(), arMode);
    Create(newSize.x, newSize.y, marginColor);
    Copy(original,
         AARecti(Vector2i::Zero(), original.GetSize()),
         AARecti(newSize / 2 - original.GetSize() / 2,
                 newSize / 2 + (original.GetSize() + 1) / 2));
}

void Image::AddMarginsToMatchAspectRatio(const Vector2i &arSizes,
                                         const Color &marginColor)
{
    AddMarginsToMatchAspectRatio(arSizes.x / float(arSizes.y), marginColor);
}

void Image::AddMarginsToMatchAspectRatio(float aspectRatio,
                                         const Color &marginColor)
{
    Vector2i newSize = GetSize();
    if (aspectRatio > 1.0f)
    {
        newSize.x = (GetHeight() * aspectRatio);
    }
    else
    {
        newSize.y = GetWidth() / aspectRatio;
    }
    Vector2i margins = (newSize - GetSize());
    AddMargins(margins / 2, marginColor, AspectRatioMode::IGNORE);
}

void Image::ResizeToMatchAspectRatio(const Vector2i &arSizes,
                                     bool makeBigger,
                                     ImageResizeMode resizeMode)
{
    ResizeToMatchAspectRatio(
        arSizes.x / float(arSizes.y), makeBigger, resizeMode);
}

void Image::ResizeToMatchAspectRatio(float aspectRatio,
                                     bool makeBigger,
                                     ImageResizeMode resizeMode)
{
    Vector2i newSize = GetSize();
    bool modifyWidth = ((aspectRatio > 1.0f) == makeBigger);
    if (modifyWidth)
    {
        newSize.x = (GetHeight() * aspectRatio);
    }
    else
    {
        newSize.y = GetWidth() / aspectRatio;
    }
    Resize(newSize, resizeMode, AspectRatioMode::IGNORE);
}

float Image::GetAspectRatio() const
{
    return GetWidth() / SCAST<float>(Math::Max(GetHeight(), 1));
}

void Image::Resize(const Vector2i &newSize,
                   ImageResizeMode resizeMode,
                   AspectRatioMode arMode)
{
    Resize(newSize.x, newSize.y, resizeMode, arMode);
}

void Image::Resize(int _newWidth,
                   int _newHeight,
                   ImageResizeMode resizeMode,
                   AspectRatioMode arMode)
{
    // First pick the new (width,height), depending on the AspectRatioMode
    Vector2i newSize = AspectRatio::GetAspectRatioedSize(
        GetSize(), Vector2i(_newWidth, _newHeight), arMode);
    if (newSize.x == GetWidth() && newSize.y == GetHeight())
    {
        return;
    }

    // Now do the resizing
    Image original = *this;

    Vector2 sizeProp(original.GetWidth() / SCAST<float>(newSize.x),
                     original.GetHeight() / SCAST<float>(newSize.y));

    Create(newSize.x, newSize.y);
    for (int y = 0; y < newSize.y; ++y)
    {
        for (int x = 0; x < newSize.x; ++x)
        {
            Color newColor;
            if (resizeMode == ImageResizeMode::NEAREST)
            {
                // Pick nearest original pixel
                Vector2 oriCoord = Vector2(x, y) * sizeProp;
                int nearestX = Math::Round(oriCoord.x);
                int nearestY = Math::Round(oriCoord.y);
                newColor = original.GetPixel(nearestX, nearestY);
            }
            else
            {
                // Average all the original pixels mapping to this resized px
                Vector2 oriTopLeftF = Vector2(x, y) * sizeProp;
                Vector2 oriBotRightF = Vector2(x + 1, y + 1) * sizeProp;
                Vector2i oriTopLeft(Math::Floor(oriTopLeftF.x),
                                    Math::Floor(oriTopLeftF.y));
                oriTopLeft = Vector2i::Max(oriTopLeft, Vector2i::Zero());
                Vector2i oriBotRight(Math::Ceil(oriBotRightF.x),
                                     Math::Ceil(oriBotRightF.y));
                oriBotRight = Vector2i::Min(oriBotRight, original.GetSize());

                newColor = Color::Zero();
                for (int oriY = oriTopLeft.y; oriY < oriBotRight.y; ++oriY)
                {
                    for (int oriX = oriTopLeft.x; oriX < oriBotRight.x; ++oriX)
                    {
                        newColor += original.GetPixel(oriX, oriY);
                    }
                }

                int pixels = (oriBotRight.x - oriTopLeft.x) *
                             (oriBotRight.y - oriTopLeft.y);
                newColor /= Math::Max(pixels, 1);
            }
            SetPixel(x, y, newColor);
        }
    }
}

Image Image::Rotated90DegreesRight() const
{
    Image result;
    result.Create(GetHeight(), GetWidth());
    for (int y = 0; y < GetHeight(); ++y)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            result.SetPixel(
                GetHeight() - y - 1, GetWidth() - x - 1, GetPixel(x, y));
        }
    }
    return result;
}

Image Image::Rotated180DegreesRight() const
{
    Image result;
    result.Create(GetWidth(), GetHeight());
    for (int y = 0; y < GetHeight(); ++y)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            result.SetPixel(
                GetWidth() - x - 1, GetHeight() - y - 1, GetPixel(x, y));
        }
    }
    return result;
}

Image Image::Rotated270DegreesRight() const
{
    Image result;
    result.Create(GetHeight(), GetWidth());
    for (int y = 0; y < GetHeight(); ++y)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            result.SetPixel(y, x, GetPixel(x, y));
        }
    }
    return result;
}

void Image::FillTransparentPixels(const Color &color)
{
    for (int y = 0; y < GetHeight(); ++y)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            if (GetPixel(x, y).a == 0)
            {
                SetPixel(x, y, color);
            }
        }
    }
}

Byte *Image::GetData()
{
    return m_pixels.Size() > 0 ? &m_pixels[0] : nullptr;
}

const Byte *Image::GetData() const
{
    return m_pixels.Size() > 0 ? &m_pixels[0] : nullptr;
}

void Image::SetPixel(int x, int y, const Color &color)
{
    ASSERT_MSG(x >= 0 && y >= 0 && x < GetWidth() && y < GetHeight(),
               "Pixel (" << x << ", " << y << ") out of bounds");
    const int coord = (y * GetWidth() + x) * 4;
    m_pixels[coord + 0] = SCAST<Byte>(color.r * 255);
    m_pixels[coord + 1] = SCAST<Byte>(color.g * 255);
    m_pixels[coord + 2] = SCAST<Byte>(color.b * 255);
    m_pixels[coord + 3] = SCAST<Byte>(color.a * 255);
}

Color Image::GetPixel(int x, int y) const
{
    ASSERT_MSG(x >= 0 && y >= 0 && x < GetWidth() && y < GetHeight(),
               "Pixel (" << x << ", " << y << ") out of bounds");
    const std::size_t coord = (y * GetWidth() + x) * 4;
    return Color(m_pixels[coord + 0] / 255.0f,
                 m_pixels[coord + 1] / 255.0f,
                 m_pixels[coord + 2] / 255.0f,
                 m_pixels[coord + 3] / 255.0f);
}

int Image::GetWidth() const
{
    return m_size.x;
}

int Image::GetHeight() const
{
    return m_size.y;
}

const Vector2i &Image::GetSize() const
{
    return m_size;
}

Image Image::InvertedVertically()
{
    Image img = *this;
    for (int y = 0; y < GetHeight(); ++y)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            Color c = GetPixel(x, y);
            img.SetPixel(x, GetHeight() - y - 1, c);
        }
    }
    return img;
}

Image Image::InvertedHorizontally()
{
    Image img = *this;
    for (int y = 0; y < GetHeight(); ++y)
    {
        for (int x = 0; x < GetWidth(); ++x)
        {
            Color c = GetPixel(x, y);
            img.SetPixel(GetWidth() - x - 1, y, c);
        }
    }
    return img;
}

Image Image::LoadFromData(int width,
                          int height,
                          const Array<Byte> &rgbaByteData)
{
    Image img(width, height);
    img.m_pixels = rgbaByteData;
    ASSERT(int(rgbaByteData.Size()) == (img.GetWidth() * img.GetHeight() * 4));
    return img;
}

void Image::Import(const Path &imageFilepath)
{
    bool ok;
    ImageIO::Import(imageFilepath, this, &ok);
}

void Image::Export(const Path &filepath) const
{
    ImageIO::Export(filepath, *this);
}
}  // namespace Bang
