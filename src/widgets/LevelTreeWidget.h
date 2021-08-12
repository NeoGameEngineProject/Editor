#ifndef NEO_LEVELTREEWIDGET_H
#define NEO_LEVELTREEWIDGET_H

#include <QTreeWidget>
#include <Level.h>

namespace Neo 
{

class LevelTreeWidget : public QTreeWidget
{
	Q_OBJECT;
	std::shared_ptr<Level> m_level;
public:
	LevelTreeWidget (QWidget* parent);
	
	void setLevel(std::shared_ptr<Level> h);
	std::shared_ptr<Level> getLevel() const { return m_level; }
	
public slots:
	void levelChangedSlot();
	void selectionChangedSlot();
	
	void setSelectionList(const std::vector<ObjectHandle>& objects);
	void contextMenuSlot(const QPoint& pos);

signals:
	void levelChanged();
	void objectSelectionChanged(ObjectHandle);
	void objectSelectionListChanged(const std::vector<ObjectHandle>&);

protected:
	void dropEvent(QDropEvent* event) override;

private:
	void addObject(ObjectHandle object, QTreeWidgetItem* parent, bool onlyChildren = false);
	
	std::vector<ObjectHandle> m_selection;
};

}

#endif
