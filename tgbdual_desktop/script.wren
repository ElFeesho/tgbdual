foreign class GameBoy {
    construct new() {}

    foreign static print(text)
    foreign static addRect(x, y, w, h, stroke, fill)
    foreign static addImage(image, x, y)
    foreign static addText(text, x, y)
    foreign static get8bit(address)
    foreign static get16bit(address)
    foreign static set8bit(address, value)
    foreign static set16bit(address, value)
    foreign static registerConsoleCommand(name, func)

    static get24bit(address) {
        return get8bit(address) + (get8bit(address-1) << 8) + (get8bit(address-2) << 16)
    }

    static set24bit(address, value) {
        set8bit(address, value & 0xff)
        set8bit(address-1, (value >> 8) & 0xff)
        set8bit(address-2, (value >> 16) & 0xff)
    }

    foreign static queueKey(key, delay, duration)

    static KEY_A { 0x01 }
    static KEY_B { 0x02 }
    static KEY_SELECT { 0x04 }
    static KEY_START { 0x08 }
    static KEY_DOWN { 0x10 }
    static KEY_UP { 0x20 }
    static KEY_LEFT { 0x40 }
    static KEY_RIGHT { 0x80 }
}

class Enemy {
    construct new(x, y) {
        _x = x
        _y = y
        _healthBar = HealthBar.new(_x + 10, _y + 40, 300, 20)
    }

    pokemonNumber { GameBoy.get8bit(0x1204) }
    hp { GameBoy.get8bit(0x1217) }
    hpMax { GameBoy.get8bit(0x1219) }

    draw() {
        GameBoy.addRect(_x, _y, 320, 100, 0xffffffff, 0xff666666)

        if (hp > 0) {
            _healthBar.healthPercentage = hp/hpMax
            _healthBar.draw()
            GameBoy.addImage("imgs/%(pokemonNumber).png", _x+138, _y+5)
            GameBoy.addText("%(hp)/%(hpMax)", _x+145, _y+48)
        } else {
            GameBoy.addText("(No Enemy)", _x+115, _y+48)
        }
    }
}

class HealthBar {
    construct new(x, y, width, height) {
        _x = x
        _y = y
        _w = width
        _h = height
        _health = 1
    }

    healthPercentage=(v) {
        _health = v
    }

    draw() {
        GameBoy.addRect(_x, _y, _w, _h, 0xff000000, 0xff333333)
        GameBoy.addRect(_x + 2, _y + 2, (_w - 4) * _health, _h - 4, 0x00000000, 0xff00ff00)
    }
}

class Pokemon {
    construct new(rosterPosition, x, y) {
        _rosterPosition = rosterPosition
        _addressShift = (0x30*(_rosterPosition-1))
        _x = x
        _y = y
        _itemList = [ "(none)", "Master Ball", "Ultra Ball", "BrightPowder", "Great Ball", "Poké Ball", "Teru-sama", "Bicycle", "Moon Stone", "Antidote", "Burn Heal", "Ice Heal", "Awakening", "Parlyz Heal", "Full Restore", "Max Potion", "Hyper Potion", "Super Potion", "Potion", "Escape Rope", "Repel", "Max Elixer", "Fire Stone", "Thunder Stone", "Water Stone", "Teru-sama", "HP Up", "Protein", "Iron", "Carbos", "Lucky Punch", "Calcium", "Rare Candy", "X Accuracy", "Leaf Stone", "Metal Powder", "Nugget", "Poké Doll", "Full Heal", "Revive", "Max Revive", "Guard Spec.", "Super Repel", "Max Repel", "Dire Hit", "Teru-sama", "Fresh Water", "Soda Pop", "Lemonade", "X Attack", "Teru-sama", "X Defend", "X Speed", "X Special", "Coin Case", "Itemfinder", "Teru-sama", "Exp Share", "Old Rod", "Good Rod", "Silver Leaf", "Super Rod", "PP Up", "Ether", "Max Ether", "Elixer", "Red Scale", "SecretPotion", "S.S. Ticket", "Mystery Egg", "Clear Bell*", "Silver Wing", "Moomoo Milk", "Quick Claw", "PSNCureBerry", "Gold Leaf", "Soft Sand", "Sharp Beak", "PRZCureBerry", "Burnt Berry", "Ice Berry", "Poison Barb", "King's Rock", "Bitter Berry", "Mint Berry", "Red Apricorn", "TinyMushroom", "Big Mushroom", "SilverPowder", "Blu Apricorn", "Teru-sama", "Amulet Coin", "Ylw Apricorn", "Grn Apricorn", "Cleanse Tag", "Mystic Water", "TwistedSpoon", "Wht Apricorn", "Black Belt", "Blk Apricorn", "Teru-sama", "Pnk Apricorn", "BlackGlasses", "SlowpokeTail", "Pink Bow", "Stick", "Smoke Ball", "NeverMeltIce", "Magnet", "MiracleBerry", "Pearl", "Big Pearl", "Everstone", "Spell Tag", "RageCandyBar", "GS Ball*", "Blue Card*", "Miracle Seed", "Thick Club", "Focus Band", "Teru-sama", "EnergyPowder", "Energy Root", "Heal Powder", "Revival Herb", "Hard Stone", "Lucky Egg", "Card Key", "Machine Part", "Egg Ticket*", "Lost Item", "Stardust", "Star Piece", "Basement Key", "Pass", "3Teru-sama", "Charcoal", "Berry Juice", "Scope Lens", "2Teru-sama", "Metal Coat", "Dragon Fang", "Teru-sama", "Leftovers", "3Teru-sama", "MysteryBerry", "Dragon Scale", "Berserk Gene", "3Teru-sama", "Sacred Ash", "Heavy Ball", "Flower Mail", "Level Ball", "Lure Ball", "Fast Ball", "Teru-sama", "Light Ball", "Friend Ball", "Moon Ball", "Love Ball", "Normal Box", "Gorgeous Box", "Sun Stone", "Polkadot Bow", "Teru-sama", "Up-Grade", "Berry", "Gold Berry", "SquirtBottle", "Teru-sama", "Park Ball", "Rainbow Wing", "Teru-sama", "Brick Piece", "Surf Mail", "Litebluemail", "Portraitmail", "Lovely Mail", "Eon Mail", "Morph Mail", "Bluesky Mail", "Music Mail", "Mirage Mail", "Teru-sama", "TM01", "TM02", "TM03", "TM04", "TM05", "TM06", "TM07", "TM08", "TM09", "TM10", "TM11", "TM12", "TM13", "TM14", "TM15", "TM16", "TM17", "TM18", "TM19", "TM20", "TM21", "TM22", "TM23", "TM24", "TM25", "TM26", "TM27", "TM28", "TM29", "TM30", "TM31", "TM32", "TM33", "TM34", "TM35", "TM36", "TM37", "TM38", "TM39", "TM40", "TM41", "TM42", "TM43", "TM44", "TM45", "TM46", "TM47", "TM48", "TM49", "TM50", "HM01", "HM02", "HM03", "HM04", "HM05", "HM06", "HM07", "HM08", "HM09", "HM10", "HM11", "HM12" ]
        _healthBar = HealthBar.new(_x+10, _y+40, 80, 10)
    }

    level=(lvl) {
        GameBoy.set8bit((0x1cfe) + (0x30*(_rosterPosition-1)), lvl)
    }

    level { GameBoy.get8bit((0x1cfe) + (0x30*(_rosterPosition-1))) }

    experience { GameBoy.get24bit(0x1ce9 + _addressShift) }
    experience=(value) {
        GameBoy.set24bit(0x1ce9 + _addressShift, value)
    }

    hp { GameBoy.get8bit(0x1d02 + _addressShift) }
    hp=(value) { GameBoy.set8bit(0x1d02 + _addressShift, value) }

    pp(moveNumber) { GameBoy.get8bit(0x1cf6 + moveNumber + _addressShift) }

    move(moveNumber) { GameBoy.get8bit(0x1ce1 + moveNumber + _addressShift) }

    hpMax { GameBoy.get8bit(0x1d04 + _addressShift) }

    rosterPosition { _rosterPosition }

    item { GameBoy.get8bit(0x1ce0 + _addressShift) }
    item=(num) { GameBoy.set8bit(0x1ce0 + _addressShift, num) }

    pokemonNumber { GameBoy.get8bit(0x1cd8+(_rosterPosition-1)) }

    originalTrainer { GameBoy.get16bit(0x1ce5 + _addressShift) }

    attack { GameBoy.get8bit(0x1d06 + _addressShift) }
    specialAttack { GameBoy.get8bit(0x1d06 + _addressShift) }

    isPresent {
        return pokemonNumber > 0 && pokemonNumber < 252
    }

    draw() {
        if (isPresent) {
            GameBoy.addRect(_x, _y, 100, 100, 0xffffffff, 0xff666666)
            GameBoy.addText("Lv %(level)", _x + 50, _y + 20)
            GameBoy.addText("XP %(experience)", _x + 10, _y + 75)
            _healthBar.healthPercentage = hp/hpMax
            _healthBar.draw()

            GameBoy.addImage("imgs/%(pokemonNumber).png", _x+5, _y+5)

            if (item < _itemList.count) {
                GameBoy.addText("I: %(_itemList[item])", _x + 10, _y + 60)
            }
        }
    }
}

class Player {
    construct new() {

    }

    currentRoom { GameBoy.get8bit(0x1fe4) }
    currentArea { GameBoy.get8bit(0x1fe3) }
    currentWorld { GameBoy.get8bit(0x1fe5) }
    money { GameBoy.get24bit(0x1850) }
}

class MenuHelper {
    construct new() {
        _x = 0
        _y = 400
        _w = 200
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
var pokemonThree = Pokemon.new(3, 0, 200)

var pokemonFour = Pokemon.new(4, 419, 0)
var pokemonFive = Pokemon.new(5, 419, 100)
var pokemonSix = Pokemon.new(6,  419, 200)
var pokemons = [ pokemon, pokemonTwo, pokemonThree, pokemonFour, pokemonFive, pokemonSix ]

var targetAddress = 0x1cd8

var enemy = Enemy.new(100, 0)

var player = Player.new()

var tick = Fn.new {

    menuHelper.draw()
    pokemon.draw()
    pokemonTwo.draw()
    pokemonThree.draw()
    pokemonFour.draw()
    pokemonFive.draw()
    pokemonSix.draw()

    enemy.draw()

    GameBoy.addText("Current Room %(player.currentRoom)", 300, 400)
    GameBoy.addText("Current Room %(player.currentArea)", 300, 410)
    GameBoy.addText("Current Room %(player.currentWorld)", 300, 420)
    GameBoy.addText("Current Room %(player.money)", 300, 430)
    GameBoy.addText("Repel: %(GameBoy.get8bit(0x1ca1))", 380, 440)
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
    if (output == "") {
        output = "0"
    }
    return output
}

var activate = Fn.new {
//    GameBoy.set8bit(0x110e, poke)
//    GameBoy.set8bit(0x1cd8, poke)
//    GameBoy.set8bit(0x1cdf, poke)
    //var offset = 0
    //GameBoy.print("Begin Dump")
    //(0..0x36).each { |num| GameBoy.print("%(dec2hex.call(0x1cd8+num)): %(GameBoy.get8bit(0x1cd8+num)) 0x%(dec2hex.call(GameBoy.get8bit(0x1cd8+num)))") }
    //GameBoy.print("Dump Complete")

    pokemon.item = 1
}

var bound = false
var onLoad = Fn.new {
    GameBoy.registerConsoleCommand("woop", Fn.new { |args|
        GameBoy.print("Awesome %(args)")
    })
}

var handleCommand = Fn.new { |command,args|

    GameBoy.print("HEllo? %(command)")
    if (command == "set_pokemon_exp") {
        if (args.count != 2) {
            GameBoy.print("Usage: set_pokemon_exp [pokemon_number (1-6)] [exp 24bit]")
        } else {
            pokemons[Num.fromString(args[0])].experience = Num.fromString(args[1])
        }
        return true
    }

    if (command == "set_pokemon_lvl") {
        if (args.count != 2) {
            GameBoy.print("Usage: set_pokemon_lvl [pokemon_number (1-6)] [level 1-100]")
        } else {
            pokemons[Num.fromString(args[0])].level = Num.fromString(args[1])
        }
        return true
    }

    if (command == "pokemon_centre") {
        if (args.count != 0) {
            GameBoy.print("Usage: pokemon_centre")
        } else {
            pokemons.each { |poke|
                poke.hp = poke.hpMax
            }
        }
        return true
    }

    if (command == "give_item") {
        if (args.count != 2) {
            GameBoy.print("Usage: give_item [pokemon number (1-6)] [Item number (0-245)]")
        } else {
            var itemNumber = Num.fromString(args[1])
            if (itemNumber < 0) {
                itemNumber = 0
            } else if (itemNumber > 245) {
                itemNumber = 245
            }

            pokemons[Num.fromString(args[0])].item = itemNumber
        }
        return true
    }
    return false
}

