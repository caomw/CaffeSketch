#include "CustomWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <QWidget>

//int CustomWidget::WIDTH = 200;
//int CustomWidget::HEIGHT = 200;

CustomWidget::CustomWidget(QWidget *parent, const QString& text, const QImage& image) : QLabel(parent) {
	this->setFixedSize(WIDGET_WIDTH, WIDGET_HEIGHT);

	QImage img = image;

	QPainter painter(&img);
	painter.setPen(Qt::blue);
	painter.setFont(QFont("Arial", 20));
	painter.drawText(image.rect(), Qt::AlignHCenter | Qt::AlignBottom, text);

	this->setPixmap(QPixmap::fromImage(img));
	this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}
