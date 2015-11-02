#include <iostream>
#include "GLWidget3D.h"
#include "MainWindow.h"
#include <GL/GLU.h>
#include "Classifier.h"
#include <QDir>
#include <QFileInfoList>
#include "CustomWidget.h"
#include "OBJLoader.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "GrammarParser.h"
#include "Rectangle.h"
#include "GLUtils.h"

GLWidget3D::GLWidget3D(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers), parent) {
	mainWin = (MainWindow*)parent;
	dragging = false;
	ctrlPressed = false;

	// これがないと、QPainterによって、OpenGLによる描画がクリアされてしまう
	setAutoFillBackground(false);

	// 光源位置をセット
	// ShadowMappingは平行光源を使っている。この位置から原点方向を平行光源の方向とする。
	light_dir = glm::normalize(glm::vec3(-4, -5, -8));

	// シャドウマップ用のmodel/view/projection行列を作成
	glm::mat4 light_pMatrix = glm::ortho<float>(-100, 100, -100, 100, 0.1, 200);
	glm::mat4 light_mvMatrix = glm::lookAt(-light_dir * 50.0f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	light_mvpMatrix = light_pMatrix * light_mvMatrix;


	classifier = new Classifier("../models/buildings/deploy.prototxt",
		"../models/buildings/train.caffemodel",
		"../models/buildings/mean.binaryproto",
		"../models/buildings/words.txt");

	{
		QStringList filters;
		filters << "*.png" << "*.jpg" << "*.bmp";
		QFileInfoList fileInfoList = QDir("../thumbs/buildings/").entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
		for (int i = 0; i < fileInfoList.size(); ++i) {
			images.push_back(QImage(fileInfoList[i].absoluteFilePath()).scaled(CustomWidget::IMAGE_WIDTH, CustomWidget::IMAGE_HEIGHT));
		}
	}

	{
		QStringList filters;
		filters << "*.xml";
		QFileInfoList fileInfoList = QDir("../cga/building/").entryInfoList(filters, QDir::Files | QDir::NoDotAndDotDot);
		for (int i = 0; i < fileInfoList.size(); ++i) {
			cga::Grammar grammar;
			cga::parseGrammar(fileInfoList[i].absoluteFilePath().toUtf8().constData(), grammar);
			grammars["building"].push_back(grammar);
		}
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

/**
 * Clear the canvas.
 */
void GLWidget3D::clearSketch() {
	sketch.fill(qRgba(255, 255, 255, 255));

	update();
}

void GLWidget3D::clearGeometry() {
	renderManager.removeObjects();
	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);
	update();
}

/**
 * Load a sketch image from a file, and display options order by their probabilities.
 * This is for test usage.
 */
void GLWidget3D::loadImage(const QString& filename) {
	QImage newImage;
	newImage.load(filename);
	newImage = newImage.scaled(width(), height());

	QPainter painter(&sketch);
	painter.drawImage(0, 0, newImage);

	predict();
	// predict function calls update(), so we do not need to call it twice.
	//update();
}

/**
* Draw the scene.
*/
void GLWidget3D::drawScene(int drawMode) {
	if (drawMode == 0) {
		glUniform1i(glGetUniformLocation(renderManager.program, "depthComputation"), 0);
	}
	else {
		glUniform1i(glGetUniformLocation(renderManager.program, "depthComputation"), 1);
	}

	renderManager.renderAll();
}

/**
 * Load a grammar from a file and generate a 3d geometry.
 * This is only for test usage.
 */
void GLWidget3D::loadCGA(char* filename) {
	renderManager.removeObjects();

	float object_width = 16.0f;
	float object_height = 8.0f;

	{ // for parthenon
		cga::Rectangle* start = new cga::Rectangle("Start", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(-object_width*0.5f, -object_height*0.5f, 0)), glm::mat4(), object_width, object_height, glm::vec3(1, 1, 1));
		system.stack.push_back(boost::shared_ptr<cga::Shape>(start));
	}

	try {
		cga::Grammar grammar;
		cga::parseGrammar(filename, grammar);
		system.randomParamValues(grammar);
		system.derive(grammar);
		system.generateGeometry(&renderManager);
		renderManager.centerObjects();
	}
	catch (const std::string& ex) {
		std::cout << "ERROR:" << std::endl << ex << std::endl;
	}
	catch (const char* ex) {
		std::cout << "ERROR:" << std::endl << ex << std::endl;
	}

	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

	updateGL();
}

/**
 * Use the sketch as an input to the pretrained network, and obtain the probabilities as output.
 * Then, display the options ordered by the probabilities.
 */
void GLWidget3D::predict() {
	QImage swapped = sketch.scaled(256, 256).rgbSwapped();
	cv::Mat img = cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine()).clone();

	std::vector<Prediction> predictions = classifier->Classify(img, 10);
	mainWin->clearList();
	QPainter painter(&sketch);
	for (size_t i = 0; i < predictions.size(); ++i) {
		Prediction p = predictions[i];
		mainWin->addListItem(QString::number(p.second, 'f', 3), images[p.first], p.first);
	}

	update();
}

/**
 * This function is called when an option is clicked by the user.
 * The corresponding grammar is used to generate a 3d geometry and display it.
 */
void GLWidget3D::selectOption(int option_index) {
	renderManager.removeObjects();

	float object_width = 16.0f;
	float object_height = 8.0f;

	{ // lot
		cga::Rectangle* start = new cga::Rectangle("Start", glm::translate(glm::rotate(glm::mat4(), -3.141592f * 0.5f, glm::vec3(1, 0, 0)), glm::vec3(-object_width*0.5f, -object_height*0.5f, 0)), glm::mat4(), object_width, object_height, glm::vec3(1, 1, 1));
		system.stack.push_back(boost::shared_ptr<cga::Shape>(start));
	}

	cga::Grammar g = grammars["building"][option_index];

	try {
		system.derive(g);
		system.generateGeometry(&renderManager);
		renderManager.centerObjects();
	}
	catch (const std::string& ex) {
		std::cout << "ERROR:" << std::endl << ex << std::endl;
	}
	catch (const char* ex) {
		std::cout << "ERROR:" << std::endl << ex << std::endl;
	}

	renderManager.updateShadowMap(this, light_dir, light_mvpMatrix);

	update();
}

void GLWidget3D::keyPressEvent(QKeyEvent *e) {
	ctrlPressed = false;

	switch (e->key()) {
	case Qt::Key_Control:
		ctrlPressed = true;
		break;
	case Qt::Key_Delete:
		clearGeometry();
		break;
	default:
		break;
	}
}

void GLWidget3D::keyReleaseEvent(QKeyEvent* e) {
	ctrlPressed = false;
}

/**
 * This event handler is called when the mouse press events occur.
 */
void GLWidget3D::mousePressEvent(QMouseEvent *e) {
	if (ctrlPressed) { // move camera
		camera.mousePress(e->x(), e->y());
	}
	else {
		lastPos = e->pos();
		dragging = true;
	}
}

/**
 * This event handler is called when the mouse release events occur.
 */
void GLWidget3D::mouseReleaseEvent(QMouseEvent *e) {
	if (ctrlPressed) {
	}
	else {
		dragging = false;
		predict();
	}
}

/**
 * This event handler is called when the mouse move events occur.
 */
void GLWidget3D::mouseMoveEvent(QMouseEvent *e) {
	if (ctrlPressed) {
		if (e->buttons() & Qt::LeftButton) { // Rotate
			camera.rotate(e->x(), e->y());
		}
		else if (e->buttons() & Qt::MidButton) { // Move
			camera.move(e->x(), e->y());
		}
		else if (e->buttons() & Qt::RightButton) { // Zoom
			camera.zoom(e->x(), e->y());
		}
		clearSketch();
	}
	else {
		drawLineTo(e->pos());
	}

	update();
}

/**
 * This function is called once before the first call to paintGL() or resizeGL().
 */
void GLWidget3D::initializeGL() {
	renderManager.init("../shaders/vertex.glsl", "../shaders/geometry.glsl", "../shaders/fragment.glsl", false);
	renderManager.renderingMode = RenderManager::RENDERING_MODE_REGULAR;

	//glClearColor(1, 1, 1, 0.0);
	glClearColor(0.9, 0.9, 0.9, 0.0);

	system.modelMat = glm::rotate(glm::mat4(), -3.1415926f * 0.5f, glm::vec3(1, 0, 0));

	sketch = QImage(this->width(), this->height(), QImage::Format_RGB888);
	sketch.fill(qRgba(255, 255, 255, 255));
}

/**
 * This function is called whenever the widget has been resized.
 */
void GLWidget3D::resizeGL(int width, int height) {
	height = height?height:1;

	glViewport(0, 0, (GLint)width, (GLint)height );
	camera.updatePMatrix(width, height);
	renderManager.resize(width, height);

	QImage newImage(width, height, QImage::Format_RGB888);
	newImage.fill(qRgba(255, 255, 255, 255));
	QPainter painter(&newImage);

	painter.drawImage(0, 0, sketch);
	sketch = newImage;
}

void GLWidget3D::paintEvent(QPaintEvent *event) {
	// OpenGLで描画
	makeCurrent();

	glUseProgram(renderManager.program);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);



	// Model view projection行列をシェーダに渡す
	glUniformMatrix4fv(glGetUniformLocation(renderManager.program, "mvpMatrix"), 1, GL_FALSE, &camera.mvpMatrix[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(renderManager.program, "mvMatrix"), 1, GL_FALSE, &camera.mvMatrix[0][0]);

	// pass the light direction to the shader
	//glUniform1fv(glGetUniformLocation(renderManager.program, "lightDir"), 3, &light_dir[0]);
	glUniform3f(glGetUniformLocation(renderManager.program, "lightDir"), light_dir.x, light_dir.y, light_dir.z);

	drawScene(0);





	// OpenGLの設定を元に戻す
	glShadeModel(GL_FLAT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	// QPainterで描画
	QPainter painter(this);
	painter.setOpacity(0.5);
	painter.drawImage(0, 0, sketch);
	painter.end();

	glEnable(GL_DEPTH_TEST);
}



