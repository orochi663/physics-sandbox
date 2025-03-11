#pragma once

namespace ui {

class Observer {
public:
    virtual ~Observer() = default;
    virtual void onStyleUpdate() = 0; // Virtual method for style updates
};

} // namespace ui