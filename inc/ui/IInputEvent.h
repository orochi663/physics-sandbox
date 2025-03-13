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

    // Full definition of KeyCode enum based on keyCodeMap_
    enum class KeyCode {
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Tab, Return, Escape, Space, Backspace,
        Up, Down, Left, Right, PageUp, PageDown, Insert, Delete, Home, End,
        PrintScreen, Pause, CapsLock,
        Minus, Plus, Underscore, Equal,
        SquareBracketOpen, SquareBracketClose,
        CurlyBracketOpen, CurlyBracketClose,
        Slash, BackSlash, Pipe, Colon, Semicolon,
        SingleQuote, DoubleQuote,
        Comma, Dot, AngleBracketOpen, AngleBracketClose,
        QuestionMark,
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
