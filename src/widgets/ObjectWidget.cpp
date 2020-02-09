#include "ObjectWidget.h"
#include "VectorWidget.h"
#include "ColorButton.h"

#include <Log.h>

#include <QFileDialog>
#include <QLineEdit>

using namespace Neo;

template<typename T>
class WidgetItem : public QTreeWidgetItem
{
public:
	WidgetItem(QTreeWidgetItem* parent, const QStringList& strings):
		QTreeWidgetItem(parent, strings) {}
};

ObjectWidget::ObjectWidget(QWidget* parent):
	QTreeWidget(parent)
{
	setColumnCount(2);
	setHeaderHidden(false);
	setHeaderLabels(QStringList() << tr("Property") << tr("Value"));
}

void ObjectWidget::setObject(ObjectHandle h)
{
	m_object = h;
	clear();

	if(h.empty())
	{
		setEnabled(false);
		return;
	}
	
	setEnabled(true);
	
	auto name = new QTreeWidgetItem(QStringList() << tr("Name"));
	auto active = new QTreeWidgetItem(QStringList() << tr("Active"));
	
	addTopLevelItem(name);
	addTopLevelItem(active);
	
	setItemWidget(name, 1, m_name = new QLineEdit(h->getName().str()));
	setItemWidget(active, 1, m_active = new QCheckBox());

	addTopLevelItem(createTransform(h));
	
	for(auto& behavior : h->getBehaviors())
		addTopLevelItem(createBehavior(behavior.get()));
	
	expandAll();
	updateObject(h);
	
	connect(m_name, &QLineEdit::editingFinished, [h, this] () mutable {
		emit requestNameChange(h, m_name->text());
	});
	
	connect(m_active, &QCheckBox::stateChanged, [h] (int state) mutable {
		h->setActive(state == Qt::Checked);
	});
}

QTreeWidgetItem* ObjectWidget::createTransform(ObjectHandle o)
{
	auto title = new QTreeWidgetItem(QStringList() << tr("Transform") << "");
	auto position = new QTreeWidgetItem(title, QStringList(tr("Position")));
	auto rotation = new QTreeWidgetItem(title, QStringList(tr("Rotation")));
	auto scale = new QTreeWidgetItem(title, QStringList(tr("Scale")));
	
	setItemWidget(position, 1, m_position = new VectorWidget<Neo::Vector3>(this, "m"));
	setItemWidget(rotation, 1, m_rotation = new VectorWidget<Neo::Vector3>(this, "°"));
	setItemWidget(scale, 1, m_scale = new VectorWidget<Neo::Vector3>(this));

	connect(m_position, &VectorWidgetBase::valueChanged, [o, this]() mutable {
		o->setPosition(m_position->value());
		o->updateMatrix();
	});
	
	connect(m_rotation, &VectorWidgetBase::valueChanged, [o, this]() mutable {
		Quaternion rot;
		rot.setFromAngles(m_rotation->value());
		o->setRotation(rot);
		o->updateMatrix();
	});
	
	connect(m_scale, &VectorWidgetBase::valueChanged, [o, this]() mutable {
		o->setScale(m_scale->value());
		o->updateMatrix();
	});
	
	return title;
}

QTreeWidgetItem* ObjectWidget::createBehavior(Behavior* b)
{
	auto title = new QTreeWidgetItem(QStringList() << b->getName() << "");
	for(auto prop : b->getProperties())
	{
		auto propItem = new QTreeWidgetItem(title, QStringList() << prop->getName().c_str());
		switch(prop->getType())
		{
		case BOOL: 
		{
			QCheckBox* widget;
			setItemWidget(propItem, 1, widget = new QCheckBox(this));
			
			connect(widget, qOverload<int>(&QCheckBox::stateChanged), [prop, b](int value) mutable {
				prop->set(value == Qt::CheckState::Checked);
				b->propertyChanged(prop);
			});
		}
		break;
			
		case INTEGER:
		{
			QSpinBox* widget;
			setItemWidget(propItem, 1, widget = new QSpinBox(this));
			
			widget->setSingleStep(1);
			widget->setRange(std::numeric_limits<int>::lowest(), std::numeric_limits<int>::max());
			
			connect(widget, qOverload<int>(&QSpinBox::valueChanged), [prop, b](int value) mutable {
				prop->set(value);
				b->propertyChanged(prop);
			});
		}
		break;
		
		case UNSIGNED_INTEGER:
		{
			QSpinBox* widget;
			setItemWidget(propItem, 1, widget = new QSpinBox(this));
			
			widget->setSingleStep(1);
			widget->setRange(std::numeric_limits<unsigned int>::lowest(), std::numeric_limits<unsigned int>::max());
			
			connect(widget, qOverload<int>(&QSpinBox::valueChanged), [prop, b](int value) mutable {
				prop->set(value);
				b->propertyChanged(prop);
			});
		}
		break;
		
		case FLOAT:
		{
			QDoubleSpinBox* widget;
			setItemWidget(propItem, 1, widget = new QDoubleSpinBox(this));
			
			widget->setSingleStep(0.1);
			widget->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
			widget->setDecimals(std::numeric_limits<float>::digits10);
			
			connect(widget, qOverload<double>(&QDoubleSpinBox::valueChanged), [prop, b](double value) mutable {
				prop->set(static_cast<float>(value));
				b->propertyChanged(prop);
			});
		}
		break;
		
		case VECTOR2:
		{
			VectorWidget<Neo::Vector2>* widget;
			setItemWidget(propItem, 1, widget = new VectorWidget<Neo::Vector2>(this));
			
			connect(widget, &VectorWidgetBase::valueChanged, [prop, widget, b]() mutable {
				prop->set(widget->value());
				b->propertyChanged(prop);
			});
		}
		break;
		
		case VECTOR3:
		{
			VectorWidget<Neo::Vector3>* widget;
			setItemWidget(propItem, 1, widget = new VectorWidget<Neo::Vector3>(this));
			
			connect(widget, &VectorWidgetBase::valueChanged, [prop, widget, b]() mutable {
				prop->set(widget->value());
				b->propertyChanged(prop);
			});
		}
		break;
		
		case VECTOR4: 
		{
			VectorWidget<Neo::Vector4>* widget;
			setItemWidget(propItem, 1, widget = new VectorWidget<Neo::Vector4>(this));
			
			connect(widget, &VectorWidgetBase::valueChanged, [prop, widget, b]() mutable {
				prop->set(widget->value());
				b->propertyChanged(prop);
			});
		}
		break;
		
		case COLOR:
		{
			ColorButton* widget;
			setItemWidget(propItem, 1, widget = new ColorButton(this));
			
			connect(widget, &ColorButton::colorChanged, [prop, b](const Vector4& color) mutable {
				prop->set(color);
				b->propertyChanged(prop);
			});
		}
		break;

		case PATH:
		{
			QFrame* frame = new QFrame(this);
			QLineEdit* widget = new QLineEdit(frame);
			QPushButton* button = new QPushButton("...", frame);

			frame->setLayout(new QHBoxLayout(frame));
			frame->layout()->addWidget(widget);
			frame->layout()->addWidget(button);
			frame->layout()->setSpacing(0);
			
			button->setMaximumWidth(20);

			setItemWidget(propItem, 1, frame);
			
			connect(widget, &QLineEdit::editingFinished, [prop, b, widget] () mutable {
				prop->set(widget->text().toStdString());
				b->propertyChanged(prop);
			});

			connect(button, &QPushButton::pressed, [prop, b, widget] () mutable {
				
				auto value = QFileDialog::getOpenFileName(widget, tr("Open File"), widget->text(), tr("All Files (*.*)"));

				if(value.isEmpty())
					return;

				widget->setText(value);
				prop->set(value.toStdString());
				b->propertyChanged(prop);
			});
		}
		break;
		
		case STRING:
		{
			QLineEdit* widget;
			setItemWidget(propItem, 1, widget = new QLineEdit(this));
			
			connect(widget, &QLineEdit::editingFinished, [prop, b, widget] () mutable {
				prop->set(widget->text().toStdString());
				b->propertyChanged(prop);
			});
		}
		break;
		
		default:
			setItemWidget(propItem, 1, new QLabel(tr("Unknown type!")));
		}
	}
		
	return title;
}


QTreeWidgetItem* ObjectWidget::findItemWithParent(const std::string& parent, const std::string& name)
{
	auto behaviorItems = findItems(QString(name.c_str()), Qt::MatchExactly | Qt::MatchRecursive, 0);
	for(auto item : behaviorItems)
	{
		if(item->parent()->text(0) == parent.c_str())
			return item;
	}
	
	return nullptr;
}

void ObjectWidget::updateObject(ObjectHandle h)
{
	m_position->setValue(h->getPosition());
	m_rotation->setValue(h->getRotation().getEulerAngles());
	m_scale->setValue(h->getScale());
	m_active->setChecked(h->isActive());
	
	for(auto& behavior : h->getBehaviors())
	{
		for(auto prop : behavior->getProperties())
		{
			auto propItem = findItemWithParent(behavior->getName(), prop->getName());
			if(!propItem)
			{
				LOG_WARNING("No property " << prop->getName() << " found!");
				continue;
			}
			
			QWidget* widget = itemWidget(propItem, 1);
			widget->blockSignals(true);
			
			switch(prop->getType())
			{
			case BOOL: reinterpret_cast<QCheckBox*>(widget)->setCheckState(prop->get<bool>() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked); break;
			case INTEGER: reinterpret_cast<QSpinBox*>(widget)->setValue(prop->get<int>()); break;
			case UNSIGNED_INTEGER: reinterpret_cast<QSpinBox*>(widget)->setValue(prop->get<unsigned int>()); break;
			case FLOAT: reinterpret_cast<QDoubleSpinBox*>(widget)->setValue(prop->get<float>()); break;
			case VECTOR2: reinterpret_cast<VectorWidget<Neo::Vector2>*>(widget)->setValue(prop->get<Vector2>()); break;
			case VECTOR3: reinterpret_cast<VectorWidget<Neo::Vector3>*>(widget)->setValue(prop->get<Vector3>()); break;
			case VECTOR4: reinterpret_cast<VectorWidget<Neo::Vector4>*>(widget)->setValue(prop->get<Vector4>()); break;
			case COLOR: reinterpret_cast<ColorButton*>(widget)->setColor(prop->get<Vector4>()); break;

			case PATH: widget = widget->layout()->itemAt(0)->widget();
			case STRING: reinterpret_cast<QLineEdit*>(widget)->setText(QString(prop->get<std::string>().c_str())); break;

			default: LOG_WARNING("Unknown property type for property " << prop->getName() << "!");
			}
			
			widget->blockSignals(false);
		}
	}
}

