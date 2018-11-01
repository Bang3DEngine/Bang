#include "Bang/GEngineDebugger.h"

#include "Bang/File.h"
#include "Bang/GBuffer.h"
#include "Bang/Input.h"
#include "Bang/Key.h"
#include "Bang/Path.h"
#include "Bang/Time.h"

using namespace Bang;

Path GEngineDebugger::c_debugDir = Path("/home/sephirot47/Bang/tmp");

GEngineDebugger::GEngineDebugger()
{
}

void GEngineDebugger::Reset()
{
    if (Input::GetKeyDown(Key::Q))
    {
        File::Remove(GEngineDebugger::c_debugDir);
        File::CreateDir(GEngineDebugger::c_debugDir);
    }
}

void GEngineDebugger::TakeGBufferShot(GBuffer *gbuffer,
                                      GL::Attachment att,
                                      const String &screenshotName)
{
    if (Input::GetKeyDown(Key::Q))
    {
        String fileName = String::ToString(int(Time::GetNow().GetMillis())) +
                          "_" + screenshotName;
        Path filepath =
            GEngineDebugger::c_debugDir.Append(fileName).AppendExtension("bmp");
        gbuffer->Export(att, filepath, true);
    }
}

void GEngineDebugger::TakeGBufferShotStencil(GBuffer *gbuffer,
                                             const String &screenshotName)
{
}
