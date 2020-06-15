#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QVector>

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
private:
	GLuint m_program;
	GLuint m_vao;
	GLuint m_vbo;

	QVector<GLfloat> m_vertices;

	const char *vshader_src =
		"#version 400 core\n"
		"layout(location = 0) in vec4 position;\n"
		"void main(){\n"
		"gl_Position = position;\n"
		"}\n";

	const char *fshader_src =
		"#version 400 core\n"
		"out vec4 color;\n"
		"void main(){\n"
		"color = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"}\n";

public:
	GLWidget(QWidget *parent = nullptr) :QOpenGLWidget(parent)
	{
		// 三角形の頂点座標
		m_vertices 
			<< -0.5f << -0.5f << 0.0f
			<< 0.0f << 0.5f << 0.0f
			<< 0.5f << -0.5f << 0.0f;
	}

	void initializeGL(void);

	void paintGL(void);
};
#endif // GLWIDGET_T