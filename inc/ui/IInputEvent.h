#pragma once
#include <glm/glm.hpp>
#include <string>

namespace ui {

enum class EventType {
    MousePress,
    MouseRelease,
    MouseMove,
    MouseWheel,
    DragStart,
    DragMove,
    Drop,
    KeyPress,
    KeyRelease,
    TextInput
};

enum class MouseButton {
    Left,
    Right,
    Middle,
    Unknown
};

enum class KeyCode {
    // [Existing KeyCode definitions remain unchanged]
    Unknown
};

enum class Modifier {
    None = 0,
    Shift = 1 << 0,
    Ctrl = 1 << 1,
    Alt = 1 << 2,
    Super = 1 << 3
};

class IMouseEvent {
public:
    virtual ~IMouseEvent() = default;
    virtual EventType getType() const = 0;
    virtual glm::vec2 getPosition() const = 0;
    virtual MouseButton getButton() const = 0;
    virtual glm::vec2 getWheelDelta() const { return glm::vec2(0.0f); }
    virtual std::string getDroppedData() const { return ""; }
};

class IKeyboardEvent {
public:
    virtual ~IKeyboardEvent() = default;
    virtual EventType getType() const = 0;
    virtual KeyCode getKeyCode() const = 0;
    virtual int getModifiers() const = 0;
};

class ITextInputEvent {
public:
    virtual ~ITextInputEvent() = default;
    virtual EventType getType() const = 0;
    virtual std::string getText() const = 0;
};

} // namespace ui