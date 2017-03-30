foreign class GameBoy {
    construct new() {}

    foreign static print(text)
    foreign static clearCanvas()
    foreign static addRect(x, y, w, h, stroke, fill)
    foreign static get8bit(address)
    foreign static get16bit(address)
    foreign static set8bit(address, value)
    foreign static set16bit(address, value)
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

var tick = Fn.new {
    GameBoy.clearCanvas()
    GameBoy.addRect(20, 20, 60, 60, 0xffff00ff, 0xff0088ff)
}

var activate = Fn.new {
    var pokemon = Pokemon.new(1)
    System.print("Pokemon %(pokemon.pokemonNumber)")
}