#pragma once
#include "IInputTranslator.h"
#include <SDL3/SDL.h>
#include <unordered_map>

namespace ui {

class SDLInputTranslator : public IInputTranslator {
public:
    SDLInputTranslator() = default;
    ~SDLInputTranslator() override = default;

    std::unique_ptr<IMouseEvent> createMouseEvent(void* event) override;
    std::unique_ptr<IKeyboardEvent> createKeyboardEvent(void* event) override;
    std::unique_ptr<ITextInputEvent> createTextInputEvent(void* event) override;

private:
    MouseButton translateMouseButton(Uint8 sdlButton);
    KeyCode translateKeyCode(SDL_Keycode sdlKey);
    int translateModifiers(Uint16 sdlMod);

    const std::unordered_map<Uint8, MouseButton> mouseButtonMap_ = {
        {SDL_BUTTON_LEFT, MouseButton::Left},
        {SDL_BUTTON_RIGHT, MouseButton::Right},
        {SDL_BUTTON_MIDDLE, MouseButton::Middle}
    };

    const std::unordered_map<SDL_Keycode, KeyCode> keyCodeMap_ = {
        {SDLK_A, KeyCode::A}, {SDLK_B, KeyCode::B}, {SDLK_C, KeyCode::C}, {SDLK_D, KeyCode::D},
        {SDLK_E, KeyCode::E}, {SDLK_F, KeyCode::F}, {SDLK_G, KeyCode::G}, {SDLK_H, KeyCode::H},
        {SDLK_I, KeyCode::I}, {SDLK_J, KeyCode::J}, {SDLK_K, KeyCode::K}, {SDLK_L, KeyCode::L},
        {SDLK_M, KeyCode::M}, {SDLK_N, KeyCode::N}, {SDLK_O, KeyCode::O}, {SDLK_P, KeyCode::P},
        {SDLK_Q, KeyCode::Q}, {SDLK_R, KeyCode::R}, {SDLK_S, KeyCode::S}, {SDLK_T, KeyCode::T},
        {SDLK_U, KeyCode::U}, {SDLK_V, KeyCode::V}, {SDLK_W, KeyCode::W}, {SDLK_X, KeyCode::X},
        {SDLK_Y, KeyCode::Y}, {SDLK_Z, KeyCode::Z},
        {SDLK_0, KeyCode::Num0}, {SDLK_1, KeyCode::Num1}, {SDLK_2, KeyCode::Num2},
        {SDLK_3, KeyCode::Num3}, {SDLK_4, KeyCode::Num4}, {SDLK_5, KeyCode::Num5},
        {SDLK_6, KeyCode::Num6}, {SDLK_7, KeyCode::Num7}, {SDLK_8, KeyCode::Num8},
        {SDLK_9, KeyCode::Num9},
        {SDLK_F1, KeyCode::F1}, {SDLK_F2, KeyCode::F2}, {SDLK_F3, KeyCode::F3},
        {SDLK_F4, KeyCode::F4}, {SDLK_F5, KeyCode::F5}, {SDLK_F6, KeyCode::F6},
        {SDLK_F7, KeyCode::F7}, {SDLK_F8, KeyCode::F8}, {SDLK_F9, KeyCode::F9},
        {SDLK_F10, KeyCode::F10}, {SDLK_F11, KeyCode::F11}, {SDLK_F12, KeyCode::F12},
        {SDLK_TAB, KeyCode::Tab}, {SDLK_RETURN, KeyCode::Return}, {SDLK_ESCAPE, KeyCode::Escape},
        {SDLK_SPACE, KeyCode::Space}, {SDLK_BACKSPACE, KeyCode::Backspace},
        {SDLK_UP, KeyCode::Up}, {SDLK_DOWN, KeyCode::Down}, {SDLK_LEFT, KeyCode::Left},
        {SDLK_RIGHT, KeyCode::Right}, {SDLK_PAGEUP, KeyCode::PageUp}, {SDLK_PAGEDOWN, KeyCode::PageDown},
        {SDLK_INSERT, KeyCode::Insert}, {SDLK_DELETE, KeyCode::Delete},
        {SDLK_HOME, KeyCode::Home}, {SDLK_END, KeyCode::End},
        {SDLK_PRINTSCREEN, KeyCode::PrintScreen}, {SDLK_PAUSE, KeyCode::Pause},
        {SDLK_CAPSLOCK, KeyCode::CapsLock},
        {SDLK_MINUS, KeyCode::Minus}, {SDLK_PLUS, KeyCode::Plus},
        {SDLK_UNDERSCORE, KeyCode::Underscore}, {SDLK_EQUALS, KeyCode::Equal},
        {SDLK_LEFTBRACKET, KeyCode::SquareBracketOpen}, {SDLK_RIGHTBRACKET, KeyCode::SquareBracketClose},
        {SDLK_LEFTBRACE, KeyCode::CurlyBracketOpen}, {SDLK_RIGHTBRACE, KeyCode::CurlyBracketClose},
        {SDLK_SLASH, KeyCode::Slash}, {SDLK_BACKSLASH, KeyCode::BackSlash},
        {SDLK_PIPE, KeyCode::Pipe}, {SDLK_COLON, KeyCode::Colon},
        {SDLK_SEMICOLON, KeyCode::Semicolon}, {SDLK_APOSTROPHE, KeyCode::SingleQuote},
        {SDLK_DBLAPOSTROPHE, KeyCode::DoubleQuote}, {SDLK_COMMA, KeyCode::Comma},
        {SDLK_PERIOD, KeyCode::Dot}, {SDLK_LESS, KeyCode::AngleBracketOpen},
        {SDLK_GREATER, KeyCode::AngleBracketClose}, {SDLK_QUESTION, KeyCode::QuestionMark}
    };
};

} // namespace ui