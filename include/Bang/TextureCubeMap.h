#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include <array>
#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Asset.h"
#include "Bang/BangDefines.h"
#include "Bang/EventEmitter.tcc"
#include "Bang/EventListener.h"
#include "Bang/GL.h"
#include "Bang/IEventsResource.h"
#include "Bang/ImageIO.h"
#include "Bang/ResourceHandle.h"
#include "Bang/String.h"
#include "Bang/Texture.h"

namespace Bang
{
template <class>
class Image;
class IEventsResource;
class MetaNode;
class Path;
class Resource;
class Texture2D;

class TextureCubeMap : public Texture, public EventListener<IEventsResource>
{
    ASSET(TextureCubeMap)

public:
    TextureCubeMap();
    virtual ~TextureCubeMap() override;

    // Texture
    using Texture::Resize;
    using Texture::CreateEmpty;
    void CreateEmpty(int size);
    bool Resize(int size);
    void Fill(GL::CubeMapDir cubeMapDir,
              const Byte *newData,
              int size,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);

    void SetSideTexture(GL::CubeMapDir cubeMapDir, Texture2D *tex);
    Imageb ToImage(GL::CubeMapDir cubeMapDir) const;
    RH<Texture2D> GetSideTexture(GL::CubeMapDir cubeMapDir) const;

    // Serializable
    virtual void ImportMeta(const MetaNode &metaNode) override;
    virtual void ExportMeta(MetaNode *metaNode) const override;

    // Resource
    virtual void Import(const Path &textureCubeMapFilepath) override;
    void Import(const Image<Byte> &topImage,
                const Image<Byte> &botImage,
                const Image<Byte> &leftImage,
                const Image<Byte> &rightImage,
                const Image<Byte> &frontImage,
                const Image<Byte> &backImage);

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // IResourceListener
    void OnImported(Resource *res) override;

private:
    std::array<RH<Texture2D>, 6> m_sideTextures;

    void CreateEmpty(const Vector2i &size) override;
    bool Resize(const Vector2i &size) override;

    void FillCubeMapDir(GL::CubeMapDir dir, const Imageb *img);
    static unsigned int GetDirIndex(GL::CubeMapDir dir);
};
}

#endif  // TEXTURECUBEMAP_H
