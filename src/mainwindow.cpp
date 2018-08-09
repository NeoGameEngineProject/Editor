#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qsurface.h>

#include "platform/OpenGLWidget.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	//ui->centralWidget->
	//ui->MainView->addWidget(new Neo::OpenGLWidget(this));
}

MainWindow::~MainWindow()
{
	delete ui;
}
