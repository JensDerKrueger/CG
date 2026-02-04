#include <GLApp.h>
#include <cmath>
#include <optional>
#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"
#include "Texture.h"

class MyGLApp : public GLApp {
public:
	Image debugImage{ 600, 600 };
	Image image{ 600,600 };

	bool drawDebug = false;

	MyGLApp() : GLApp{ 600,600,1,"Texturing" } {}

	virtual void init() override {
		GL(glDisable(GL_CULL_FACE));
		GL(glClearColor(0, 0, 0, 0));

		Scene texturedScene = Scene::genTexturedScene();
		render(texturedScene, 5, image);
		render(texturedScene, 5, debugImage, true);
	}

	void render(Scene& scene, int depth, Image& image, bool debug = false) {
		scene.setDebug(debug);

		Camera camera;
		camera.setEyePoint(Vec3{ 0.0, 0.0, 2.0 });
		camera.setLookAt(Vec3{ 0.0, 0.0, 0.0 });

		// instantiates the actual renderer with recursion depth "depth" and
		// 9x super sampling NOTE: you might want to reduce the super sampling
		// to 1 for testing and debugging
		Raytracer renderer(depth, 9);
		renderer.setCamera(camera);
		renderer.setScene(scene);
		renderer.render(image);
	}

	virtual void draw() override {
		GL(glClear(GL_COLOR_BUFFER_BIT));

		if (drawDebug)
			drawImage(debugImage);
		else
			drawImage(image);
	}

	void keyboard(int key, int scancode, int action, int mods) override {
		if (key == GLENV_KEY_SPACE && action == GLENV_PRESS) {
			drawDebug = !drawDebug;
		}
	}

} myApp;

#ifdef _WIN32
#include <Windows.h>

INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow) {
	std::vector<std::string> args = getArgsWindows();
#else
int main(int argc, char** argv) {
	std::vector<std::string> args{ argv + 1, argv + argc };
#endif
	try {
		myApp.run();
	}
	catch (const GLException& e) {
		std::stringstream ss;
		ss << "Insufficient OpenGL Support " << e.what();
#ifndef _WIN32
		std::cerr << ss.str().c_str() << std::endl;
#else
		MessageBoxA(
			NULL,
			ss.str().c_str(),
			"OpenGL Error",
			MB_ICONERROR | MB_OK
		);
#endif
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
