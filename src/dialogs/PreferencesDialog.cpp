#include "PreferencesDialog.h"
#include "ui_PreferencesDialog.h"

#include <mainwindow.h>

#include <plugins/PluginHost.h>
#include <Log.h>

PreferencesDialog::PreferencesDialog(QWidget* parent, MainWindow::Configuration& config):
	QDialog(parent),
	ui(new Ui::PreferencesDialog),
	m_config(config)
{
	ui->setupUi(this);
	ui->themeCombo->setCurrentText(config.theme.c_str());

	connect(ui->themeCombo, &QComboBox::currentTextChanged, [this](const QString& text) {
		m_config.theme = text.toStdString();
	});
}

PreferencesDialog::~PreferencesDialog()
{
	delete ui;
}
