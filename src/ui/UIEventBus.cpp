#include "ui/UIEventBus.h"
#include "ui/UIElement.h"
#include <spdlog/spdlog.h>

namespace ui {

UIEventBus& UIEventBus::getInstance() {
    static UIEventBus instance;
    return instance;
}

void UIEventBus::registerHandler(const std::string& eventType, UIElement* subscriber, EventHandler handler) {
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[eventType][subscriber] = handler;
    spdlog::debug("Registered handler for {} to event: {}", (void*)subscriber, eventType);
}

void UIEventBus::unregisterHandler(const std::string& eventType, UIElement* subscriber) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto& subscribers = handlers_[eventType];
    subscribers.erase(subscriber);
    if (subscribers.empty()) {
        handlers_.erase(eventType);
    }
    spdlog::debug("Unregistered handler for {} from event: {}", (void*)subscriber, eventType);
}

void UIEventBus::unregisterAllForSubscriber(UIElement* subscriber) {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& [eventType, subscribers] : handlers_) {
        subscribers.erase(subscriber);
        if (subscribers.empty()) {
            handlers_.erase(eventType);
        }
    }
    spdlog::debug("Unregistered all handlers for: {}", (void*)subscriber);
}

void UIEventBus::publish(const std::string& eventType, UIElement* publisher, EventType data) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = handlers_.find(eventType);
    if (it != handlers_.end()) {
        for (const auto& [subscriber, handler] : it->second) {
            handler(publisher, data);
        }
        spdlog::debug("Published event {} from {}", eventType, (void*)publisher);
    }
}

} // namespace ui