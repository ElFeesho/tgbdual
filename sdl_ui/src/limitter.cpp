//
// Created by chris on 31/03/17.
//

#include "limitter.h"

#include <SDL/SDL.h>

limitter::limitter(LimittedOperation operation) : _operation{operation} {}

void limitter::fast() {
    _targetTime = 1;
}

void limitter::normal() {
    _targetTime = 16;
}

void limitter::limit() {
    uint32_t startTime = SDL_GetTicks();

    _operation();

    uint32_t operationTime = SDL_GetTicks() - startTime;

    if (operationTime < _targetTime) {
        SDL_Delay(_targetTime - operationTime);
    }
}