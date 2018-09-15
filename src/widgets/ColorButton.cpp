#include "ColorButton.h"

#include <QColorDialog>

using namespace Neo;

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
	setStyleSheet("background-color: " + m_color.name());
	emit colorChanged(c);
}

void ColorButton::setColor(const QColor& c)
{
	m_color = c;
	setStyleSheet("background-color: " + m_color.name());
	emit colorChanged(Vector4(c.red()/256.0f, 
				  c.green()/256.0f, 
				  c.blue()/256.0f, 
				  c.alpha()/256.0f));
}

