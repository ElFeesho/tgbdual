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
    construct new(rosterPosition, x, y) {
        _rosterPosition = rosterPosition
        _x = x
        _y = y
    }

    level { GameBoy.get8bit((0x1cd8+0x26) + (0x30*(_rosterPosition-1))) }

    experience { GameBoy.get16bit(0x1cd8+0x12 + (0x30*(_rosterPosition-1))) }

    hp { GameBoy.get8bit(0x1d02 + (0x30*(_rosterPosition-1))) }

    hpMax { GameBoy.get8bit(0x1d04 + (0x30*(_rosterPosition-1))) }

    rosterPosition { _rosterPosition }

    pokemonNumber { GameBoy.get8bit(0x1cd8+(_rosterPosition-1)) }

    isPresent {
        return pokemonNumber > 0 && pokemonNumber < 252
    }

    draw() {
        if (isPresent) {
            GameBoy.addRect(_x, _y, 100, 100, 0xffffffff, 0xff666666)
            GameBoy.addText("#%(pokemonNumber)", _x + 10, _y + 10)
            GameBoy.addText("Lv. %(level)", _x + 50, _y + 10)
            GameBoy.addText("Exp. %(experience)", _x + 10, _y + 35)
            GameBoy.addRect(_x + 10, _y + 20, 80, 10, 0xff000000, 0xff333333)
            GameBoy.addRect(_x + 12, _y + 22, 76 * (hp/hpMax), 6, 0x00000000, 0xff00ff00)
        }
    }
}

class MenuHelper {
    construct new() {
        _x = 0
        _y = 320
        _w = 300
        _h = 70
    }

    activeItemPosition() {
        return GameBoy.get8bit(0xfa9) + GameBoy.get8bit(0x10e4) - 1
    }

    draw() {
        GameBoy.addRect(_x, _y, _w, _h, 0xffffffff, 0xff333333)
        GameBoy.addText("Item Helper", _x + 5, _y + 5)
        GameBoy.addText("Item Count %(GameBoy.get8bit(0x1894 + activeItemPosition()*2))", _x + 5, _y + 15)
        GameBoy.addText("Item Type %(GameBoy.get8bit(0x1893 + activeItemPosition()*2))", _x + 5, _y + 25)

        GameBoy.addText("Item pos: %(activeItemPosition())", _x + 5, _y + 35)
        GameBoy.addText("Scroll: %(GameBoy.get8bit(0x10e4))", _x + 5, _y + 45)
        GameBoy.addText("TMScroll: %(GameBoy.get8bit(0x10e2))", _x + 5, _y + 55)
    }
}

var menuHelper = MenuHelper.new()
var pokemon = Pokemon.new(1, 0, 0)
var pokemonTwo = Pokemon.new(2, 0, 100)

var targetAddress = 0x1cd8

var tick = Fn.new {

    menuHelper.draw()
    pokemon.draw()
    pokemonTwo.draw()

    GameBoy.addText("%(GameBoy.get16bit(0x1cd8+0x12)) %(GameBoy.get16bit(0x1cea))", 200, 340)
}

var dec2hex = Fn.new { |input|
    var table = [ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f" ]
    var b = input
    var output = ""
    while (b != 0) {
        var r = b%16
        b = (b/16).floor
        output = table[r.ceil] + output
    }
    return output
}

var activate = Fn.new {
//    GameBoy.set8bit(0x110e, poke)
//    GameBoy.set8bit(0x1cd8, poke)
//    GameBoy.set8bit(0x1cdf, poke)
    var offset = 0
    GameBoy.print("Begin Dump")
    (0..48).each { |num| GameBoy.print("%(dec2hex.call(0x1cd8+num)): %(GameBoy.get8bit(0x1cd8+num))") }
    GameBoy.print("Dump Complete")
}