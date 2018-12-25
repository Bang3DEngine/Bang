#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/Image.h"
#include "Bang/ImageIO.h"
#include "Bang/MetaNode.h"
#include "Bang/String.h"
#include "Bang/Texture.h"

namespace Bang
{
class Color;
class Path;

class Texture2D : public Texture
{
    ASSET(Texture2D)

public:
    Texture2D(const Texture2D &tex) = delete;

    void CreateEmpty(const Vector2i &size);
    bool Resize(const Vector2i &size);
    void CreateEmpty(int width, int height);
    bool Resize(int width, int height);
    void Fill(const Color &fillColor, int width, int height);
    void Fill(const Byte *newData,
              int width,
              int height,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);

    void SetAlphaCutoff(float alphaCutoff);

    int GetWidth() const;
    int GetHeight() const;
    Image ToImage() const;
    const Vector2i &GetSize() const;
    float GetAlphaCutoff() const;
    const Image &GetImage() const;
    uint GetBytesSize() const;

    void Import(const Image &image);

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // Asset
    virtual void Import(const Path &imageFilepath) override;

protected:
    Texture2D();
    virtual ~Texture2D() override;

    void SetWidth(int width);
    void SetHeight(int height);

    Image ToImage(GL::TextureTarget texTarget) const;

    void OnFormatChanged() override;

private:
    Image m_image;
    float m_alphaCutoff = 0.0f;
    Vector2i m_size = Vector2i::Zero();
};
}  // namespace Bang

#endif  // TEXTURE2D_H
