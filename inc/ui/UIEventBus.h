#pragma once
#include "IInputEvent.h"
#include <functional>
#include <unordered_map>
#include <memory>

namespace ui {

class UIElement;

class UIEventBus {
public:
    using EventHandler = std::function<void(UIElement*, EventType)>;
    static UIEventBus& getInstance();

    // Registration and unregistration
    void registerHandler(const std::string& eventType, UIElement* subscriber, EventHandler handler);
    void unregisterHandler(const std::string& eventType, UIElement* subscriber);
    void unregisterAllForSubscriber(UIElement* subscriber);

    // Event publishing
    void publish(const std::string& eventType, UIElement* publisher, EventType data);

private:
    UIEventBus() = default;
    ~UIEventBus() = default;
    UIEventBus(const UIEventBus&) = delete;
    UIEventBus& operator=(const UIEventBus&) = delete;

    std::unordered_map<std::string, std::unordered_map<UIElement*, EventHandler>> handlers_;
    std::mutex mutex_;
};

} // namespace ui