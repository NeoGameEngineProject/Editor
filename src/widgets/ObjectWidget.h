#ifndef NEO_OBJECTWIDGET_H
#define NEO_OBJECTWIDGET_H

#include <QTreeWidget>
#include <Object.h>
#include "VectorWidget.h"
#include <QCheckBox>

namespace Neo 
{

class ObjectWidget : public QTreeWidget
{
	Q_OBJECT;
	ObjectHandle m_object;
public:
	ObjectWidget (QWidget* parent);
	
	ObjectHandle getObject() const { return m_object; }
	
public slots:
	void setObject(ObjectHandle h);
	void updateObject(ObjectHandle h);
	
signals:
	void objectChanged(ObjectHandle);
	void requestNameChange(ObjectHandle, QString);
	
	void beginUndoableChange();
	void endUndoableChange();

private:
	QTreeWidgetItem* createTransform(ObjectHandle o);
	QTreeWidgetItem* createBehavior(Behavior* b);
	QTreeWidgetItem* createCustomBehavior(Behavior* b, QTreeWidgetItem* parent);
	QTreeWidgetItem* findItemWithParent(const std::string& parent, const std::string& name);
	
	VectorWidget<Vector3>* m_position, *m_rotation, *m_scale;
	QLineEdit* m_name;
	QCheckBox* m_active;
};

}

#endif
