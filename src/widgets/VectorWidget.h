#ifndef NEO_VECTORWIDGET_H
#define NEO_VECTORWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>

#include <Object.h>

#include <Vector2.h>
#include <Vector3.h>
#include <Vector4.h>

namespace Neo 
{

class VectorWidgetBase : public QWidget
{
	Q_OBJECT;
public:
	VectorWidgetBase(QWidget* parent): QWidget(parent) 
	{
		setLayout(new QHBoxLayout());
	}
signals:
	void valueChanged();
};

template<typename T>
class VectorWidget : public VectorWidgetBase
{
public:
	VectorWidget(QWidget* parent, const char* unit = ""):
		VectorWidgetBase(parent)
	{
		buildWidgets(unit);
	}
	
	void setValue(const T& value)
	{
		constexpr int num = s_getComponents();
		if constexpr(num >= 1)
			m_fields[0]->setValue(value.x);
		
		if constexpr(num >= 2)
			m_fields[1]->setValue(value.y);
		
		if constexpr(num >= 3)
			m_fields[2]->setValue(value.z);
		
		if constexpr(num >= 4)
			m_fields[3]->setValue(value.w);
	}
	
	T value() const
	{
		T value;
		
		constexpr int num = s_getComponents();
		if constexpr(num >= 1)
			value.x = m_fields[0]->value();
		
		if constexpr(num >= 2)
			value.y = m_fields[1]->value();
		
		if constexpr(num >= 3)
			value.z = m_fields[2]->value();
		
		if constexpr(num >= 4)
			value.w = m_fields[3]->value();
		
		return value;
	}
	
	constexpr int getComponents()
	{
		return s_getComponents();
	}
	
private:
	void buildWidgets(const char* unit)
	{
		QSizePolicy size;
		auto layout = this->layout();
		const char* axes[] = {"X", "Y", "Z", "W"};
		
		size.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
		
		for(int i = 0; i < getComponents(); i++)
		{
			auto label = new QLabel(axes[i]);
			m_fields[i] = new QDoubleSpinBox(this);
			m_fields[i]->setSingleStep(0.1);
			m_fields[i]->setSuffix(QString(" ") + unit);
			m_fields[i]->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
			m_fields[i]->setDecimals(std::numeric_limits<float>::digits10);
			
			m_fields[i]->setSizePolicy(size);
			m_fields[i]->setMinimumWidth(16);
			
			label->setMaximumWidth(16);
		
			layout->addWidget(label);
			layout->addWidget(m_fields[i]);
			
			connect(m_fields[i], qOverload<double>(&QDoubleSpinBox::valueChanged), [this](double) { emit valueChanged(); });
		}
	}
	
	static constexpr int s_getComponents()
	{
		if constexpr(std::is_same<T, Neo::Vector2>::value)
		{
			return 2;
		}
		else if constexpr(std::is_same<T, Neo::Vector3>::value)
		{
			return 3;
		}
		else if constexpr(std::is_same<T, Neo::Vector4>::value)
		{
			return 4;
		}
		
		return -1;
	}
	
	static_assert(s_getComponents() != -1, "Unsupported type given!");
	static_assert(s_getComponents() > 0, "At least one value needs to be worked on!");
	static_assert(s_getComponents() <= 4, "At most 4 components are supported!");
	
	T m_value;
	QDoubleSpinBox* m_fields[s_getComponents()];
};

}

#endif
