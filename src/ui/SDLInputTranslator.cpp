#include "ui/SDLInputTranslator.h"
#include <spdlog/spdlog.h>

namespace ui {

MouseButton SDLInputTranslator::translateMouseButton(Uint8 sdlButton) {
    auto it = mouseButtonMap_.find(sdlButton);
    return it != mouseButtonMap_.end() ? it->second : MouseButton::Unknown;
}

KeyCode SDLInputTranslator::translateKeyCode(SDL_Keycode sdlKey) {
    auto it = keyCodeMap_.find(sdlKey);
    return it != keyCodeMap_.end() ? it->second : KeyCode::Unknown;
}

int SDLInputTranslator::translateModifiers(Uint16 sdlMod) {
    int modifiers = static_cast<int>(Modifier::None);
    if (sdlMod & SDL_KMOD_SHIFT) modifiers |= static_cast<int>(Modifier::Shift);
    if (sdlMod & SDL_KMOD_CTRL) modifiers |= static_cast<int>(Modifier::Ctrl);
    if (sdlMod & SDL_KMOD_ALT) modifiers |= static_cast<int>(Modifier::Alt);
    if (sdlMod & SDL_KMOD_GUI) modifiers |= static_cast<int>(Modifier::Super);
    return modifiers;
}

std::unique_ptr<IMouseEvent> SDLInputTranslator::createMouseEvent(void* event) {
    SDL_Event* sdlEvent = static_cast<SDL_Event*>(event);
    class SDLMouseEvent : public IMouseEvent {
    public:
        SDLMouseEvent(EventType type, glm::vec2 pos, MouseButton button, glm::vec2 wheelDelta = glm::vec2(0.0f), std::string droppedData = "")
            : type_(type), pos_(pos), button_(button), wheelDelta_(wheelDelta), droppedData_(droppedData) {}
        EventType getType() const override { return type_; }
        glm::vec2 getPosition() const override { return pos_; }
        MouseButton getButton() const override { return button_; }
        glm::vec2 getWheelDelta() const override { return wheelDelta_; }
        std::string getDroppedData() const override { return droppedData_; }

    private:
        EventType type_;
        glm::vec2 pos_;
        MouseButton button_;
        glm::vec2 wheelDelta_;
        std::string droppedData_;
    };

    switch (sdlEvent->type) {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            return std::make_unique<SDLMouseEvent>(EventType::MousePress,
                                                  glm::vec2(sdlEvent->button.x, sdlEvent->button.y),
                                                  translateMouseButton(sdlEvent->button.button));
        case SDL_EVENT_MOUSE_BUTTON_UP:
            return std::make_unique<SDLMouseEvent>(EventType::MouseRelease,
                                                  glm::vec2(sdlEvent->button.x, sdlEvent->button.y),
                                                  translateMouseButton(sdlEvent->button.button));
        case SDL_EVENT_MOUSE_MOTION:
            return std::make_unique<SDLMouseEvent>(EventType::MouseMove,
                                                  glm::vec2(sdlEvent->motion.x, sdlEvent->motion.y),
                                                  MouseButton::Unknown);
        case SDL_EVENT_MOUSE_WHEEL:
            return std::make_unique<SDLMouseEvent>(EventType::MouseWheel,
                                                  glm::vec2(sdlEvent->wheel.x, sdlEvent->wheel.y),
                                                  MouseButton::Unknown,
                                                  glm::vec2(sdlEvent->wheel.x, sdlEvent->wheel.y));
        case SDL_EVENT_DROP_FILE: {
            std::string droppedFile(sdlEvent->drop.data);
            SDL_free((void*)sdlEvent->drop.data);
            return std::make_unique<SDLMouseEvent>(EventType::Drop,
                                                  glm::vec2(0, 0), // Position not provided by SDL_DROPFILE
                                                  MouseButton::Unknown,
                                                  glm::vec2(0.0f),
                                                  droppedFile);
        }
        default:
            return nullptr;
    }
}

std::unique_ptr<IKeyboardEvent> SDLInputTranslator::createKeyboardEvent(void* event) {
    SDL_Event* sdlEvent = static_cast<SDL_Event*>(event);
    class SDLKeyboardEvent : public IKeyboardEvent {
    public:
        SDLKeyboardEvent(EventType type, KeyCode key, int modifiers)
            : type_(type), key_(key), modifiers_(modifiers) {}
        EventType getType() const override { return type_; }
        KeyCode getKeyCode() const override { return key_; }
        int getModifiers() const override { return modifiers_; }

    private:
        EventType type_;
        KeyCode key_;
        int modifiers_;
    };

    if (sdlEvent->type == SDL_EVENT_KEY_DOWN) {
        return std::make_unique<SDLKeyboardEvent>(EventType::KeyPress,
                                                 translateKeyCode(sdlEvent->key.key),
                                                 translateModifiers(sdlEvent->key.mod));
    } else if (sdlEvent->type == SDL_EVENT_KEY_UP) {
        return std::make_unique<SDLKeyboardEvent>(EventType::KeyRelease,
                                                 translateKeyCode(sdlEvent->key.key),
                                                 translateModifiers(sdlEvent->key.mod));
    }
    return nullptr;
}

std::unique_ptr<ITextInputEvent> SDLInputTranslator::createTextInputEvent(void* event) {
    SDL_Event* sdlEvent = static_cast<SDL_Event*>(event);
    class SDLTextInputEvent : public ITextInputEvent {
    public:
        SDLTextInputEvent(EventType type, std::string text)
            : type_(type), text_(text) {}
        EventType getType() const override { return type_; }
        std::string getText() const override { return text_; }

    private:
        EventType type_;
        std::string text_;
    };

    if (sdlEvent->type == SDL_EVENT_TEXT_INPUT) {
        return std::make_unique<SDLTextInputEvent>(EventType::TextInput, sdlEvent->text.text);
    }
    return nullptr;
}

} // namespace ui