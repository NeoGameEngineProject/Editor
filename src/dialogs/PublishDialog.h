#ifndef PUBLISHDIALOG_H
#define PUBLISHDIALOG_H

#include <QDialog>

namespace Ui {
class PublishDialog;
}

class PublishDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PublishDialog(QWidget *parent = nullptr);
	~PublishDialog();

private:
	Ui::PublishDialog *ui;
};

#endif // PUBLISHDIALOG_H
