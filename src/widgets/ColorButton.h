#ifndef NEO_COLORWIDGET_H
#define NEO_COLORWIDGET_H

#include <QPushButton>
#include <Vector4.h>

namespace Neo 
{

class ColorButton : public QPushButton
{
	Q_OBJECT;
public:
	ColorButton(QWidget* parent);

public slots:
	void setColor(const Vector4& c);
	void setColor(const QColor& c);
signals:
	void colorChanged(Vector4);
	
private:
	QColor m_color;
};

}

#endif
