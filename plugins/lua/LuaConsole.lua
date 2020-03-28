
PluginName = "Lua Console"
PluginVersion = "1.0.0"
PluginAuthor = "Yannick Pflanzer"
PluginLicense = "LGPL"

local QtCore = require 'qtcore'
local QtWidgets = require 'qtwidgets'

local tr = QtCore.QObject.tr;

local dock = QtWidgets.QDockWidget.new(tr "Lua Console")
dock:setObjectName("LuaConsoleDock")

local widget = QtWidgets.QWidget()
local dockLayout = QtWidgets.QVBoxLayout.new()
local textOutput = QtWidgets.QTextEdit.new()
local lineEdit = QtWidgets.QLineEdit.new()

print = function(...)
	local parm = {...}
	local str = ""

	for i, v in ipairs(parm) do
		str = str .. tostring(v) .. "\t"
	end

	textOutput:append(str)
end

lineEdit:connect('2returnPressed()', function(self)
	print("> " .. self:text():toStdString())
	local f, err = loadstring(self:text():toStdString());

	if f == nil then
		print(tr "Error:", err)
	else
		local success, msg = pcall(f)
		if not success then
			print(tr "Error:", msg)
		end
	end
	self:setText("")
end)

textOutput:setReadOnly(true)

dockLayout:addWidget(textOutput)
dockLayout:addWidget(lineEdit)
widget:setLayout(dockLayout)
dock:setWidget(widget)

AddDockWidget(QtWidgets.BottomDockWidgetArea, dock)
