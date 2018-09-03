#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include "Bang/Bang.h"

#include "Bang/GL.h"
#include "Bang/Asset.h"
#include "Bang/Image.h"
#include "Bang/Texture2D.h"
#include "Bang/ResourceHandle.h"
#include "Bang/IEventsResource.h"

NAMESPACE_BANG_BEGIN

FORWARD class Path;
FORWARD class MetaNode;

class TextureCubeMap : public Texture,
                       public EventListener<IEventsResource>
{
    ASSET(TextureCubeMap)

public:
    TextureCubeMap();
    virtual ~TextureCubeMap();

    // Texture
    using Texture::Resize;
    using Texture::CreateEmpty;
    void CreateEmpty(int size);
    void Resize(int size);
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
    void Import(const Image<Byte> &topImage,   const Image<Byte> &botImage,
                const Image<Byte> &leftImage,  const Image<Byte> &rightImage,
                const Image<Byte> &frontImage, const Image<Byte> &backImage);

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // IResourceListener
    void OnImported(Resource *res) override;

private:
    std::array<RH<Texture2D>, 6> m_sideTextures;

    void CreateEmpty(const Vector2i &size) override;
    void Resize(const Vector2i &size) override;

    void FillCubeMapDir(GL::CubeMapDir dir, const Imageb *img);
    static unsigned int GetDirIndex(GL::CubeMapDir dir);
};

NAMESPACE_BANG_END

#endif // TEXTURECUBEMAP_H

