#include "PluginDialog.h"
#include "ui_PluginDialog.h"

#include <plugins/PluginHost.h>
#include <Log.h>

PluginDialog::PluginDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PluginDialog)
{
	ui->setupUi(this);

	auto& plugins = PluginHost::get();
	ui->tableWidget->setRowCount(plugins.getPlugins().size());

	int i = 0;
	for(auto& p : plugins.getPlugins())
	{
		const auto name = p.getGlobalString("PluginName");
		const auto author = p.getGlobalString("PluginAuthor");
		const auto license = p.getGlobalString("PluginLicense");
		const auto version = p.getGlobalString("PluginVersion");

		int j = 0;
		ui->tableWidget->setItem(i, j++, new QTableWidgetItem(name.c_str()));
		ui->tableWidget->setItem(i, j++, new QTableWidgetItem(author.c_str()));
		ui->tableWidget->setItem(i, j++, new QTableWidgetItem(license.c_str()));
		ui->tableWidget->setItem(i, j++, new QTableWidgetItem(version.c_str()));

		i++;
	}
}

PluginDialog::~PluginDialog()
{
	delete ui;
}
