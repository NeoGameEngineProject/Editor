#ifndef NEO_EDITORWIDGET_H
#define NEO_EDITORWIDGET_H

#include "LevelWidget.h"

namespace Neo 
{

class EditorWidget : public LevelWidget
{
public:
	EditorWidget(QWidget* parent);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	
	void updateImGuiInput();
};

}

#endif
