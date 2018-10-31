#include "LevelTreeWidget.h"
#include <Log.h>

#include <QTreeWidgetItem>

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
	
	connect(this, &QTreeWidget::itemSelectionChanged, this, &LevelTreeWidget::selectionChangedSlot);
}

void LevelTreeWidget::addObject(ObjectHandle object, QTreeWidgetItem* parent, bool onlyChildren)
{
	QTreeWidgetItem* item = (onlyChildren ? parent : new ObjectItem(parent, QStringList(object->getName().str()), object));
	for(auto& child : object->getChildren())
	{
		if(child->isActive())
			addObject(child, item);
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
