require "classes/sheet"

local sheet = Sheet.new("background")
local quad = MOAIGfxQuad2D.new()
quad:setTexture("assets/rooms/outside/art.png")
quad:setRect(0, 0, SIZE.x, SIZE.y)
quad:setUVRect(0, 0, 1, 1)

local prop = MOAIProp2D.new()
prop:setDeck(quad)

sheet:insertProp(prop)
sheet:pushRenderPass()

sheet:installClick(true)
sheet:installHover(true)


function sheet:onHover()
    textbox:setString("")
    mouse.cursor = 0
    return true
end

function sheet:onClick(prop, x, y, down)
    if down then
        addVis(function()
            MOAIGfxDevice.setPenColor(0, 0, 1)
            MOAIDraw.fillRect(x - 8, y - 8, x + 8, y + 8)
        end, 3)
    end
        
    return true
end