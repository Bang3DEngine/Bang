#ifndef TEXTURECUBEMAP_H
#define TEXTURECUBEMAP_H

#include "Bang/Bang.h"

#include "Bang/GL.h"
#include "Bang/Asset.h"
#include "Bang/Image.h"
#include "Bang/Texture.h"
#include "Bang/Texture2D.h"
#include "Bang/ResourceHandle.h"

NAMESPACE_BANG_BEGIN

FORWARD class Path;
FORWARD class XMLNode;

class TextureCubeMap : public Texture,
                       public Asset
{
    ASSET(TextureCubeMap)

public:
    TextureCubeMap();
    virtual ~TextureCubeMap();

    void Fill(GL::CubeMapDir cubeMapDir,
              const Byte *newData,
              int size,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType);
    void GenerateMipMaps() const;

    void SetDirTexture(GL::CubeMapDir cubeMapDir, Texture2D *tex);

    Texture2D* GetDirTexture(GL::CubeMapDir cubeMapDir) const;

    void Import(const Image<Byte> &topImage,   const Image<Byte> &botImage,
                const Image<Byte> &leftImage,  const Image<Byte> &rightImage,
                const Image<Byte> &frontImage, const Image<Byte> &backImage);

    // Serializable
    virtual void ImportXML(const XMLNode &xmlInfo) override;
    virtual void ExportXML(XMLNode *xmlInfo) const override;

    // Resource
    virtual void Import(const Path &textureCubeMapFilepath) override;

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

private:
    static const std::array<GL::CubeMapDir, 6> AllCubeMapDirs;
    std::array<RH<Texture2D>, 6> m_dirTextures;

    // Texture
    void CreateEmpty(int width, int height) override;
    void Resize(int width, int height) override;

    static unsigned int GetDirIndex(GL::CubeMapDir dir);
};

NAMESPACE_BANG_END

#endif // TEXTURECUBEMAP_H

