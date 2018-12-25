#ifndef GBUFFER_H
#define GBUFFER_H

#include <stack>

#include "Bang/AssetHandle.h"
#include "Bang/BangDefines.h"
#include "Bang/Framebuffer.h"
#include "Bang/GL.h"
#include "Bang/String.h"

namespace Bang
{
class ShaderProgram;
class Texture2D;

class GBuffer : public Framebuffer
{
public:
    static const GL::Attachment AttColor;
    static const GL::Attachment AttAlbedo;
    static const GL::Attachment AttLight;
    static const GL::Attachment AttNormal;
    static const GL::Attachment AttMisc;
    static const GL::Attachment AttDepthStencil;

    GBuffer(int width = 1, int height = 1);
    virtual ~GBuffer() override;

    void BindAttachmentsForReading(ShaderProgram *sp);

    void ApplyPassBlend(ShaderProgram *sp,
                        GL::BlendFactor srcBlendFactor,
                        GL::BlendFactor dstBlendFactor,
                        const AARect &mask = AARect::NDCRect());
    void ApplyPass(ShaderProgram *sp,
                   bool willReadFromColor = false,
                   const AARect &mask = AARect::NDCRect());

    using Framebuffer::Resize;
    virtual bool Resize(const Vector2i &size) override;
    void SetAllDrawBuffers() const override;
    void SetAllDrawBuffersExceptColor();
    void SetColorDrawBuffer();
    void SetLightDrawBuffer();
    void SetHDR(bool hdr);

    bool GetHDR() const;
    void PushDepthStencilTexture();
    void SetSceneDepthStencil();
    void SetCanvasDepthStencil();
    void SetOverlayDepthStencil();
    void PopDepthStencilTexture();
    void SetDepthStencilTexture(Texture2D *depthStencilTexture);
    Texture2D *GetSceneDepthStencilTexture() const;
    Texture2D *GetCanvasDepthStencilTexture() const;
    Texture2D *GetOverlayDepthStencilTexture() const;
    Texture2D *GetDepthStencilTexture() const;
    Texture2D *GetColorTexture0() const;
    Texture2D *GetColorTexture1() const;
    Texture2D *GetDrawColorTexture() const;
    Texture2D *GetReadColorTexture() const;

    static String GetMiscTexName();
    static String GetLightTexName();
    static String GetColorsTexName();
    static String GetAlbedoTexName();
    static String GetNormalsTexName();
    static String GetDepthStencilTexName();

private:
    bool m_hdr = false;

    AH<Texture2D> m_colorTexture0;
    AH<Texture2D> m_colorTexture1;
    Texture2D *p_drawColorTexture = nullptr;
    Texture2D *p_readColorTexture = nullptr;

    AH<Texture2D> m_sceneDepthStencilTexture;
    AH<Texture2D> m_canvasDepthStencilTexture;
    AH<Texture2D> m_overlayDepthStencilTexture;
    AH<Texture2D> p_currentDepthStencilTexture;
    std::stack<AH<Texture2D>> m_depthStencilTexturesStack;

    void RenderViewportPlane();
    void PingPongColorBuffers();
    void ApplyPass_(ShaderProgram *sp, const AARect &mask);

    friend class GEngine;
};
}

#endif  // GBUFFER_H
