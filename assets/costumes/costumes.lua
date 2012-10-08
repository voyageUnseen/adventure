-- this file is procedurally generated by utils/build_costume.py
-- so you probably shouldn't edit it by hand :)

require "classes/costume"

local cost, anim, deck, curve

-- art/santino
--------------------------------------------------

cost = Costume.new()
cost:addPose("idle", 5, "assets/costumes/santino/idle.png", 24, 40, 60, true)
cost:addPose("walk", 8, "assets/costumes/santino/walk8.png", 24, 40, 60, true)
cost:addPose("walk", 2, "assets/costumes/santino/walk2.png", 24, 40, 60, true)
cost:addPose("walk", 4, "assets/costumes/santino/walk4.png", 24, 40, 60, true)
cost:addPose("walk", 6, "assets/costumes/santino/walk6.png", 24, 40, 60, true)
costumes.santino = cost

-- art/charles
--------------------------------------------------

cost = Costume.new()
cost:addPose("idle", 5, "assets/costumes/charles/idle.png", 1, 24, 52, true)
costumes.charles = cost

