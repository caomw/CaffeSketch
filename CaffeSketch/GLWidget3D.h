#pragma once

#include <glew.h>
#include "Shader.h"
#include "Vertex.h"
#include <QGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include <vector>
#include <QImage>
#include "Camera.h"
#include "ShadowMapping.h"
#include "RenderManager.h"
#include "CGA.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;
class Classifier;
class MainWindow;

class GLWidget3D : public QGLWidget {
private:
	static enum{ VERTEX, NORMAL, COLOR, TOTAL_VBO_ID };

	MainWindow* mainWin;
	QImage sketch;
	QPoint lastPos;
	bool dragging;
	bool ctrlPressed;
	Classifier* classifier;
	std::vector<QImage> images;
	std::map<std::string, std::vector<cga::Grammar> > grammars;

	Camera camera;
	glm::vec3 light_dir;
	glm::mat4 light_mvpMatrix;
	RenderManager renderManager;
	cga::CGA system;

public:
	GLWidget3D(QWidget *parent);
	void drawLineTo(const QPoint &endPoint);
	void clearSketch();
	void clearGeometry();
	void loadImage(const QString& filename);
	void drawScene(int drawMode);
	void loadCGA(char* filename);
	void predict();
	void selectOption(int option_index);
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
	void initializeGL();
	void resizeGL(int width, int height);
	void paintEvent(QPaintEvent *event);

};

