#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <mainwindow.h>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreferencesDialog(QWidget* parent, MainWindow::Configuration& config);
	~PreferencesDialog();

private:
	Ui::PreferencesDialog *ui;
	MainWindow::Configuration& m_config;
};

#endif
