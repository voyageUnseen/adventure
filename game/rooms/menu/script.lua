local topic = Topic.new("test")
topic:addOptions(
    {
        caption = "New Game",
        flags = "xs",
        callback = function()
            Room.change("meadow")
        end
    },
    {
        caption = "Exit",
        flags = "xs",
        callback = function()
            os.exit()
        end
    }
)

game.updateBuffer(" ");
topic:show()
