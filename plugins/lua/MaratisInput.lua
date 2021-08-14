
PluginName = "Maratis Input"
PluginDescription = "Maratis Input Method"
PluginVersion = "1.0.0"
PluginAuthor = "Yannick Pflanzer"
PluginLicense = "LGPL"

RegisterInputMethod()

local camera = GetEditorCamera()
local cameraObject = camera:getParent()

local function selection_center()
	local selection = GetEditorSelection()
	local position = Neo.Vector3(0, 0, 0)

	for i = 1, #selection, 1 do
		position = position + selection[i]:getGlobalPosition()
	end

	return position/#selection
end

local function getTranslationSpeed()
	return GetEditorMovementSpeed() * 0.002
end

local function getRotationSpeed()
	return GetEditorMovementSpeed() * 0.002
end

local function zoom_vue(mouse, cameraObject)
	local position = cameraObject:getPosition()
	local axis = cameraObject:getTransform():getRotatedVector3(Neo.Vector3(0,0,-1))

	local wheel = mouse:getScrollValue() * -30.0
	local factor = (selection_center() - position):getLength() * 0.01

	cameraObject:setPosition(position - (axis * wheel * factor) * getTranslationSpeed())
end

local function pan_vue(mouse, camera, fov)
	local camTrans = camera:getTransform()
	local xAxis = camTrans:getRotatedVector3(Neo.Vector3(1, 0, 0))
	local zAxis = camTrans:getRotatedVector3(Neo.Vector3(0, 1, 0))

	local position = camera:getGlobalPosition()
	local axis = camTrans:getRotatedVector3(Neo.Vector3(0, 0, -1))

	local z = (selection_center() - position):dotProduct(axis)
	local fovFactor = fov * 0.0192;

	local dp = mouse:getDirection()
	local translationSpeed = -getTranslationSpeed()

	camera:setPosition(position - (xAxis * -dp.x*z*fovFactor)*translationSpeed + (zAxis * -dp.y*z*fovFactor)*translationSpeed)
end

local function rotate_vue(mouse, camera)
	local camTrans = camera:getTransform()

	local center = camTrans:getInverse() * selection_center()
	local position = camera:getGlobalPosition()
	local rotationSpeed = getRotationSpeed()
	local dp = mouse:getDirection()

	camera:setPosition(selection_center())

	local rotation = camera:getRotation()
	rotation:setFromAngles(rotation:getEulerAngles() + Neo.Vector3(-dp.y*150*rotationSpeed, 0, -dp.x*150*rotationSpeed))

	camera:setRotation(rotation)
	camera:setPosition(position)
	camera:updateMatrix()

	center = camTrans * center
	camera:translate(selection_center() - center)
end

function UpdateInput(platform)
	local input = platform:getInputContext()
	local kb = input:getKeyboard()
	local mouse = input:getMouse()

	zoom_vue(mouse, cameraObject)

	if mouse:isKeyDown(Neo.MOUSE_BUTTON_MIDDLE) and input:isKeyDown(Neo.KEY_LSHIFT) then
		pan_vue(mouse, cameraObject, camera:getFov())
	elseif mouse:isKeyDown(Neo.MOUSE_BUTTON_MIDDLE) then
		rotate_vue(mouse, cameraObject)
	end

	cameraObject:updateMatrix()
end
