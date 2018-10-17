#ifndef GRAPHICPIPELINEDEBUGGER_H
#define GRAPHICPIPELINEDEBUGGER_H

#include "Bang/BangDefines.h"
#include "Bang/GL.h"
#include "Bang/String.h"

namespace Bang
{
class GBuffer;
class Path;

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
}

#endif  // GRAPHICPIPELINEDEBUGGER_H
