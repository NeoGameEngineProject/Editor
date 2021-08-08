#ifndef __UNDO_ACTIONS_H__
#define __UNDO_ACTIONS_H__

#include <QUndoStack>
#include <BinaryScene.h>
#include "platform/LevelWidget.h"

struct FullSceneEditCommand : public QUndoCommand
{
	FullSceneEditCommand(Neo::LevelWidget& wdg):
		m_levelWidget(wdg)
	{ }

	void loadLevel(std::stringstream& ss)
	{
		auto& level = *m_levelWidget.getLevel();

		// clearObjects also clears the current camera
		auto* cam = level.getCurrentCamera();
		level.clearObjects();
		
		// Load scene from buffer
		Neo::BinaryScene scene;
		scene.load(level, ss);

		level.setCurrentCamera(cam);

		// Re-initialize level!
		m_levelWidget.setNeedsInit(true);
	}

	void undo() override
	{
		loadLevel(m_undo);
	}

	void redo() override
	{
		// Redo is being called always on creation
		// so we need to ignore the first call!
		if(m_redoCounter++ > 0)
			loadLevel(m_redo);
	}

	void setUndo()
	{
		Neo::BinaryScene scene;
		scene.save(*m_levelWidget.getLevel(), m_undo);
	}

	void setRedo()
	{
		Neo::BinaryScene scene;
		scene.save(*m_levelWidget.getLevel(), m_redo);
	}

	std::stringstream m_undo, m_redo;
	Neo::LevelWidget& m_levelWidget;
	int m_redoCounter = 0;
};

#endif
