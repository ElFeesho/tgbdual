//
// Created by chris on 02/04/17.
//

#include "Console.h"
#include <SDL/SDL_gfxPrimitives.h>

Console::Console() {

}

void Console::open() {
    _open = true;
}

void Console::close() {
    _open = false;
}

void Console::draw(SDL_Surface *screen) {
    if (_open) {
        Sint16 x[] = {0, 0, (Sint16) screen->w, (Sint16) screen->w};
        Sint16 y[] = {0, (Sint16) (screen->h / 2), (Sint16) (screen->h / 2), 0};
        filledPolygonRGBA(screen, x, y, 4, 33, 33, 33, 220);
        stringRGBA(screen, 3, (Sint16) (screen->h / 2 - 10), ">", 255, 255, 255, 255);

        stringRGBA(screen, (Sint16) (10 + _cursorPos * 8), (Sint16) (screen->h / 2 - 10), "_", 255, 255, 255, (Uint8) (SDL_GetTicks() % 255));
        stringRGBA(screen, 10, (Sint16) (screen->h / 2 - 10), _currentLine.c_str(), 255, 255, 255, 255);

        for(int i = 0; i < _history.size(); i++)
        {
            auto history = _history[i];
            if (history.outputType() == OutputType::stdout) {
                stringRGBA(screen, 3, (Sint16) ((Sint16) (screen->h / 2 - 10) - (10 * (_history.size() - i))),
                           history.line().c_str(), 255, 255, 255, 255);
            }
            else
            {
                stringRGBA(screen, 3, (Sint16) ((Sint16) (screen->h / 2 - 10) - (10 * (_history.size() - i))),
                           history.line().c_str(), 255, 0, 0, 255);
            }
        }

    }
}

bool Console::isOpen() {
    return _open;
}

void Console::update(SDLKey key) {
    if (key == SDLK_LEFT) {
        _cursorPos--;
        if (_cursorPos < 0) {
            _cursorPos = 0;
        }
    } else if (key == SDLK_RIGHT) {
        _cursorPos++;
        if (_cursorPos > _currentLine.length()) {
            _cursorPos = _currentLine.length();
        }
    } else if (key == SDLK_BACKSPACE && _cursorPos>0) {
        _currentLine.erase(_cursorPos - 1, _cursorPos);
        _cursorPos--;
    }
    else if(key == SDLK_RETURN) {
        addhOutput(_currentLine);
        addError("Command unrecognised");
        _currentLine = "";
        _cursorPos = 0;
    } else if (key >= SDLK_0 && key <= SDLK_z || key == SDLK_SPACE) {
        _currentLine.insert(_cursorPos, 1, (char) key);
        _cursorPos++;
    }
}

void Console::addhOutput(std::string output) {
    _history.push_back(Console::HistoryLine{output});
}

void Console::addError(std::string error) {
    _history.push_back(Console::HistoryLine{error, OutputType::stderr});
}

Console::HistoryLine::HistoryLine(std::string line, Console::OutputType outputType) : _line(line), _outputType(outputType) {

}

const std::string &Console::HistoryLine::line() {
    return _line;
}

Console::OutputType Console::HistoryLine::outputType() {
    return _outputType;
}