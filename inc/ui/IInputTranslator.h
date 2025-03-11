#pragma once
#include "IInputEvent.h"

namespace ui {

class IInputTranslator {
public:
    virtual ~IInputTranslator() = default;
    virtual std::unique_ptr<IMouseEvent> createMouseEvent(void* event) = 0;
    virtual std::unique_ptr<IKeyboardEvent> createKeyboardEvent(void* event) = 0;
    virtual std::unique_ptr<ITextInputEvent> createTextInputEvent(void* event) = 0;
};

} // namespace ui