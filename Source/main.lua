print(pd_link.say_hello())

local duration = 0
local result = 0
local prevDuration

function doFFT()
	playdate.resetElapsedTime()
	result = pd_link.do_goertzel()
	duration = playdate.getElapsedTime()
	
	print("result: 0x" .. string.format("%x", result))
	print("duration: " .. duration * 1000000 .. "µs")
end


local gfx = playdate.graphics
function playdate.update()
	if prevDuration ~= duration then
		gfx.pushContext()
		local text = tostring(duration * 1000000) .. "us"
		gfx.clear()
		gfx.drawText(text, 10, 10)
		gfx.popContext()
		
		prevDuration = duration
	end
end

local myInputHandlers = {
	AButtonDown = function()
		doFFT()
	end,
}
playdate.inputHandlers.push(myInputHandlers)
