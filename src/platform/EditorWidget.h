#ifndef NEO_EDITORWIDGET_H
#define NEO_EDITORWIDGET_H

#include "LevelWidget.h"
#include <Texture.h>

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
	const std::vector<ObjectHandle>& getSelection() const { return m_selection; }
	
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
	void updateDPI();
	void updateImGuiInput();
	Vector3 selectionCenter();

	float m_scaledWidth = 0, m_scaledHeight = 0, m_dpiScale = 1;
	std::vector<ObjectHandle> m_selection;
	EDITOR_MODE m_mode = EDITOR_TRANSLATE;

	// Contains the textures belonging to the camera/light/etc. objects
	Texture* m_objectTextures[3];
};

}

#endif
