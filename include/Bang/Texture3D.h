#ifndef TEXTURE3D_H
#define TEXTURE3D_H

#include "Bang/Texture.h"

namespace Bang
{
class Texture3D : public Texture
{
    ASSET(Texture3D)

public:
    Texture3D();
    virtual ~Texture3D();

    void CreateEmpty(const Vector3i &size);
    void CreateEmpty(uint width, uint height, uint depth);
    bool Resize(const Vector3i &size);
    void Fill(const Byte *newData,
              const Vector3i &size,
              GL::ColorComp inputDataColorComp,
              GL::DataType inputDataType,
              uint paddingFilling = 0);

    uint GetWidth() const;
    uint GetHeight() const;
    uint GetDepth() const;
    const Vector3i &GetSize() const;
    Vector3i GetSizePOT() const;
    uint GetBytesSize() const;

    // GLObject
    GL::BindTarget GetGLBindTarget() const override;

    // IReflectable
    void Reflect() override;

    // Asset
    virtual void Import(const Path &volumeTextureFilepath) override;

private:
    Vector3i m_size = Vector3i::Zero();

    static uint GetPOT(float x);
};
}

#endif  // TEXTURE3D_H
