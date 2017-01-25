#include <Windows.h>
#include <Framework/Window.h>
#include <Framework/Program.h>

#include <Framework/Application.h>
#include <Framework/DeferredRenderer.h>

#include <GL/glew.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	Application app(new DeferredRenderer());
	while (app.IsRunning())
	{
		
	}

	Window window(800, 600, "Framework");
	window.MakeCurrent();


	MSG msg;
	std::memset(&msg, 0, sizeof(MSG));

	if (glewInit() != GLEW_OK)
	{
		MessageBox(NULL, "Couldn't initialize glew32", "Error!", MB_OK | MB_ICONHAND);
		msg.message = WM_QUIT;
	}

	glClearColor(1, 0, 0, 1);

	GLuint vao, vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float quadData[] = {
		-1.0f,  1.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (GLvoid*)(sizeof(float) * 2));

	Program program;
	program.CreateHandle();
	program.AttachShader(Program::VERTEX_SHADER_TYPE, "src/Shaders/Basic.vert");
	program.AttachShader(Program::FRAGMENT_SHADER_TYPE, "src/Shaders/Basic.frag");
	program.Link();

	while (msg.message != WM_QUIT) {
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program.Use();
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
		program.Unuse();

		window.SwapBuffers();
	}

	return 0;
}