#include "LevelTreeWidget.h"
#include <Log.h>

#include <QTreeWidgetItem>
#include <QAction>
#include <QMenu>
#include <QDropEvent>

using namespace Neo;

class ObjectItem : public QTreeWidgetItem
{
	ObjectHandle m_object;
public:
	ObjectItem(QTreeWidgetItem* parent, const QStringList& strings, ObjectHandle object):
		QTreeWidgetItem(parent, strings),
		m_object(object) {}

	ObjectHandle getObject() const { return m_object; }
};

LevelTreeWidget::LevelTreeWidget(QWidget* parent):
	QTreeWidget(parent)
{
	setColumnCount(1);
	setHeaderHidden(true);
	setSelectionBehavior(SelectionBehavior::SelectRows);
	setSelectionMode(SelectionMode::ExtendedSelection);

	setContextMenuPolicy(Qt::CustomContextMenu);
	
	connect(this, &QTreeWidget::itemSelectionChanged, this, &LevelTreeWidget::selectionChangedSlot);
	connect(this, &QTreeWidget::customContextMenuRequested, this, &LevelTreeWidget::contextMenuSlot);
}

void LevelTreeWidget::addObject(ObjectHandle object, QTreeWidgetItem* parent, bool onlyChildren)
{
	QTreeWidgetItem* item = (onlyChildren ? parent : new ObjectItem(parent, QStringList(object->getName().str()), object));

	for(auto& child : object->getChildren())
	{
		addObject(child, item, false);
	}
}

void LevelTreeWidget::levelChangedSlot()
{
	if(m_level == nullptr)
		return;
	
	clear();
	addObject(m_level->getRoot(), invisibleRootItem(), true);
}

void LevelTreeWidget::setLevel(std::shared_ptr<Level> h)
{
	m_level = h;
	levelChangedSlot();
}

void LevelTreeWidget::selectionChangedSlot()
{
	if(selectedItems().empty())
		return;
	
	auto item = dynamic_cast<ObjectItem*>(this->selectedItems()[0]);
	if(!item)
		return;
	
	emit objectSelectionChanged(item->getObject());
	
	m_selection.clear();
	m_selection.reserve(selectedItems().size());
	for(size_t i = 0; i < selectedItems().size(); i++)
	{
		auto item = dynamic_cast<ObjectItem*>(selectedItems()[i]);
		if(item)
			m_selection.push_back(item->getObject());
	}
	
	emit objectSelectionListChanged(m_selection);
}

void LevelTreeWidget::setSelectionList(const std::vector<ObjectHandle>& objects)
{
	if(objects.empty())
	{
		blockSignals(true);
		clearSelection();
		m_selection.clear();
		blockSignals(false);

		emit objectSelectionChanged(ObjectHandle());
		return;
	}
	
	blockSignals(true);
	m_selection = objects;
	
	clearSelection();
	
	for(size_t i = 0; i < topLevelItemCount(); i++)
	{
		auto item = dynamic_cast<ObjectItem*>(topLevelItem(i));
		if(item && std::find(objects.begin(), objects.end(), item->getObject()) != objects.end())
		{
			item->setSelected(true);
			scrollToItem(item);
		}
	}
	
	blockSignals(false);
	
	emit objectSelectionChanged(objects[0]);
}

void LevelTreeWidget::contextMenuSlot(const QPoint& pos)
{
	QTreeWidgetItem* nd = itemAt(pos);
	if(!nd)
		return;

	const std::string label = nd->text(0).toStdString();
	auto obj = m_level->find(label.c_str());
	if(obj.empty())
		return;

	QMenu menu(this);

	auto* camera = obj->getBehavior<CameraBehavior>();
	if(camera)
	{
		QAction* newAct = new QAction(tr("Set as Main Camera"), this);
		newAct->setStatusTip(tr("Set this camera as the default camera for the level."));
	
		connect(newAct, &QAction::triggered, [this, label]() {
			m_level->setMainCameraName(label.c_str());
		});
		
		menu.addAction(newAct);
	}

	QPoint pt(pos);
	menu.exec(mapToGlobal(pos));
}

void LevelTreeWidget::dropEvent(QDropEvent* event)
{
	auto items = QTreeWidget::selectedItems();

	// First, execute drop
	QTreeWidget::dropEvent(event);

	// Update scene to reflect new parent-child relationships
	LOG_DEBUG("Dropping items");
	for(auto* item : items)
	{
		LOG_DEBUG("Dropping: " << item->text(0).toStdString());
		auto object = static_cast<ObjectItem*>(item)->getObject();
		
		auto* newParentItem = item->parent();
		if(newParentItem == nullptr)
		{
			object->setParent(m_level->getRoot());
			object->updateMatrix();
			continue;
		}

		auto parentObject = m_level->find(newParentItem->text(0).toUtf8().data());
		if(parentObject.empty())
			LOG_WARNING("Parent not found!");

		object->setParent(parentObject);
		object->updateMatrix();
	}
	
	//LOG_INFO(QTreeWidget:)
	//auto* source = dynamic_cast<ObjectItem*>(event->source());
	//if(!source)
	//	return;

	//LOG_DEBUG("Dropped " << source->getObject()->getName());
}
