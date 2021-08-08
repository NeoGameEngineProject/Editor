#include "ColorButton.h"

#include <QColorDialog>

using namespace Neo;

constexpr auto s_borderstyle = "border-style: outset; border-width: 1px; border-color: black; margin: 1px";

ColorButton::ColorButton(QWidget* parent):
	QPushButton(parent)
{
	setColor(Vector4(1, 1, 1, 1));
	connect(this, &QPushButton::clicked, [this]() {
		setColor(QColorDialog::getColor(m_color, parentWidget()));
	});
}

void ColorButton::setColor(const Vector4& c)
{
	m_color = QColor(c.x*255, c.y*255, c.z*255, c.w*255);
	setStyleSheet("background-color: " + m_color.name() + ";" + s_borderstyle);
	emit colorChanged(c);
}

void ColorButton::setColor(const QColor& c)
{
	m_color = c;
	setStyleSheet("background-color: " + m_color.name() + ";" + s_borderstyle);
	emit colorChanged(Vector4(c.red()/255.0f, 
				  c.green()/255.0f, 
				  c.blue()/255.0f, 
				  c.alpha()/255.0f));
}

