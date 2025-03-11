#include "UIFacade.h"

namespace ui {

UIManager* UIFacade::manager_ = nullptr;

void UIFacade::initialize() {
    if (!manager_) manager_ = &UIManager::getInstance();
}

void UIFacade::shutdown() {
    manager_ = nullptr;
}

void UIFacade::createCanvas(const std::string& styleType, int zIndex) {
    if (manager_) manager_->addCanvas(CanvasFactory::createCanvas(styleType, zIndex));
}

void UIFacade::setTheme(std::unique_ptr<UITheme> theme) {
    if (manager_) manager_->setGlobalTheme(std::move(theme));
}

void UIFacade::processInput(void* rawEvent) {
    if (manager_) manager_->processInput(rawEvent);
}

void UIFacade::update() {
    if (manager_) manager_->update();
}

void UIFacade::render(IRenderer* renderer) {
    if (manager_) manager_->render(renderer);
}

} // namespace ui