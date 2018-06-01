#ifndef INPUT_H
#define INPUT_H

#include "Bang/Bang.h"

#include <iostream>
#include BANG_SDL2_INCLUDE(SDL.h)

#include "Bang/Key.h"
#include "Bang/UMap.h"
#include "Bang/Array.h"
#include "Bang/String.h"
#include "Bang/Vector2.h"
#include "Bang/IToString.h"
#include "Bang/MouseButton.h"

NAMESPACE_BANG_BEGIN

FORWARD class Window;

class Input
{
public:
    static String KeyToString(Key k);

    static bool GetKey(Key k);
    static bool GetKeyUp(Key k);
    static bool GetKeyDown(Key k);
    static bool GetKeyDownRepeat(Key k);

    static const Array<Key>& GetKeysUp();
    static const Array<Key>& GetKeysDown();
    static const Array<Key>& GetPressedKeys();
    static const Array<Key>& GetKeysDownRepeat();

    static Vector2 GetMouseWheel();

    static Array<MouseButton> GetMouseButtons();
    static Array<MouseButton> GetMouseButtonsUp();
    static Array<MouseButton> GetMouseButtonsDown();
    static bool GetMouseButton(MouseButton mb);
    static bool GetMouseButtonUp(MouseButton mb);
    static bool GetMouseButtonDown(MouseButton mb);
    static bool GetMouseButtonDoubleClick(MouseButton mb);

    static bool IsMouseInsideWindow();

    static float GetMouseAxisX();
    static float GetMouseAxisY();
    static Vector2 GetMouseAxis();

    static int GetMouseDeltaX();
    static int GetMouseDeltaY();
    static Vector2i GetMouseDelta();

    static void SetMouseWrapping(bool isMouseWrapping);
    static bool IsMouseWrapping();

    static void LockMouseMovement(bool lock);
    static bool IsLockMouseMovement();

    static void SetMousePositionWindow(int windowMousePosX,  int windowMousePosY);
    static void SetMousePositionWindow(const Vector2i &windowMousePosition);
    static Vector2i GetMousePosition();
    static Vector2  GetMousePositionNDC();
    static Vector2i GetMousePositionWindow();
    static Vector2  GetMousePositionWindowNDC();
    static Vector2i GetPreviousMousePositionWindow();

    static void StartTextInput();
    static String PollInputText();
    static void StopTextInput();

    void Reset();

    static Input* GetActive();

private:
    static constexpr float DoubleClickMaxSeconds = 0.25f;

    float m_lastMouseDownTimestamp = 0;
    bool m_isMouseWrapping         = false;
    bool m_isADoubleClick          = false;
    bool m_lockMouseMovement       = false;
    bool m_isMouseInside           = false;
    Vector2 m_lastMouseWheelDelta  = Vector2::Zero;

    String m_inputText = "";

    Vector2i m_lastMousePosWindow = Vector2i::Zero;
    Vector2i m_lastClickMousePos  = Vector2i::Zero;

    Array<Key> m_keysUp;
    Array<Key> m_keysDown;
    Array<Key> m_pressedKeys;
    Array<Key> m_keysDownRepeat;

    Input();
    virtual ~Input();

    struct EventInfo : public IToString
    {
        enum class Type
        { NONE, KEY_DOWN, KEY_UP, MOUSE_DOWN, MOUSE_UP, MOUSE_MOVE, WHEEL };

        Type type               = Type::NONE;
        Key key                 = Key::NONE;
        MouseButton mouseButton = MouseButton::NONE;
        bool autoRepeat         = false;
        int x                   = 0;
        int y                   = 0;
        float timestampSecs     = 0;
        Vector2 wheelDelta      = Vector2::Zero;

        String ToString() const override { return String::ToString(type); }
    };


    struct ButtonInfo : public IToString
    {
        bool up = false;         // Just one frame
        bool down = false;       // Just one frame
        bool pressed = false;    // Long duration
        bool autoRepeat = false;

        ButtonInfo() { up = down = pressed = false; }
        ButtonInfo(bool up, bool down, bool pressed)
        {
            this->up = up;
            this->down = down;
            this->pressed = pressed;
        }

        String ToString() const override
        {
            std::ostringstream oss;
            oss << "(Up: " << up << ", Down: " << down <<
                   ", Pressed: " << pressed << ")";
            return String(oss.str());
        }
    };

    UMap<Key, ButtonInfo, EnumClassHash> m_keyInfos;
    UMap<MouseButton, ButtonInfo, EnumClassHash> m_mouseInfo;
    Array<EventInfo> m_eventInfoQueue;

    void ProcessMouseWheelEventInfo(const EventInfo &ei);
    void ProcessMouseMoveEventInfo(const EventInfo &ei);
    void ProcessMouseDownEventInfo(const EventInfo &ei);
    void ProcessMouseUpEventInfo(const EventInfo &ei);
    void ProcessKeyDownEventInfo(const EventInfo &ei);
    void ProcessKeyUpEventInfo(const EventInfo &ei);

    void PeekEvent(const SDL_Event &event, const Window *window);
    void EnqueueEvent(const EventInfo &eventInfo);
    void ProcessEventInfo(const EventInfo &eventInfo);

    void ProcessEnqueuedEvents();
    void OnFrameFinished();

    friend class Window;
};

NAMESPACE_BANG_END

#endif // INPUT_H
