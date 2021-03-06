#include "limitter.h"
#include "emulator_time.h"

limitter::limitter(LimittedOperation operation) : _operation{operation} {}

void limitter::fast() {
    _targetTime = 1;
}

void limitter::normal() {
    _targetTime = 16;
}

void limitter::limit() {
    uint32_t startTime = emulator_time::current_time();

    _operation();

    uint32_t operationTime = emulator_time::current_time() - startTime;

    if (operationTime < _targetTime) {
        emulator_time::sleep(_targetTime - operationTime);
    }
}