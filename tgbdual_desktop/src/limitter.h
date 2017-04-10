//
// Created by chris on 31/03/17.
//

#pragma once

#include <functional>
#include <cstdint>

class limitter {
public:
    using LimittedOperation = std::function<void()>;
    limitter(LimittedOperation operation);

    void fast();
    void normal();

    void limit();
private:
    LimittedOperation _operation;
    uint32_t _targetTime { 16 };
};
