#include <iostream>
#include "GLWidget3D.h"
#include "MainWindow.h"
#include <GL/GLU.h>
#include "Classifier.h"
#include <QDir>
#include <QFileInfoList>
#include "CustomWidget.h"

GLWidget3D::GLWidget3D(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
	dragging = false;

	mainWin = (MainWindow*)parent;

	// これがないと、QPainterによって、OpenGLによる描画がクリアされてしまう
	setAutoFillBackground(false);

	classifier = new Classifier("../models/buildings/deploy.prototxt",
		"../models/buildings/train.caffemodel",
		"../models/buildings/mean.binaryproto",
		"../models/buildings/words.txt");

	QStringList filters;
	filters << "*.png" << "*.jpg" << "*.bmp";
	QFileInfoList fileInfoList = QDir("../thumbs/buildings/").entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
	for (int i = 0; i < fileInfoList.size(); ++i) {
		images.push_back(QImage(fileInfoList[i].absoluteFilePath()).scaled(CustomWidget::IMAGE_WIDTH, CustomWidget::IMAGE_HEIGHT));
	}
}

void GLWidget3D::drawLineTo(const QPoint &endPoint) {
	QPoint pt1(lastPos.x(), lastPos.y());
	QPoint pt2(endPoint.x(), endPoint.y());

	QPainter painter(&sketch);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::HighQualityAntialiasing);

	painter.drawLine(pt1, pt2);

	lastPos = endPoint;
}

void GLWidget3D::newImage() {
	sketch.fill(qRgba(255, 255, 255, 255));

	update();
}

void GLWidget3D::loadImage(const QString& filename) {
	QImage newImage;
	newImage.load(filename);
	newImage = newImage.scaled(width(), height());

	QPainter painter(&sketch);
	painter.drawImage(0, 0, newImage);

	predict();
	//update();
}

void GLWidget3D::predict() {
	QImage swapped = sketch.scaled(256, 256).rgbSwapped();
	cv::Mat img = cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();

	std::vector<Prediction> predictions = classifier->Classify(img, 10);
	mainWin->clearList();
	QPainter painter(&sketch);
	for (size_t i = 0; i < predictions.size(); ++i) {
		Prediction p = predictions[i];
		mainWin->addListItem(QString::number(p.second, 'f', 3), images[p.first]);

		std::cout << std::fixed << std::setprecision(3) << p.second << " - " << p.first << std::endl;
	}

	update();
}

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	lastPos = e->pos();
	dragging = true;
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	dragging = false;

	predict();
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	drawLineTo(e->pos());

	update();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL() {
	glClearColor(1, 1, 1, 0.0);

	sketch = QImage(this->width(), this->height(), QImage::Format_RGB888);
	sketch.fill(qRgba(255, 255, 255, 255));
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
	height = height?height:1;

	glViewport(0, 0, (GLint)width, (GLint)height );

	QImage newImage(width, height, QImage::Format_RGB888);
	newImage.fill(qRgba(255, 255, 255, 255));
	QPainter painter(&newImage);

	painter.drawImage(0, 0, sketch);
	sketch = newImage;
}

void GLWidget3D::paintEvent(QPaintEvent *event) {
	// OpenGLで描画
	makeCurrent();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);




	// OpenGLの設定を元に戻す
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// QPainterで描画
	QPainter painter(this);
	//painter.setOpacity(0.5);
	painter.drawImage(0, 0, sketch);
	painter.end();

	glEnable(GL_DEPTH_TEST);
}



