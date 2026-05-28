#include <GLApp.h>
#include <ArcBall.h>
#include <FontRenderer.h>
#include <cmath>
#include <cstdint>
#ifndef __EMSCRIPTEN__
#include <mutex>
#include <thread>
#endif
#include <vector>
#include "Scene.h"
#include "Camera.h"
#include "Raytracer.h"
#include "Texture.h"

class MyGLApp : public GLApp {
public:
	Image debugImage{ 600, 600 };
	Image image{ 600,600 };
	Scene scene;
	Camera camera;
	FontRenderer fontRenderer{"helvetica_neue.bmp", "helvetica_neue.pos"};
	std::shared_ptr<FontEngine> fontEngine{nullptr};
	std::vector<float> previewData;
	Vec3 previewCenter;
	ArcBall arcBall{Vec2ui{600, 600}};

	bool drawDebug = false;
	bool mouseDragActive{false};
	bool showPreview{true};
	bool renderRequested{false};
#ifndef __EMSCRIPTEN__
	std::mutex renderMutex;
	bool renderRunning{false};
	bool renderReady{false};
	uint64_t renderGeneration{0};
	Image renderedImage{600,600};
	Image renderedDebugImage{600,600};
#endif

	MyGLApp() : GLApp{ 600,600,1,"Texturing" } {}

	virtual void init() override {
		GL(glDisable(GL_CULL_FACE));
		GL(glEnable(GL_DEPTH_TEST));
		fontEngine = fontRenderer.generateFontEngine();

		camera.setEyePoint(Vec3{ 0.0, 0.0, 2.0 });
		camera.setLookAt(Vec3{ 0.0, 0.0, 0.0 });

		scene = Scene::genTexturedScene();
		const Vec3 backgroundColor = scene.getBackgroundcolor();
		setBackground(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
		previewData = scene.getTriangleData();
		previewCenter = computePreviewCenter(previewData);
		requestRender();
	}

	virtual void resize(const Dimensions winDim, const Dimensions fbDim) override
	{
		GLApp::resize(winDim, fbDim);
		arcBall.setWindowSize(Vec2ui{winDim.width, winDim.height});
	}

	Image render(Scene scene, Camera camera, int depth, uint32_t width, uint32_t height, bool debug = false) {
		scene.setDebug(debug);

		Image result{width, height};
		Raytracer renderer(depth, 9);
		renderer.setCamera(camera);
		renderer.setScene(scene);
		renderer.render(result);
		return result;
	}

	Vec3 computePreviewCenter(const std::vector<float>& data) const
	{
		Vec3 center;
		size_t vertexCount = 0;

		for (size_t i = 0; i + 9 < data.size(); i += 10)
		{
			center = center + Vec3{data[i + 0], data[i + 1], data[i + 2]};
			++vertexCount;
		}

		return vertexCount == 0 ? Vec3{} : center / float(vertexCount);
	}

	void requestRender()
	{
#ifndef __EMSCRIPTEN__
		std::lock_guard<std::mutex> lock(renderMutex);
		++renderGeneration;
		renderRequested = true;
		renderReady = false;
		showPreview = true;
#else
		renderRequested = true;
		showPreview = true;
#endif
	}

	void startRenderIfNeeded()
	{
#ifndef __EMSCRIPTEN__
		Scene sceneToRender;
		Camera cameraToRender;
		uint32_t width = image.width;
		uint32_t height = image.height;
		uint64_t generation = 0;

		{
			std::lock_guard<std::mutex> lock(renderMutex);
			if (renderRunning || !renderRequested)
				return;

			renderRunning = true;
			renderRequested = false;
			generation = renderGeneration;
			sceneToRender = scene;
			cameraToRender = camera;
			width = image.width;
			height = image.height;
		}

		std::thread([this, sceneToRender, cameraToRender, generation, width, height]() {
			Image result = render(sceneToRender, cameraToRender, 5, width, height);
			Image debugResult = render(sceneToRender, cameraToRender, 5, width, height, true);

			std::lock_guard<std::mutex> lock(renderMutex);
			if (generation == renderGeneration)
			{
				renderedImage = std::move(result);
				renderedDebugImage = std::move(debugResult);
				renderReady = true;
			}
			renderRunning = false;
		}).detach();
#else
		if (!renderRequested || mouseDragActive)
			return;

		renderRequested = false;
		image = render(scene, camera, 5, image.width, image.height);
		debugImage = render(scene, camera, 5, debugImage.width, debugImage.height, true);
		showPreview = false;
#endif
	}

	void collectRenderResult()
	{
#ifndef __EMSCRIPTEN__
		std::lock_guard<std::mutex> lock(renderMutex);
		if (!renderReady)
			return;

		image = std::move(renderedImage);
		debugImage = std::move(renderedDebugImage);
		renderReady = false;
		showPreview = false;
#endif
	}

	void updatePreviewLight()
	{
		std::shared_ptr<const LightSource> light = scene.getLight(0);
		if (!light)
			return;

		const Vec3 localOrigin;
		const Vec3 localLightPos = localOrigin + light->getDirection(localOrigin) * light->getDistance(localOrigin);
		setLightPos(scene.getModel() * localLightPos);
	}

	void drawPreviewText()
	{
		if (!fontEngine)
			return;

		fontEngine->render("OpenGL Preview", getAspect(), 0.035f, {0.0f, -0.92f}, Alignment::Center, {1.0f, 0.0f, 0.0f, 0.9f});
	}

	virtual void draw() override {
		collectRenderResult();
		startRenderIfNeeded();

		if (showPreview)
		{
			setDrawProjection(camera.getViewProjection(getAspect()));
			setDrawTransform(scene.getModel());
			updatePreviewLight();
			drawTriangles(previewData, TrisDrawType::LIST, false, true);
			drawPreviewText();
			return;
		}

		setDrawProjection(Mat4{});
		setDrawTransform(Mat4{});
		if (drawDebug)
			drawImage(debugImage);
		else
			drawImage(image);
	}

	virtual void mouseButton(int button, int state, int mods, double xPosition, double yPosition) override
	{
		if (button != GLENV_MOUSE_BUTTON_LEFT)
			return;

		mouseDragActive = state == GLENV_MOUSE_PRESS;
		if (mouseDragActive)
		{
			arcBall.click(Vec2ui{uint32_t(xPosition), uint32_t(yPosition)});
			showPreview = true;
		}
		else
		{
			requestRender();
		}
	}

	virtual void mouseMove(double xPosition, double yPosition) override
	{
		if (!mouseDragActive)
			return;

		const Mat4 rotation = arcBall.drag(Vec2ui{uint32_t(xPosition), uint32_t(yPosition)}).computeRotation();
		const Vec3 pivot = scene.getModel() * previewCenter;
		const Mat4 rotateAroundPivot = Mat4::translation(pivot) * rotation * Mat4::translation(pivot * -1.0f);
		scene.setModel(rotateAroundPivot * scene.getModel());
		requestRender();
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
