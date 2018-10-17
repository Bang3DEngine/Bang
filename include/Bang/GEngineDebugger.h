#ifndef GRAPHICPIPELINEDEBUGGER_H
#define GRAPHICPIPELINEDEBUGGER_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/String.h"

NAMESPACE_BANG_BEGIN

FORWARD class GBuffer;
FORWARD class Path;

class GEngineDebugger
{
public:
    static Path c_debugDir;
    static void Reset();
    static void TakeGBufferShot(GBuffer *gbuffer,
                                GL::Attachment att,
                                const String &screenshotName);
    static void TakeGBufferShotStencil(GBuffer *gbuffer,
                                       const String &screenshotName);

private:
    GEngineDebugger();
};

NAMESPACE_BANG_END

#endif // GRAPHICPIPELINEDEBUGGER_H
