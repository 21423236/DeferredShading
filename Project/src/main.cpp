#include <Windows.h>
#include <Framework/Window.h>
#include <Framework/Program.h>

#include <Framework/Application.h>
#include <Framework/DeferredRenderer.h>

#include <GL/glew.h>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	Application application(new DeferredRenderer());
	return application.Run();
}