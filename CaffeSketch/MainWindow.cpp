#include "MainWindow.h"
#include "Classifier.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include "CustomWidget.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);

	connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(onNew()));
	connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(onOpen()));
	connect(ui.actionOpenCGA, SIGNAL(triggered()), this, SLOT(onOpenCGA()));
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

void MainWindow::keyPressEvent(QKeyEvent* e) {
	glWidget->keyPressEvent(e);
}

void MainWindow::keyReleaseEvent(QKeyEvent* e) {
	glWidget->keyReleaseEvent(e);
}

void MainWindow::clearList() {
	thumbsList->clear();
}

/**
 * Add an option to the list widget.
 *
 * @param text			probability that is shown on this item widget
 * @param image			image that is shown on this item widget
 * @param option_index	the index of this option (This index is not the index of the ordered options, but the index of the options in the original order.)
 */
void MainWindow::addListItem(const QString& text, const QImage& image, int option_index) {
	CustomWidget* customWidget = new CustomWidget(this, text, image, option_index);
	QListWidgetItem *item = new QListWidgetItem();
	item->setSizeHint(customWidget->size());
	thumbsList->addItem(item);
	thumbsList->setItemWidget(item, customWidget);
}

/**
 * This is called when the user clicks [File] -> [New].
 */
void MainWindow::onNew() {
	glWidget->clearSketch();
}

/**
 * This is called when the user clickes [File] -> [Open]
 */
void MainWindow::onOpen() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open Image file..."), "", tr("Image Files (*.jpg *.png *.bmp)"));
	if (filename.isEmpty()) return;

	glWidget->loadImage(filename);
}

/**
* This is called when the user clickes [File] -> [Open CGA]
*/
void MainWindow::onOpenCGA() {
	QString filename = QFileDialog::getOpenFileName(this, tr("Open CGA file..."), "", tr("CGA Files (*.xml)"));
	if (filename.isEmpty()) return;

	glWidget->loadCGA(filename.toUtf8().data());
}

/**
* This is called when the user clickes [Tool] -> [Predict]
*/
void MainWindow::onPredict() {
	glWidget->predict();
}
