#include "ObjectWidget.h"
#include "VectorWidget.h"
#include <Log.h>

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
	
	auto behaviors = new QTreeWidgetItem(QStringList(tr("Behaviors")));
	addTopLevelItem(behaviors);
	
	for(auto& behavior : h->getBehaviors())
		behaviors->addChild(createBehavior(behavior.get()));
	
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
	
	return title;
}

void ObjectWidget::updateObject(ObjectHandle h)
{
	m_position->setValue(h->getPosition());
	m_rotation->setValue(h->getRotation().getEulerAngles());
	m_scale->setValue(h->getScale());
	m_active->setChecked(h->isActive());
	
	// TODO Behaviors
}

