#include "QtInputContext.h"
#include <NeoEngine.h>
#include <algorithm>
#include <limits>
#include <cstring>
#include <QApplication>

#ifdef max // Defined by the Windows SDK
#undef max
#endif

using namespace Neo;

void QtInputContext::handleInput()
{
	Keyboard& kbd = getKeyboard();
	Mouse& mouse = getMouse();

	flush();
	
	QCoreApplication::processEvents();
	
	// If the mouse is locked we need to ensure that the direction is reset when no movement happened.
	if(isMouseRelative())
		mouse.setDirection(Vector2(0,0));

	if(!isMouseRelative()) // Only calculate direction from position when it is needed
		mouse.flushDirection();
}

void QtInputContext::setMouseRelative(bool value)
{
	
}

