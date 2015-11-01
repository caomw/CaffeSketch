#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include "GLWidget3D.h"
#include <QListWidget>

class MainWindow : public QMainWindow {
	Q_OBJECT

private:
	Ui::MainWindowClass ui;
	GLWidget3D* glWidget;
	QListWidget *thumbsList;

public:
	MainWindow(QWidget *parent = 0);
	void clearList();
	void addListItem(const QString& text, const QImage& image);

public slots:
	void onNew();
	void onOpen();
	void onPredict();
};

#endif // MAINWINDOW_H
