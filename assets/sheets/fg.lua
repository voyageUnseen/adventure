require "classes/game"
require "classes/costume"
require "classes/sheet"

local sheet = Sheet.new("foreground")

local quad = MOAIGfxQuad2D.new()
quad:setTexture("assets/static/numbers.png")
quad:setRect(0, 0, 256, 256)
quad:setUVRect(0, 0, 1, 1)

local prop = MOAIProp2D.new()
prop:setLoc(500, 500)
prop:setDeck(quad)

local santino = MOAIProp2D.new()
santino:setLoc(400, 400)
sheet:insertProp(santino)
costumes.santino:bind(santino)
costumes.santino:refresh_anim()

sheet:insertProp(prop)
sheet:install()

sheet:allowHover(true)

function sheet:onHover(prop, x, y)
    game.setHoverText("hello")
    game.setCursor(5)

    return true
end
