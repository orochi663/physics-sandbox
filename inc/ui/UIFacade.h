#pragma once
#include "UIManager.h"
#include "UICanvas.h"
#include <memory>

namespace ui {

class UIFacade {
public:
    static void initialize();
    static void shutdown();
    static void createCanvas(const std::string& styleType = "canvas", int zIndex = 0);
    static void setTheme(std::unique_ptr<UITheme> theme);
    static void processInput(void* rawEvent);
    static void update();
    static void render(IRenderer* renderer);

private:
    static UIManager* manager_;
};

} // namespace ui