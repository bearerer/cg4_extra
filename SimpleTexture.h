/** 
	This class in its current form is only used to be able to identify a QImage
	in a list by assigning a name to every SimpleTexture created using this class.
*/

#pragma once
#include <QGLWidget>
#include <iostream>


class SimpleTexture{
public:
	SimpleTexture(QString _name, QImage _img);
	~SimpleTexture(void);

	QString name;
	QImage img;
};

