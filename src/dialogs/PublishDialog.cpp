#include "PublishDialog.h"
#include "ui_PublishDialog.h"

PublishDialog::PublishDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PublishDialog)
{
	ui->setupUi(this);
}

PublishDialog::~PublishDialog()
{
	delete ui;
}
