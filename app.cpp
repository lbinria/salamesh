#include "app.h"

#include "core/cameras/trackball_camera.h"
#include "core/cameras/descent_camera.h"
#include "core/renderers/line_renderer.h"

#include "core/models/polyline_model.h"
#include "core/models/surface_model.h"
#include "core/models/volume_model.h"

#include "module_triangle_diagnostic/triangle_inspector.h"

#include <ranges>

struct UBOMatrices {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec2 viewport;
};

Image App::screenshot(const std::string& filename, int targetWidth, int targetHeight) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);

	// Allocate memory for pixel data
	std::vector<unsigned char> pixels(width * height * 4);

	// Read pixels from front buffer
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	// Flip the image vertically
	std::vector<unsigned char> flipped_pixels(width * height * 4);
	for(int y = 0; y < height; ++y) {
		memcpy(
			&flipped_pixels[y * width * 4],
			&pixels[(height - 1 - y) * width * 4],
			width * 4
		);
	}

	// No resize needed
	// Save to PNG file
	if (targetWidth == -1 && targetHeight == -1) {
		stbi_write_png(filename.c_str(), width, height, 4, flipped_pixels.data(), 0);
		unsigned int texId;
		int w, h, c;
		sl::load_texture_2d(filename.c_str(), texId, w, h, c);
		return Image{
			.texId = texId,
			.width = w,
			.height = h,
			.channels = c
		};
	}

	// Calculate target dimensions
	int resizedWidth = targetWidth;
	int resizedHeight = targetHeight;

    // Maintain aspect ratio if only one dimension is specified
    if (targetWidth == -1) {
        float aspect = static_cast<float>(width) / height;
        resizedWidth = static_cast<int>(resizedHeight * aspect);
    }
    if (targetHeight == -1) {
        float aspect = static_cast<float>(height) / width;
        resizedHeight = static_cast<int>(resizedWidth * aspect);
    }

	// Resize buffer
	std::vector<unsigned char> resizedPixels(resizedWidth * resizedHeight * 4);

    // Resize using stb_image_resize
    stbir_resize_uint8_srgb(
        flipped_pixels.data(), width, height, 0,  // source image
        resizedPixels.data(), resizedWidth, resizedHeight, 0,  // destination image
        STBIR_RGBA  // number of channels
    );

	// Save resized image
	stbi_write_png(filename.c_str(), resizedWidth, resizedHeight, 4, resizedPixels.data(), 0);

	unsigned int texId;
	int w, h, c;
	sl::load_texture_2d(filename.c_str(), texId, w, h, c);
	return Image{
		.texId = texId,
		.width = w,
		.height = h,
		.channels = c
	};
}

void App::quit() {
	glfwSetWindowShouldClose(window, true);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
	if (!app)
		return;
	
	// Redirect to virtual
	app->key_event(key, scancode, action, mods);
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app)
		return;

	// Redirect to virtual
	app->mouse_scroll(xoffset, yoffset);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app)
		return;

	// Dbl clicks detection
	if (action == GLFW_PRESS) {
		InputState &st = app->getInputState();
		for (int i = 0; i < 8; ++i) {
			// Get last click time for this button
			auto lastClick = st.mouse.lastClicks[i];
			// Compute the time since last click
			auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastClick);
			st.mouse.dblButtons[i] = (dt < std::chrono::milliseconds(st.mouse.getDblClickInterval()));
		}

		// Set the last click time for this button
		st.mouse.lastClicks[button] = std::chrono::steady_clock::now();
	}

	// Redirect to virtual
	app->mouse_button(button, action, mods);
}

static void set_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app)
		return;
	
	// Redirect to virtual
	app->mouse_move(xpos, ypos);
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));

    if (!app)
		return;

	app->screenWidth = width;
	app->screenHeight = height;

	app->getRenderSurface().resize(width, height);
}

void App::setupColormaps() {
	// Load default colormap textures
	addColormap("CET-R41", sl::assetsPath("CET-R41px.png"));
	addColormap("CET-L08", sl::assetsPath("CET-L08px.png"));
	addColormap("alpha", sl::assetsPath("colormap_alpha.png"));
	addColormap("cat", "/home/tex/Models/cat/Cat_diffuse.jpg");
	addColormap("extended", sl::assetsPath("extended.png"));
}

void App::setupCameras() {
	auto trackballCamera = std::make_shared<TrackBallCamera>();
	cameras["default"] = std::move(trackballCamera);
	setSelectedCamera("default");
}

bool App::setup() {

	std::cout << "App setup..." << std::endl;

	std::chrono::steady_clock::time_point begin_setup = std::chrono::steady_clock::now();


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// MSAA
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create a GLFW window
	window = glfwCreateWindow(screenWidth, screenHeight, "SalaMesh", NULL, NULL);
	
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	std::cout << "GLFW window created !" << std::endl;

	// Switch to fullscreen

	// 1) Get window center in desktop coords
	int wx, wy, ww, wh;
	glfwGetWindowPos(window, &wx, &wy);
	glfwGetWindowSize(window, &ww, &wh);
	int cx = wx + ww/2;
	int cy = wy + wh/2;
	std::cout << "Window pos & size: " 
		<< cx << ", " << cy << ", " << ww << ", " << wh << "]" 
		<< std::endl;

	int monitorCount = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);	
	std::cout << "Monitors count: " << monitorCount << std::endl;

	// Maybe check that ?
	if (monitorCount == 0) {
		std::cerr << "No monitor found, quit the app." << std::endl;
		return false;
	}

	// Choose best monitor
	GLFWmonitor* monitor = nullptr;
	int res = 0;
	int monitorX = 0, monitorY = 0;
	for (int i = 0; i < monitorCount; ++i) {
		GLFWmonitor* m = monitors[i];
		int mx, my, mw, mh;
		glfwGetMonitorWorkarea(m, &mx, &my, &mw, &mh);
		int curRes = mw * mh;

		std::cout << "Monitor " << i << " working area: [" << mx << ", " << my << ", " << mw << ", " << mh << "]" << std::endl;
		std::cout << "Monitor res: " << curRes << std::endl;

		if (cx >= mx && cx <= mx + mw &&
			cy >= my && cy <= my + mh && res <= curRes) {
			// The window center is within the monitor's work area
			monitor = m;
			curRes = res;
			monitorX = mx;
			monitorY = my;
		}
	}

	// Quick fix ! No best monitor ? Take arbitrary the first
	if (!monitor)
		monitor = monitors[0];

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	if (monitor) {
		// Calculate 80% of screen width and height
		int windowWidth = static_cast<int>(mode->width * 0.8f);
		int windowHeight = static_cast<int>(mode->height * 0.8f);

		// Calculate the position to center the window
		int posX = (mode->width - windowWidth) / 2 + monitorX;
		int posY = (mode->height - windowHeight) / 2 + monitorY;

		// Set window to 80% size and center it
		glfwSetWindowPos(window, posX, posY);
		glfwSetWindowSize(window, windowWidth, windowHeight);

		// Update screen width and height if needed
		screenWidth = windowWidth;
		screenHeight = windowHeight;
	}

	std::cout << "Window size: " << screenWidth << ", " << screenHeight << std::endl;

	glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);

	std::cout << "GLFW current context created !" << std::endl;

	// Setup inputs
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, set_cursor_pos_callback);  

	std::cout << "GLFW setup input callbacks !" << std::endl;


    // Setup ImGui
	// Comment below because it takes 50ms !
    // IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

	// hook function pointers
	#ifdef WIN32
		glewInit();
	#else

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	
	#endif

	std::chrono::steady_clock::time_point end_setup = std::chrono::steady_clock::now();
    std::cout << "GLFW / ImGui setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_setup - begin_setup).count() << "ms" << std::endl;

	// OpenGL
	glEnable(GL_MULTISAMPLE);  
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Query vendor and renderer strings
	const GLubyte* vendor   = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version  = glGetString(GL_VERSION);

	std::cout << "GPU Vendor  : " << (vendor   ? reinterpret_cast<const char*>(vendor)   : "Unknown") << "\n";
	std::cout << "GPU Renderer: " << (renderer ? reinterpret_cast<const char*>(renderer) : "Unknown") << "\n";
	std::cout << "GL Version  : " << (version  ? reinterpret_cast<const char*>(version)  : "Unknown") << "\n";

	// GLint64 maxUnits;
	// glGetInteger64i_v(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0, &maxUnits);
	GLint maxUnits;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << maxUnits << std::endl;

	GLint maxColorAttachments = 0;
	GLint maxDrawBuffers     = 0;

	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);

	std::cout 
		<< "GL_MAX_COLOR_ATTACHMENTS = " << maxColorAttachments << std::endl
		<< "GL_MAX_DRAW_BUFFERS     = " << maxDrawBuffers     << std::endl;

	// Check max draw buffers
	if (maxColorAttachments < 5) {
		std::cerr << "Warning: your GPU supports only " << maxColorAttachments << " color attachments, some features may not work." << std::endl;
	}
	if (maxDrawBuffers < 5) {
		std::cerr << "Warning: your GPU supports only " << maxDrawBuffers << " draw buffers, some features may not work." << std::endl;
	}

	// Load default colormap textures
	setupColormaps();

	// Load icons
	int iconWidth, iconHeight, iconChannels;
	if (!sl::load_texture_2d(sl::assetsPath("icons/Eye.png"), eyeIcon, iconWidth, iconHeight, iconChannels)) {
		std::cerr << "Unable to load Eye icon." << std::endl;
	}
	if (!sl::load_texture_2d(sl::assetsPath("icons/BugAnt.png"), bugAntIcon, iconWidth, iconHeight, iconChannels)) {
		std::cerr << "Unable to load BugAnt icon." << std::endl;
	}

	auto renderSurface = std::make_unique<RenderSurface>(screenWidth, screenHeight);
	renderSurface->setBackgroundColor({0.05, 0.1, 0.15});
	renderSurface->setup();
	renderSurfaces.push_back(std::move(renderSurface));

	// auto renderSurface2 = std::make_unique<RenderSurface>(screenWidth, screenHeight);
	// renderSurface2->setBackgroundColor({0.16, 0.85, 0.35});
	// glGenFramebuffers(1, &screenFbo);
	// renderSurface2->setup();
	// renderSurfaces.push_back(std::move(renderSurface2));

	// Init UBO
	sl::createUBO(uboMatrices, sizeof(UBOMatrices));


	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    float quadVerts[] = { 
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

	// Screen init
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	glEnable(GL_PROGRAM_POINT_SIZE);
	// glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	return true;
}

// TODO probably merge some with start function
void App::init() {


	std::cout << "Init" << std::endl;

	// Register model types
	modelInstanciator.registerType("TriModel", []() { return std::make_unique<TriModel>(); });
	modelInstanciator.registerType("QuadModel", []() { return std::make_unique<QuadModel>(); });
	modelInstanciator.registerType("PolyModel", []() { return std::make_unique<PolyModel>(); });
	modelInstanciator.registerType("TetModel", []() { return std::make_unique<TetModel>(); });
	modelInstanciator.registerType("HexModel", []() { return std::make_unique<HexModel>(); });
	// modelInstanciator.registerType("PolylineModel", []() { return std::make_unique<PolylineModel>(); });
	// modelInstanciator.registerType("PyramidModel", []() { return std::make_unique<PyramidModel>(); });
	// modelInstanciator.registerType("PrismModel", []() { return std::make_unique<PrismModel>(); });

	// Register cameras types
	cameraInstanciator.registerType("DescentCamera", []() { return std::make_unique<DescentCamera>(); });
	cameraInstanciator.registerType("TrackBallCamera", []() { return std::make_unique<TrackBallCamera>(); });
	// Register renderers types
	rendererInstanciator.registerType("LineRenderer", []() { return std::make_unique<LineRenderer>(); });
	rendererInstanciator.registerType("PointSetRenderer", []() { return std::make_unique<PointSetRenderer>(); });

	// Create cameras
	setupCameras();
	
	getRenderSurface().setCamera(cameras["default"]);
	// renderSurfaces[1]->setCamera(cameras[1]);

	// Load model
	// Check for files given in args (is it models, scripts ?)
	std::set<std::string> accepted{".obj", ".mesh", ".geogram"};

	for (auto &p : args.paths) {
		std::string ext = p.extension().string();
		sl::toLower(ext);

		if (accepted.contains(ext)) {
			auto modelName = loadModel(p.string());
			focus(modelName);
		}
		else if (p.extension() == ".lua") {
			std::cout << "load script: " << p.string() << std::endl;
			auto script = std::make_unique<LuaScript>(*this, p.string());
			script->init();
			scripts.push_back(std::move(script));
		}
		else if (p.extension() == ".json") {
			// Check internal format of json file (state file, settings...)
			std::ifstream ifs(p);
			if (!ifs.is_open()) {
				std::cerr << "Failed to open file given as argument: " << p.string() << std::endl;
				continue;;
			}

			json j;
			ifs >> j;
			ifs.close();

			if (j["header"]["type"] == "state") {
				loadState(j, p.string());
			}
		}
	}

	// Load modules
	Settings settings;
	settings.load(args.settings_path);
	loadModules(settings);

	// auto ti = std::make_unique<TriangleInspector>(*this);
	// ti->init();
	// scripts.push_back(std::move(ti));
}

void App::start() {

	Shader screenShader(sl::shadersPath("screen.vert"), sl::shadersPath("screen.frag"));
	screenShader.use();
	screenShader.setInt("screenTexture", 0);

	// Init inherited class 
	init();

	glm::mat4 model(1.f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.f, 0.f, 0.f));

	glm::mat4 projection(1.f);
	glm::mat4 view(1.f);

	view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

	float lastTimeValue = glfwGetTime();


	while(!glfwWindowShouldClose(window)) {

		// Compute dela time
		float timeValue = glfwGetTime();
		float dt = timeValue - lastTimeValue;

		processInput(window);

		// Set view / projection from current camera
		view = getCurrentCamera().getViewMatrix();
		projection = getCurrentCamera().getProjectionMatrix();

		// Update UBO
		UBOMatrices mats{
			view,
			projection,
			{getWidth(), getHeight()} // TODO here maybe replace by renderSurface width / height
		};
		
		sl::updateUBOData(uboMatrices, sizeof(UBOMatrices), &mats);

		getRenderSurface().bind();
		getRenderSurface().clear();
		glEnable(GL_DEPTH_TEST);
		glCullFace(cull_mode);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		update(dt);

		// Render scene
		for (auto &[k, r] : renderers) {
			glm::vec3 o{0.f};
			r->render(o);
		}

		for (auto &[k, model] : models) {
			model->setColormap0Texture(colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_0)].tex);
			model->setColormap1Texture(colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_1)].tex);
			model->setColormap2Texture(colormaps[model->getSelectedColormap(ColormapLayer::COLORMAP_LAYER_2)].tex);
			model->render();
		}

		// Go back to default framebuffer to draw the screen quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		getRenderSurface().render(screenShader, quadVAO);


		// renderSurfaces[1]->bind();
		// renderSurfaces[1]->clear();
		// glEnable(GL_DEPTH_TEST);
		// glCullFace(cull_mode);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// // // // Update UBO
		// // // glm::mat4 mats[2] = { view, projection }; // Ensure view and projection matrices are contiguous in memory
		// // // glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		// // // glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, glm::value_ptr(mats[0]), GL_DYNAMIC_DRAW);
		// // // glBindBuffer(GL_UNIFORM_BUFFER, 0);  

		// // Render scene
		// for (auto &model : models) {
		// 	model->setTexture(colormaps[model->getSelectedColormap()]);
		// 	model->render();
		// }

		// glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
		// renderSurfaces[1]->render(screenShader, quadVAO);




		// -------------------
		// Go back to default framebuffer to draw UI
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// --- Draw UI ---
		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		
		ImGui::NewFrame();
	
		// ImGui::Begin("Render");
		// ImVec2 size = ImGui::GetWindowSize();
		// renderSurfaceWindowHovered = ImGui::IsItemHovered();
		// ImGui::Image((ImTextureID)renderSurfaces[1]->texColor, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		// ImGui::End();

		drawGui();

		// ImGui::Begin("Render");
		// ImVec2 size = ImGui::GetWindowSize();
		// auto s = getRenderSurface();
		// s.resize(size.x, size.y);
		// ImGui::Image((ImTextureID)s.texColor, size, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));
		// ImGui::End();

		// Render the ImGui frame
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// -------------------

		// Swap buffers and get IO
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		lastTimeValue = timeValue;
	}

	// Clean up
	clean();
	// Terminate & quit
	glfwTerminate();

	return;
}

// Continuous update
void App::update(float dt) {
	
	if (!_isUIHovered)
		updateCamera(dt);

	for (auto &script : scripts) {
		script->update(dt);
	}

}

void App::updateCamera(float dt) {

	float speed = 0.01f;
	if (countModels() > 0) {
		speed = getCurrentModel().getRadius() * 0.5f * dt;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		getCurrentCamera().moveForward(speed);
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		getCurrentCamera().moveForward(-speed);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		getCurrentCamera().moveRight(-speed);
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		getCurrentCamera().moveRight(speed);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		getCurrentCamera().moveUp(speed);
	} else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		getCurrentCamera().moveUp(-speed);
	}


	if (st.mouse.isLeftButton()) {
		// getCurrentCamera().move(st.mouse.delta);
		getCurrentCamera().move(st.mouse.lastPos, st.mouse.pos);
	}

	if (st.mouse.isRightButton()) {
		auto trackball = dynamic_cast<TrackBallCamera*>(&getCurrentCamera());
		if (trackball) {
			trackball->movePan(st.mouse.delta);
		}
	}


	
}

void App::setupLayout() {


	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {

		ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::DockSpaceOverViewport(dockspace_id, viewport, ImGuiDockNodeFlags_PassthruCentralNode);

		static auto first_time = true;
		if (first_time)
		{
			first_time = false;
			// Clear out existing layout
			ImGui::DockBuilderRemoveNode(dockspace_id);

			ImGuiID top_bar_node = ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_NoTabBar);
			ImGui::DockBuilderSetNodeSize(top_bar_node, ImVec2(viewport->WorkSize.x, 30.0f)); // Thin top 

			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetWindowSize());



			// get id of main dock space area
			ImGuiID dockspace_main_id = dockspace_id;
			// Create a dock node for the right docked window
			ImGuiID navBar = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Left, 1.f, nullptr, &dockspace_main_id);
			ImGuiID toolBar = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Right, 1.f, nullptr, &dockspace_main_id);
			ImGuiID botBar = ImGui::DockBuilderSplitNode(dockspace_main_id, ImGuiDir_Down, 0.2f, nullptr, &dockspace_main_id);

			ImGuiID leftbot = ImGui::DockBuilderSplitNode(navBar, ImGuiDir_Down, 0.25f, nullptr, &navBar);

			ImGui::DockBuilderDockWindow("Bot Bar", botBar);
			// ImGui::DockBuilderDockWindow("Left Side Bar", leftbot);
			// ImGui::DockBuilderDockWindow("Right Side Bar", toolBar);
			// ImGui::DockBuilderDockWindow("Right Side Bar2", toolBar);
			// // Position the top bar at the top
			// ImGui::DockBuilderDockWindow("TopBar", top_bar_node);

			for (auto &s : scripts) {
				auto layout = s->layoutGui();
				for (auto &l : layout) {
					if (l.second == "nav_bar")
						ImGui::DockBuilderDockWindow(l.first.c_str(), navBar);
					else if (l.second == "bot_bar")
						ImGui::DockBuilderDockWindow(l.first.c_str(), botBar);
					else if (l.second == "tool_bar")
						ImGui::DockBuilderDockWindow(l.first.c_str(), toolBar);
				}
			}

			// ImGui::DockBuilderDockWindow("Scene", left);

			ImGui::DockBuilderFinish(dockspace_id);
		}


		ImGui::Begin("Bot Bar");
		ImGui::Text("bot bar");
		ImGui::End();


	}
}



void App::drawGui() {


	setupLayout();

	// Foot bar
	ImGui::SetNextWindowPos(
		ImVec2(0, ImGui::GetIO().DisplaySize.y - 30.f), 
		ImGuiCond_Always
	);
	ImGui::SetNextWindowSize(
		ImVec2(ImGui::GetIO().DisplaySize.x, 30.f)
	);

	ImGui::Begin("Foot_bar", nullptr, 
		ImGuiWindowFlags_NoMove |           // Prevent window movement
		ImGuiWindowFlags_AlwaysAutoResize | // Automatically adjust size
		ImGuiWindowFlags_NoDecoration         // Remove default title bar (optional)
	);

	ImGui::Text(getNavigationPathString().c_str());

	ImGui::End();

	
	// display
	if (ImGuiFileDialog::Instance()->Display("OpenModelDlg")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK

			std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			std::cout << "directory path:" << directoryPath << std::endl;

			auto selection = ImGuiFileDialog::Instance()->GetSelection();
			
			for (auto &kv : selection)
			{
				std::string filename = kv.first;
				std::string fullpath = kv.second;
				std::cout << "filename: " << filename << ", fullpath: " << fullpath << std::endl;
				std::cout << "read model..." << std::endl;
				auto modelName = loadModel(fullpath);
				
				if (!modelName.empty())
					focus(modelName);
			}
		}
		
		// close
		ImGuiFileDialog::Instance()->Close();
	}

	// display save dialog
	if (ImGuiFileDialog::Instance()->Display("SaveModelAsDlg")) {
		if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
			std::string filename = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string directoryPath = ImGuiFileDialog::Instance()->GetCurrentPath();
			// action
			std::cout << "file path:" << directoryPath << ", file path name: " << filename << std::endl;
			std::cout << "save model..." << std::endl;
			if (!getCurrentModel().saveAs(filename)) {
				std::cerr << "Unable to save current model at: " << filename << std::endl;
			}
		}
		
		// close
		ImGuiFileDialog::Instance()->Close();
	}

	
	ImGui::GetBackgroundDrawList()->AddCircle(ImGui::GetMousePos(), st.mouse.getCursorRadius(), IM_COL32(225, 225, 255, 200), 0, 2);



	for (auto &script : scripts) {
		// if (script->status == LuaScript::SCRIPT_STATUS_OK) {
			bool success = script->drawGui(ImGui::GetCurrentContext());
			if (!success) {

				// Clear properly ImGui

				// auto &io = ImGui::GetIO();
				// bool last_error_recovery = io.ConfigErrorRecovery;
				// io.ConfigErrorRecovery = false;
				
				// // ImGui::NewFrame();
				// // ImGui::EndFrame();

				// io.ConfigErrorRecovery = last_error_recovery;
				// // Clean up commands
				// auto draw_data = ImGui::GetDrawData();
				// if (draw_data != nullptr)
				// 	draw_data->Clear();

			}
		// }
	}

	_isUIHovered = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow);
}

void App::clean() {
	std::cout << "App clean..." << std::endl;

	for (auto &[k, model] : models) {
		model->clean();
	}

	for (auto &[k, renderer] : renderers) {
		renderer->clean();
	}

	for (auto &script : scripts) {
		script->cleanup();
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	for (auto &rs : renderSurfaces)
		rs->clean();

	// Clear textures
	for (int i = 0; i < colormaps.size(); ++i)
		glDeleteTextures(1, &colormaps[i].tex);
}

std::string App::loadModel(const std::string& filename, std::string name) {

	std::string modelName = name.empty() ? 
		std::filesystem::path(filename).stem().string() + std::to_string(models.size()) : 
		name;

	bool success = false;

	std::unique_ptr<Model> model;
	model = std::make_unique<PolyModel>();

	success = model->load(filename);

	if (!success) {
		model = std::make_unique<TriModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<QuadModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<TetModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<HexModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<PolylineModel>();
		success = model->load(filename);
	}

	if (!success)
		return "";


	// Setup default gfx
	model->setLight(true);
	auto meshRenderer = model->getMeshRenderer();
	if (meshRenderer) {
		meshRenderer->setMeshShrink(0.f);
		meshRenderer->setMeshSize(0.0f);
	}
	
	auto edges = model->getEdgesRenderer();
	if (edges && model->getModelType() != ModelType::POLYLINE_MODEL) {
		edges->setVisible(false);
	}

	// By default points not visible
	model->getPointsRenderer().setVisible(false);

	// Setup default clipping plane
	model->setupClipping();
	modelNameByIndex[model->getIndex()] = modelName;
	models[modelName] = std::move(model);

	// Update cameras far planes
	computeFarPlane();

	// Notify scripts
	for (auto &s : scripts) {
		s->modelLoaded(modelName);
	}

	return modelName;
}

void App::computeFarPlane() {
	auto diameter = computeSceneDiameter();

	for (auto &[k, c] : cameras) {
		c->setFarPlane(diameter * 5.f /* 5.f is an arbitrary value... */);
	}

	// TODO should refresh camera here, elsewhere nothing will be visible until we doing somethin that refresh the camera!
}

void App::focus(std::string modelName) {
	setSelectedModel(modelName);
	auto &model = models[modelName];
	getCurrentCamera().lookAtBox(model->bbox());
}



void App::addColormap(const std::string name, const std::string filename) {
	int width, height, nrChannels;
	
	Colormap cm{
		name,
		0,
		0,
		0
	};

	if(!sl::load_texture_2d(filename, cm.tex, width, height, nrChannels)) {
		std::cerr << "Unable to load colormap " << name << " at " << filename << "." << std::endl;
		return;
	}

	cm.width = width;
	cm.height = height;

	colormaps.resize(colormaps.size() + 1);
	colormaps[colormaps.size() - 1] = cm;
}

void App::removeColormap(const std::string name) {
	for (int i = 0; i < colormaps.size(); ++i) {
		if (colormaps[i].name == name) {
			colormaps.erase(colormaps.begin() + i);
			return;
		}
	}
}

Colormap App::getColormap(const std::string name) {
	for (int i = 0; i < colormaps.size(); ++i) {
		if (colormaps[i].name == name) {
			return colormaps[i];
		}
	}
	throw std::runtime_error("Colormap " + name + " not found.");
}

Colormap App::getColormap(int idx) {
	return colormaps[idx];
}

long App::pickEdge(double x, double y) {
	if (!st.cell.anyHovered() && !st.facet.anyHovered())
		return -1;

	auto model = getHoveredModel();
	if (!model)
		return -1;

	auto p = pickPoint(x, y);

	// TODO here look at all condition that are useless, but if I use line below, there is a bug
	// It seems that some hovered element are update during call to this function, is it possible as
	// it is called by callback mouse_move that is decorelated from main loop
	// Line below should be sufficient to get hovered

	int h;
	if (st.cell.anyHovered() && (model->getModelType() == ModelType::HEX_MODEL || model->getModelType() == ModelType::TET_MODEL))
		h = st.cell.getHovered();
	else if (st.facet.anyHovered() && (model->getModelType() == ModelType::TRI_MODEL || model->getModelType() == ModelType::QUAD_MODEL || model->getModelType() == ModelType::POLYGON_MODEL))
		h = st.facet.getHovered();
	else 
		return -1;

	return model->pickEdge(p, h);
}

long App::pick_mesh(double x, double y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT4);
	long id = pick(x, y);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	return id >= 0 && id < Model::getMaxIndex() ? id : -1;
}

std::vector<long> App::pick_vertices(double x, double y, int radius) {
	auto model = getHoveredModel();

	if (!model)
		return {};

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	auto ids = pick(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Clean ids
	std::vector<long> clean_ids;
	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
		return id >= 0 && id < model->nverts();
	});

	return clean_ids;
}

std::vector<long> App::pick_facets(double x, double y, int radius) {
	auto model = getHoveredModel();

	if (!model)
		return {};

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	auto ids = pick(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Clean ids
	std::vector<long> clean_ids;
	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
		return id >= 0 && id < model->nfacets();
	});

	return clean_ids;
}

std::vector<long> App::pick_cells(double x, double y, int radius) {		
	auto model = getHoveredModel();

	if (!model)
		return {};

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	auto ids = pick(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Clean ids
	std::vector<long> clean_ids;
	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
		return id >= 0 && id < model->ncells();
	});

	return clean_ids;
}

float getLinearDepth(float depth, float _near, float _far) {
	return _near * _far / (_far + depth * (_near - _far));
}

float App::getDepth(double x, double y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
    float depth;
    glReadPixels(x, screenHeight - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
}

void getNDC(int x, int y, int w, int h, float &ndcX, float &ndcY) {
	ndcX = (2.f*x) / w - 1.0f;
	ndcY = 1.0f - (2.f*y) / h;
}

void App::unproject(int x, int y, float depth, glm::vec3 &p) {
	
	// Screen coordinates to NDC
	float ndcX, ndcY;
	getNDC(x, y, screenWidth, screenHeight, ndcX, ndcY);

	// Clip space coordinates
	glm::vec4 clipSpace(ndcX, ndcY, depth, 1.0f);

	// Unproject clip space to view space
	glm::mat4 invProj = glm::inverse(getCurrentCamera().getProjectionMatrix());
	glm::vec4 viewSpace = invProj * clipSpace;

	// Unproject view space to world space
	glm::mat4 invView = glm::inverse(getCurrentCamera().getViewMatrix());
	glm::vec4 worldSpace = invView * (viewSpace / viewSpace.w);
	p = glm::vec3(worldSpace);
}

glm::vec3 App::pickPoint(double x, double y) {
	// Read depth value
    float depth = getDepth(x, y);
	depth = depth * 2.f - 1.f; // Convert to NDC range [-1, 1]

	glm::vec3 p;
	unproject(x, y, depth, p);
	return p;
}

long App::pick(double x, double y) {	
	unsigned char pixel[4];
	glReadPixels(x, screenHeight - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	// Decode id from pixel
	return pixel[3] == 0 ? -1 :
		pixel[0] +
		pixel[1] * 256 +
		pixel[2] * 256 * 256;
}

std::set<long> App::pick(double xPos, double yPos, int radius) {

	const int diameter = radius * 2 + 1;
	std::set<long> pickIDs;

	// Allocate buffer for square that bounds our circle
	unsigned char* pixelData = new unsigned char[diameter * diameter * 4];

	// Read pixels in square that bounds our circle
	glReadPixels(
		xPos - radius,
		screenHeight - yPos - radius,
		diameter,
		diameter,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		pixelData
	);

	// Process each pixel in the bounding square
	for(int y = 0; y < diameter; ++y) {
		for(int x = 0; x < diameter; ++x) {
			// Calculate distance from center
			int dx = x - radius;
			int dy = y - radius;
			float distSq = dx * dx + dy * dy;

			// Only process pixels within circle
			if(distSq <= radius * radius) {
				int offset = (y * diameter + x) * 4;
				unsigned char r = pixelData[offset];
				unsigned char g = pixelData[offset + 1];
				unsigned char b = pixelData[offset + 2];
				unsigned char a = pixelData[offset + 3];
				
				long pickID = a == 0 ? -1 :
							r +
							g * 256 +
							b * 256 * 256;

				if (pickID != -1) {
					pickIDs.insert(pickID);
				}
			}
		}
	}

	delete[] pixelData;
	return pickIDs;
}

std::tuple<glm::vec3, glm::vec3> App::computeSceneBBox() {
	glm::vec3 min{std::numeric_limits<float>::max()};
	glm::vec3 max{-std::numeric_limits<float>::max()};
	for (auto &[k, m] : models) {
		auto [cmin, cmax] = m->bbox();
		min = glm::min(min, cmin);
		max = glm::max(max, cmax);
	}
	
	return std::make_tuple(min, max);
}

float App::computeSceneDiameter() {
	auto [min, max] = computeSceneBBox();
	return glm::length(max - min);
}

void App::showOpenModelDialog() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	config.countSelectionMax = -1;
	// config.flags = ImGuiFileDialogFlags_Mul
	ImGuiFileDialog::Instance()->OpenDialog(
		"OpenModelDlg", 
		"Choose File", 
		"All supported mesh files {.geogram, .mesh, .obj},.geogram,.mesh,.obj,.json", 
		config
	);
}

void App::showSaveModelDialog() {
	IGFD::FileDialogConfig config;
	config.path = ".";
	config.flags = ImGuiFileDialogFlags_ConfirmOverwrite;
	ImGuiFileDialog::Instance()->OpenDialog("SaveModelAsDlg", "Save as", ".geogram,.mesh,.obj", config);
}

Snapshot App::snapshot() {
	// Create dir snapshots
	try {
		if (!std::filesystem::exists("snapshots"))
			fs::create_directory("snapshots");
	} catch (const std::filesystem::filesystem_error &e) {
		std::cerr << "Error creating directory: " << e.what() << " on snapshot." << std::endl;
		// TODO quit here else side effects !
	}

	auto now = std::chrono::system_clock::now();
	auto unixTimestamp = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
	auto stateFilename = std::filesystem::path("snapshots") / ("state_" + unixTimestamp + ".json");
	auto screenshotFilename = std::filesystem::path("snapshots") / ("state_" + unixTimestamp + ".jpg");
	saveState(stateFilename.string());
	Image thumb = screenshot(screenshotFilename.string(), 256);

	Snapshot s;
	s.stateFilename = stateFilename.string();
	s.thumbFilename = screenshotFilename.string();
	s.image = thumb;
	return s;
}

void App::loadSnapshot() {
	if (!std::filesystem::exists("snapshots"))
		std::cerr << "Warning: no snapshot." << std::endl;

	int unixTimestamp = 0;
	std::string filename = "";
	try {
		for (auto const& dir_entry : fs::directory_iterator("snapshots")) {
			if (!fs::is_regular_file(dir_entry.path()) || 
				dir_entry.path().extension() != ".json")
				continue;

			std::string name = dir_entry.path().stem().string();
			std::string strUnixTimestamp = name.substr(name.find("_") + 1, name.size() - 1);
			int i = std::stoi(strUnixTimestamp);

			if (i > unixTimestamp) {
				unixTimestamp = i;
				filename = fs::absolute(dir_entry.path()).string();
			}
		}
	}
	catch (fs::filesystem_error const& ex) {
		std::cout << "Error occurred during loading latest snapshot. " << ex.what() << std::endl;
		return;
	}

	// Load
	loadState(filename);
}

std::vector<Snapshot> App::listSnapshots() {
	auto stateFilenames = sl::listDirectory("snapshots", ".json");
	auto thumbFilenames = sl::listDirectory("snapshots", ".jpg");

	std::sort(stateFilenames.begin(), stateFilenames.end(), std::greater<std::string>());
	std::sort(thumbFilenames.begin(), thumbFilenames.end(), std::greater<std::string>());
	int size = std::min(stateFilenames.size(), thumbFilenames.size());
	
	std::vector<Snapshot> snapshots;
	for (int i = 0; i < size; ++i) {

		unsigned int tex;
		int w, h, c;
		if (!sl::load_texture_2d(thumbFilenames[i], tex, w, h, c)) {
			std::cerr << "Unable to load snapshot thumb " << thumbFilenames[i] << "." << std::endl;
			continue;
		}
		
		snapshots.push_back(Snapshot{
			.stateFilename = stateFilenames[i],
			.thumbFilename = thumbFilenames[i],
			.image = {
				.texId = tex,
				.width = w,
				.height = h,
				.channels = c
			}
		});
	}

	return snapshots;
}

void App::saveState(const std::string filename) {

	std::filesystem::path p = filename;

	json j;

	// Save app states
	j["header"]["type"] = "state";
	j["cull_mode"] = cull_mode;
	j["selected_model"] = selectedModel;
	j["selected_camera"] = selectedCamera;
	j["models"] = json::object();
	j["cameras"] = json::object();

	// Save models states
	for (auto &[k, m] : models) {
		m->saveState(p.parent_path().string(), j["models"][k]);
	}

	// Save cameras states
	for (auto &[k, c] : cameras) {
		c->saveState(j["cameras"][k]);
	}

	std::ofstream ofs(filename);
	if (!ofs.is_open()) {
		std::cerr << "Failed to open file for saving state: " << filename << std::endl;
		return;
	}

	ofs << j.dump(4); // Pretty print with 4 spaces indentation
	ofs.close();

	std::cout << "State saved to: " << filename << std::endl;
}

void App::clearScene() {

	// // Clear renderers
	// for (auto &[k, r] : renderers)
	// 	r->clear();
	clearRenderers();
	clearModels();
	clearCameras();	
	clearColormaps();
	// TODO clear selected color map... elements etc... layers...
	notifySceneCleared();

	setNavigationPath({});
}

void App::loadState(json &j, const std::string path) {

	clearScene();

	// Load cameras states
	for (auto &[cameraName, jCamera] : j["cameras"].items()) {
		auto type = jCamera["type"].get<std::string>();
		auto camera = cameraInstanciator.make(type);
		if (camera) {
			camera->loadState(jCamera);
			cameras[cameraName] = std::move(camera);
		}
	}

	// Load models states
	for (auto &[modelName, jModel] : j["models"].items()) {
		// Concatenate state.json file path with model path
		// in order to search the mesh file relatively to the state.json file
		std::string modelRelPath = jModel["path"];
		auto modelPath = 
			std::filesystem::path(path).remove_filename() / 
			std::filesystem::path(modelRelPath);
		
		// Try to load the model mesh
		if (loadModel(modelPath.string(), modelName).empty())
			continue;
		
		// Get last added model
		auto &model = models[modelName];
		// Load state into last loaded model
		model->loadState(jModel);

		// TODO! recompute cameras far / near
	}

	// Load app state
	cull_mode = j["cull_mode"].get<int>();
	setSelectedModel(j["selected_model"].get<std::string>());
	setSelectedCamera(j["selected_camera"].get<std::string>());

	std::cout << "State loaded successfully." << std::endl;
}

void App::loadState(const std::string filename) {

	std::ifstream ifs(filename);
	if (!ifs.is_open()) {
		std::cerr << "Failed to open file for loading state: " << filename << std::endl;
		return;
	}

	json j;
	ifs >> j;
	ifs.close();

	std::cout << "Load state from: " << filename << std::endl;

	loadState(j, filename);
}



void App::processInput(GLFWwindow *window) {

	if (ImGui::GetIO().WantCaptureMouse /*&& renderSurfaceWindowHovered*/) {
		return;
	}

	// Get mouse position
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	st.mouse.lastPos = st.mouse.pos;
	st.mouse.pos = glm::vec2(x, y);
	st.mouse.delta = st.mouse.pos - st.mouse.lastPos;

	// Check mouse buttons pressed
	for (int i = 0; i < 8; ++i) {
		auto mouseButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1 + i);
		if (mouseButton == GLFW_PRESS) {
			st.mouse.buttons[i] = true;
		} else if (mouseButton == GLFW_RELEASE) {
			st.mouse.buttons[i] = false;
		}
	}
}

std::unique_ptr<LuaScript> App::loadScript(fs::path scriptPath) {
	std::unique_ptr<LuaScript> script;
	// Check for lua script
	
	if (fs::exists(scriptPath) && fs::is_regular_file(scriptPath)) {
		script = std::make_unique<LuaScript>(*this, scriptPath.string());
		std::cout << "load script: " << scriptPath.string() << std::endl;
	}

	return script;
}

void App::loadCppScript(fs::path scriptPath, sol::state& state) {
	std::cout << "load C++ script: " << scriptPath.filename().string() << std::endl;
	// Load the shared library
	ModuleLoader loader;
	
	auto script = loader.load(scriptPath.string(), *this, state);

	if (script) {
		scripts.push_back(std::move(script));
	} else {
		std::cout << "Failed to load C++ script at: " << scriptPath.string() << std::endl;
	}
}

void App::loadCppScript(fs::path scriptPath) {
	std::cout << "load C++ script: " << scriptPath.filename().string() << std::endl;
	// Load the shared library
	ModuleLoader loader;
	
	auto script = loader.load(scriptPath.string(), *this);

	if (script) {
		scripts.push_back(std::move(script));
	} else {
		std::cout << "Failed to load C++ script at: " << scriptPath.string() << std::endl;
	}
}

void App::loadModule(fs::path m) {

	// Transform relative path to absolute 
	// according to exe location
	if (m.is_relative()) {
		m = fs::path(sl::exePath(m.string()));
	}

	if (!fs::exists(m)) {
		std::cerr << "Module at " << m << " not found." << std::endl;
		return;
	}

	auto scriptPath = fs::path(m) / "script.lua";
	auto script = loadScript(scriptPath);
	
	// Check for so / dll files
	for (const auto& entry : fs::directory_iterator(m)) {
		auto p = entry.path();
		if (entry.is_regular_file() && (p.extension() == ".so" || p.extension() == ".dll")) {
			if (script)
				loadCppScript(p, script->getState());
			else 
				loadCppScript(p);
		}
	}

	// Add scripts
	if (script) {
		script->init();
		scripts.push_back(std::move(script));
	}
}

void App::loadModules(Settings &settings) {
	for (auto m : settings.modules) {
		loadModule(m);
	}
}

void App::key_event(int key, int scancode, int action, int mods) {
	
	bool ctrlPressed = (mods & GLFW_MOD_CONTROL) != 0;
	if (ctrlPressed && key == GLFW_KEY_A && action == GLFW_PRESS) {
		quit();
	}
	
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		screenshot("screenshot.png");
	}

	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		getCurrentCamera().setLock(true);
	} else if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		getCurrentCamera().setLock(false);
	}

	for (auto &script : scripts) {
		script->key_event(key, scancode, action, mods);
	}

}

void App::mouse_scroll(double xoffset, double yoffset) {
	if (_isUIHovered)
		return;

	// Maybe move to a cameracontroller class
	if (!getCurrentCamera().isLocked()) {
		st.mouse.scrollDelta = glm::vec2(xoffset, yoffset);
		getCurrentCamera().zoom(st.mouse.scrollDelta.y);
	}
	else 
		st.mouse.setCursorRadius(st.mouse.getCursorRadius() + static_cast<int>(yoffset));

	for (auto &script : scripts) {
		script->mouse_scroll(xoffset, yoffset);
	}
}

void App::mouse_button(int button, int action, int mods) {

	for (auto &script : scripts)
		script->mouse_button(button, action, mods);
}

void App::mouse_move(double x, double y) {

	long pick_mesh_id = pick_mesh(x, y);
	if (pick_mesh_id >= 0)
		st.mesh.setHovered({pick_mesh_id});
	else 
		st.mesh.setHovered({});

	st.vertex.setHovered(pick_vertices(x, y, st.mouse.getCursorRadius()));
	st.facet.setHovered(pick_facets(x, y, st.mouse.getCursorRadius()));
	st.cell.setHovered(pick_cells(x, y, st.mouse.getCursorRadius()));


	if (glm::dot(st.mouse.pos, st.mouse.lastPos) > 4) {
		auto edge = pickEdge(x, y);
		if (edge >= 0) 
			st.edge.setHovered({edge});
		else
			st.edge.setHovered({});
	}

	for (auto &script : scripts) {
		script->mouse_move(x, y);
	}

}