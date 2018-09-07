#ifndef NEO_EDITORWIDGET_H
#define NEO_EDITORWIDGET_H

#include "LevelWidget.h"

namespace Neo 
{

enum EDITOR_MODE
{
	EDITOR_TRANSLATE,
	EDITOR_ROTATE,
	EDITOR_SCALE
};

class EditorWidget : public LevelWidget
{
	Q_OBJECT;
public:
	EditorWidget(QWidget* parent);

	void setMode(EDITOR_MODE mode) { m_mode = mode; } 
	
protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

public slots:
	void setSelection(const std::vector<ObjectHandle>& selection);
	
signals:
	void objectSelectionChanged(ObjectHandle);
	void objectSelectionListChanged(const std::vector<ObjectHandle>&);
	void objectChanged(ObjectHandle);
	
private:
	void updateImGuiInput();
	Vector3 selectionCenter();
	
	std::vector<ObjectHandle> m_selection;
	EDITOR_MODE m_mode = EDITOR_TRANSLATE;
};

}

#endif
