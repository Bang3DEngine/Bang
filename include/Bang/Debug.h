#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>

#include "Bang/Array.tcc"
#include "Bang/BangDefines.h"
#include "Bang/DebugMessageType.h"
#include "Bang/EventEmitter.h"
#include "Bang/EventListener.tcc"
#include "Bang/IEventsDebug.h"
#include "Bang/String.h"

namespace Bang
{
class Shader;

class Debug : public EventEmitter<IEventsDebug>
{
public:
    static void Log(const String &str, int line, const String &fileName);
    static void DLog(const String &str, int line, const String &fileName);
    static void Warn(const String &str, int line, const String &fileName);
    static void Error(const String &str, int line, const String &fileName);

    static void PrintUniforms(Shader *shader);
    static void PrintUniforms(uint shaderProgramId,
                              uint fromUniformIdx = 0,
                              uint toUniformIdx = SCAST<uint>(-2));
    static void PrintAllUniforms();
    static void PrintUniforms();

    static Debug *GetInstance();

protected:
    Debug();
    virtual ~Debug() override;

    static void Message(DebugMessageType msgType,
                        const String &str,
                        int line,
                        const String &fileName);

    static void OnMessage();

    friend class Application;
};

#define Debug_Log(msg)                             \
    do                                             \
    {                                              \
        std::ostringstream log;                    \
        log << std::boolalpha << msg;              \
        log.flush();                               \
        Debug::Log(log.str(), __LINE__, __FILE__); \
    } while (0)

#define Debug_DLog(msg)                             \
    do                                              \
    {                                               \
        std::ostringstream log;                     \
        log << std::boolalpha << msg;               \
        log.flush();                                \
        Debug::DLog(log.str(), __LINE__, __FILE__); \
    } while (0)

#define Debug_Peek(varName) Debug_Log(#varName << ": " << (varName))
#define Debug_DPeek(varName) Debug_DLog(#varName << ": " << (varName))

#define Debug_Warn(msg)                             \
    do                                              \
    {                                               \
        std::ostringstream log;                     \
        log << std::boolalpha << msg;               \
        log.flush();                                \
        Debug::Warn(log.str(), __LINE__, __FILE__); \
    } while (0)

#define Debug_Error(msg)                             \
    do                                               \
    {                                                \
        std::ostringstream log;                      \
        log << std::boolalpha << msg;                \
        log.flush();                                 \
        Debug::Error(log.str(), __LINE__, __FILE__); \
    } while (0)
}  // namespace Bang

#endif  // DEBUG_H
