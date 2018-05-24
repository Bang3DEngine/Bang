#ifndef GBUFFER_H
#define GBUFFER_H

#include <stack>

#include "Bang/GL.h"
#include "Bang/Color.h"
#include "Bang/AARect.h"
#include "Bang/Framebuffer.h"

NAMESPACE_BANG_BEGIN

FORWARD class ShaderProgram;

class GBuffer : public Framebuffer
{
public:

    static const GL::Attachment AttColor        = GL::Attachment::COLOR0;

    // (diffColor.r, diffColor.g, diffColor.b, diffColor.a)
    static const GL::Attachment AttAlbedo       = GL::Attachment::COLOR1;

    // (normal.x, normal.y, normal.z, 0)
    static const GL::Attachment AttNormal       = GL::Attachment::COLOR2;

    // (receivesLighting, roughness, metalness, ---)
    // If receivesLighting >  0 ---> receivesShadows
    // If receivesLighting <= 0 ---> not receivesShadows
    static const GL::Attachment AttMisc         = GL::Attachment::COLOR3;

    static const GL::Attachment AttColorRead    = GL::Attachment::COLOR4;

    static const GL::Attachment AttDepthStencil = GL::Attachment::DEPTH_STENCIL;

    GBuffer(int width, int height);
    virtual ~GBuffer();

    void BindAttachmentsForReading(ShaderProgram *sp,
                                   bool readFromCopiedColor = true);

    void ApplyPassBlend(ShaderProgram *sp,
                        GL::BlendFactor srcBlendFactor,
                        GL::BlendFactor dstBlendFactor,
                        const AARect &mask = AARect::NDCRect);
    void ApplyPass(ShaderProgram *sp,
                   bool willReadFromColor = false,
                   const AARect &mask = AARect::NDCRect);

    void PrepareColorReadBuffer(const AARect &readNDCRect = AARect::NDCRect);

    void SetAllDrawBuffers() const override;
    void SetAllDrawBuffersExceptColor();
    void SetColorDrawBuffer();

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

    static String GetMiscTexName();
    static String GetColorsTexName();
    static String GetAlbedoTexName();
    static String GetNormalsTexName();
    static String GetDepthStencilTexName();

private:
    RH<Texture2D> m_sceneDepthStencilTexture;
    RH<Texture2D> m_canvasDepthStencilTexture;
    RH<Texture2D> m_overlayDepthStencilTexture;
    Texture2D *p_currentDepthStencilTexture = nullptr;
    std::stack<RH<Texture2D>> m_depthStencilTexturesStack;

    void RenderViewportPlane();
    void ApplyPass_(ShaderProgram *sp, const AARect &mask);

    friend class GEngine;
};

NAMESPACE_BANG_END

#endif // GBUFFER_H
