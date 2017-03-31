foreign class GameBoy {
    construct new() {}

    foreign static print(text)
    foreign static clearCanvas()
    foreign static addRect(x, y, w, h, stroke, fill)
    foreign static addImage(image, x, y)
    foreign static addText(text, x, y)
    foreign static get8bit(address)
    foreign static get16bit(address)
    foreign static set8bit(address, value)
    foreign static set16bit(address, value)
    foreign static queueKey(key, delay, duration)
}

class Pokemon {
    construct new(rosterPosition) {
        _rosterPosition = rosterPosition
    }

    hp { GameBoy.get8bit(0x1d02 + (0x30*(_rosterPosition-1))) }

    hpMax { GameBoy.get8bit(0x1d04 + (0x30*(_rosterPosition-1))) }

    rosterPosition { _rosterPosition }

    pokemonNumber { GameBoy.get8bit(0x1cd8+(_rosterPosition-1)) }

    isPresent {
        GameBoy.print("IS PRESENT")
        return pokemonNumber > 0 && pokemonNumber < 252
    }
}

var activeItemPosition = Fn.new {
    return GameBoy.get8bit(0xfa9) + GameBoy.get8bit(0x10e4) - 1
}

var tick = Fn.new {
    GameBoy.clearCanvas()
    GameBoy.addRect(10, 10, 100, 80, 0xffff00ff, 0xff0088ff)
    GameBoy.addText("MenuPos %(GameBoy.get8bit(0xf74))", 15, 15)
    GameBoy.addText("Item Count %(GameBoy.get8bit(0x1894 + activeItemPosition.call()*2))", 15, 25)
    GameBoy.addText("Item Type %(GameBoy.get8bit(0x1893 + activeItemPosition.call()*2))", 15, 35)

    GameBoy.addText("Item pos: %(activeItemPosition.call())", 15, 45)
    GameBoy.addText("Scroll: %(GameBoy.get8bit(0x10e4))", 15, 55)
    GameBoy.addText("TMScroll: %(GameBoy.get8bit(0x10e2))", 15, 65)
}

var activate = Fn.new {
    GameBoy.set8bit(0x1893, GameBoy.get8bit(0x1893)+1)
    GameBoy.queueKey(0x10, 0, 100)
    GameBoy.queueKey(0x20, 200, 100)
}