#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include <array>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/GL.h"
#include "Bang/IEventsAsset.h"
#include "Bang/ImageIO.h"
#include "Bang/String.h"
#include "Bang/Texture.h"

namespace Bang
{
class Image;
class IEventsAsset;
class MetaNode;
class Path;
class Asset;
class Texture2D;

class TextureCubeMap : public Texture, public EventListener<IEventsAsset>
{
    ASSET(TextureCubeMap)

public:
    TextureCubeMap();
    virtual ~TextureCubeMap() override;

    void CreateEmpty(uint size);
    bool Resize(uint size);
    void Fill(GL::CubeMapDir cubeMapDir,
              const Byte *newData,
              uint size,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);
    void SetSideTexture(GL::CubeMapDir cubeMapDir, Texture2D *tex);

    uint GetSize() const;
    AH<Texture2D> GetSideTexture(GL::CubeMapDir cubeMapDir) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // Asset
    virtual void Import(const Path &textureCubeMapFilepath) override;
    void Import(const Image &topImage,
                const Image &botImage,
                const Image &leftImage,
                const Image &rightImage,
                const Image &frontImage,
                const Image &backImage);

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // IAssetListener
    void OnImported(Asset *res) override;

private:
    std::array<AH<Texture2D>, 6> m_sideTextures;
    uint m_size = 0;

    void FillCubeMapDir(GL::CubeMapDir dir, const Image *img);
    static uint GetDirIndex(GL::CubeMapDir dir);
};
}  // namespace Bang

#endif  // TEXTURECUBEMAP_H
