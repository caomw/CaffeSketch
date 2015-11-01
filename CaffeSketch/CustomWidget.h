#pragma once

#include <QLabel>
#include <QImage>
#include <QString>

class CustomWidget : public QLabel {
public:
	static const int WIDGET_WIDTH = 200;
	static const int WIDGET_HEIGHT = 200;
	static const int IMAGE_WIDTH = 180;
	static const int IMAGE_HEIGHT = 180;

public:
	CustomWidget(QWidget *parent, const QString& text, const QImage& image);	
};

