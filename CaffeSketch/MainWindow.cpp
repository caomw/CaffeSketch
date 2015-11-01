#include "MainWindow.h"
#include "Classifier.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include "CustomWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionPredict, SIGNAL(triggered()), this, SLOT(onPredict()));
	
	glWidget = new GLWidget3D(this);

	thumbsList = new QListWidget(this);
	thumbsList->setFlow(QListView::TopToBottom);
	thumbsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	thumbsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	thumbsList->setFixedWidth(CustomWidget::WIDGET_WIDTH + thumbsList->frameWidth() * 2 + 18);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(thumbsList);
	layout->addWidget(glWidget);

	centralWidget()->setLayout(layout);
}

void MainWindow::clearList() {
	thumbsList->clear();
}

void MainWindow::addListItem(const QString& text, const QImage& image) {
	CustomWidget* customWidget = new CustomWidget(this, text, image);
	QListWidgetItem *item = new QListWidgetItem();
	item->setSizeHint(customWidget->size());
	thumbsList->addItem(item);
	thumbsList->setItemWidget(item, customWidget);
}

void MainWindow::onNew() {
	glWidget->newImage();
}

void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image file..."), "", tr("Image Files (*.jpg *.png *.bmp)"));
	if (filename.isEmpty()) return;

	glWidget->loadImage(filename);
}

void MainWindow::onPredict() {
	glWidget->predict();
}
