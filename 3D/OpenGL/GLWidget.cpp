#include "gl/glew.h"
#include "GLWidget.h"

void GLWidget::initializeGL(void)
{
	glewInit();
	initializeOpenGLFunctions(); // 初期化
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_program = glCreateProgram();
	// vertex shaderの作成
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vshader_src, 0);
	glCompileShader(vshader);

	// fragment shaderの作成
	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fshader_src, 0);
	glCompileShader(fshader);

	// プログラムにアタッチ
	glAttachShader(m_program, vshader);
	glAttachShader(m_program, fshader);

	// リンク
	glLinkProgram(m_program);

	// 削除
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	// vaoの作成
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	// vdoの作成
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(GLfloat),
				 m_vertices.constData(), GL_STATIC_DRAW);

	// vertex shaderのコードで頂点座標のロケーション(場所)は0に指定済み
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
}

void GLWidget::paintGL(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_program);
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, m_vertices.size() / 3); // ドローコール(描画)
}
