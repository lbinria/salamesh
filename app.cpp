#include "app.h"

struct UBOMatrices {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
	alignas(16) glm::vec2 viewport;
};

void App::screenshot(const std::string& filename) {
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

    // Save to PNG file
	
    stbi_write_png(filename.c_str(), width, height, 4, flipped_pixels.data(), 0);
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



void App::setup() {

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
		return;
	}
	std::cout << "GLFW window created !" << std::endl;

	// Switch to fullscreen

	// 1) Get window center in desktop coords
	int wx, wy, ww, wh;
	glfwGetWindowPos(window, &wx, &wy);
	glfwGetWindowSize(window, &ww, &wh);
	int cx = wx + ww/2;
	int cy = wy + wh/2;

	int monitorCount = 0;
	GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
	
	// Choose best monitor
	GLFWmonitor* monitor = nullptr;
	for (int i = 0; i < monitorCount; ++i) {
		GLFWmonitor* m = monitors[i];
		int mx, my;
		glfwGetMonitorPos(m, &mx, &my);
		int mw, mh;
		glfwGetMonitorWorkarea(m, &mx, &my, &mw, &mh);

		if (cx >= mx && cx <= mx + mw &&
			cy >= my && cy <= my + mh) {
			// The window center is within the monitor's work area
			monitor = m;
			break;
		}
	}

	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (monitor)
	{
		// Switch to fullscreen on the primary monitor at its current video mode
		glfwSetWindowMonitor(
			window,
			monitor,
			0, 0,
			mode->width,
			mode->height,
			mode->refreshRate
		);

		// Set screen width and height to the monitor's resolution
		screenWidth = mode->width;
		screenHeight = mode->height;
	}

	std::cout << "Screen width: " << screenWidth << ", " << screenHeight << std::endl;

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
		return;
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

	// Load colormap texture
	int width, height, nrChannels;
	sl::load_texture_1d("assets/CET-R41px.png", colormaps[0], width, height, nrChannels);
	sl::load_texture_2d("assets/CET-R41px.png", colormaps2D[0], width, height, nrChannels);
	sl::load_texture_1d("assets/CET-L08px.png", colormaps[1], width, height, nrChannels);
	sl::load_texture_2d("assets/CET-L08px.png", colormaps2D[1], width, height, nrChannels);

	sl::load_texture_1d("assets/colormap_alpha.png", colormaps[2], width, height, nrChannels);
	sl::load_texture_2d("assets/colormap_alpha.png", colormaps2D[2], width, height, nrChannels);

	// Load icons
	int iconWidth, iconHeight, iconChannels;
	sl::load_texture_2d("assets/icons/Eye.png", eyeIcon, iconWidth, iconHeight, iconChannels);
	sl::load_texture_2d("assets/icons/BugAnt.png", bugAntIcon, iconWidth, iconHeight, iconChannels);

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

}

void App::start() {

	Shader screenShader("shaders/screen.vert", "shaders/screen.frag");
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
		view = getCamera().getViewMatrix();
		projection = getCamera().getProjectionMatrix();

		// Update UBO
		UBOMatrices mats{
			view,
			projection,
			{getScreenWidth(), getScreenHeight()}
		};
		
		sl::updateUBOData(uboMatrices, sizeof(UBOMatrices), &mats);

		getRenderSurface().bind();
		getRenderSurface().clear();
		glEnable(GL_DEPTH_TEST);
		glCullFace(cull_mode);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene
		for (auto &model : models) {
			model->setTexture(colormaps[model->getSelectedColormap()]);
			model->render();
		}

		update(dt);

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

		draw_gui();

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

void App::clean() {
	std::cout << "App clean..." << std::endl;

	for (auto &model : models) {
		model->clean();
	}

	for (auto &c : components) {
		c->cleanup();
	}

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	for (auto &rs : renderSurfaces)
		rs->clean();

	// Clear textures
	for (int i = 0; i < IM_ARRAYSIZE(colormaps); ++i)
		glDeleteTextures(1, &colormaps[i]);
	for (int i = 0; i < IM_ARRAYSIZE(colormaps2D); ++i)
		glDeleteTextures(1, &colormaps2D[i]);
}

std::unique_ptr<Model> App::makeModel(ModelType type) {
	switch (type)
	{
	case ModelType::TRI_MESH:
		return std::make_unique<TriModel>();
	case ModelType::QUAD_MESH:
		return std::make_unique<QuadModel>();
	case ModelType::POLYGON_MESH:
		return std::make_unique<PolyModel>();
	case ModelType::TET_MESH:
		return std::make_unique<TetModel>();
	case ModelType::HEX_MESH:
		return std::make_unique<HexModel>();
	case ModelType::POLYLINE_MESH:
	case ModelType::PYRAMID_MESH:
	case ModelType::PRISM_MESH:
		throw std::runtime_error("makeModel for type: " + std::to_string(type) + " not implemented.");
	}

	throw std::runtime_error("makeModel for type: " + std::to_string(type) + " not implemented.");
}

bool App::loadModel(const std::string& filename) {

	// TODO please do something more intelligent here, should deduce mesh type !

	bool success = false;

	std::unique_ptr<Model> model;
	model = std::make_unique<TriModel>();

	success = model->load(filename);

	if (!success) {
		model = std::make_unique<QuadModel>();
		success = model->load(filename);
	}

	if (!success) {
		model = std::make_unique<PolyModel>();
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

	if (!success)
		return false;

	model->setName(std::filesystem::path(filename).stem().string() + std::to_string(models.size()));

	model->setMeshIndex(models.size());

	// Setup default gfx
	model->setLight(true);
	model->getMesh().setMeshShrink(0.f);
	model->getMesh().setMeshSize(0.0f);
	model->setColorMode(ColorMode::COLOR);
	
	auto edges = model->getEdges();
	if (edges)
		edges->setVisible(false);

	// Setup default clipping plane
	model->setupClipping();

	models.push_back(std::move(model));

	// Update cameras far planes
	computeFarPlane();

	// Notify components
	for (auto &c : components) {
		c->modelLoaded(filename);
	}

	return true;
}

void App::computeFarPlane() {
	auto diameter = computeSceneDiameter();

	for (auto &c : cameras) {
		c->setFarPlane(diameter * 5.f /* 5.f is an arbitrary value... */);
	}
}

void App::focus(int modelIdx) {
	auto &model = models[modelIdx];

	setSelectedModel(modelIdx);
	getCamera().lookAtBox(model->bbox());
}

// TODO add model must recompute far plane on loading
int App::addModel(std::string name, ModelType type) {
	auto model = makeModel(type);
	model->setName(name);
	model->setMeshIndex(models.size());

	// 
	// model->loadCallback = ([this](Model&, const std::string) -> bool {
	// 	this->computeFarPlane();
	// 	this->component->modelLoaded(blabla)
	// });

	models.push_back(std::move(model));
	return models.size() - 1;
}

void App::removeModel(int idx) {
	models.erase(models.begin() + idx);

	// Update mesh indexes
	for (int i = idx; i < models.size(); ++i) {
		auto &m = models[i];
		m->setMeshIndex(i);
	}
}

bool App::removeModel(std::string name) {
	for (int i = 0; i < models.size(); ++i) {
		auto &m = models[i];
		if (m->getName() == name) {
			removeModel(i);
			return true;
		}
	}

	return false;
}

std::shared_ptr<Model> App::getModelByName(std::string name) {
	for (auto &m : models) {
		if (m->getName() == name)
			return m;
	}
	return nullptr;
}

int App::getIndexOfModel(std::string name) {
	for (int i = 0; i < models.size(); ++i) {
		auto &m = models[i];
		if (m->getName() == name)
			return i;
	}
	return -1;
}

long App::pick_edge(double x, double y) {
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

	// int h = st.cell.anyHovered() ? st.cell.getHovered() : st.facet.getHovered();

	int h;
	if (st.cell.anyHovered() && (model->getModelType() == ModelType::HEX_MESH || model->getModelType() == ModelType::TET_MESH))
		h = st.cell.getHovered();
	else if (st.facet.anyHovered() && (model->getModelType() == ModelType::TRI_MESH || model->getModelType() == ModelType::QUAD_MESH))
		h = st.facet.getHovered();
	else 
		return -1;

	return model->pick_edge(p, h);
}

long App::pick_mesh(double x, double y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT4);
	long id = pick(x, y);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	return id >= 0 && id < models.size() ? id : -1;
}

std::vector<long> App::pick_vertices(double x, double y, int radius) {
	if (!hasModels())
		return {};

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT3);
	auto ids = pick(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Clean ids
	std::vector<long> clean_ids;
	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
		// TODO getCurrentModel is false !!!! when picking is current picking model instead
		return id >= 0 && id < getCurrentModel().nverts();
	});

	return clean_ids;
}

std::vector<long> App::pick_facets(double x, double y, int radius) {
	if (!hasModels())
		return {};

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	auto ids = pick(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Clean ids
	std::vector<long> clean_ids;
	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
		// TODO getCurrentModel is false !!!! when picking is current picking model instead
		return id >= 0 && id < getCurrentModel().nfacets();
	});

	return clean_ids;
}

std::vector<long> App::pick_cells(double x, double y, int radius) {
	if (!hasModels())
		return {};
		
	glBindFramebuffer(GL_READ_FRAMEBUFFER, getRenderSurface().fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT2);
	auto ids = pick(x, y, radius);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	// Clean ids
	std::vector<long> clean_ids;
	std::copy_if(ids.begin(), ids.end(), std::back_inserter(clean_ids), [&](long id) {
		// TODO getCurrentModel is false !!!! when picking is current picking model instead
		return id >= 0 && id < getCurrentModel().ncells();
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
	glm::mat4 invProj = glm::inverse(getCamera().getProjectionMatrix());
	glm::vec4 viewSpace = invProj * clipSpace;

	// Unproject view space to world space
	glm::mat4 invView = glm::inverse(getCamera().getViewMatrix());
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
	for (auto &m : models) {
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

void App::saveState(const std::string filename) {

	json j;

	// Save app states
	j["header"]["type"] = "state";
	j["cull_mode"] = cull_mode;
	j["selected_model"] = selectedModel;
	j["selected_camera"] = selectedCamera;
	j["models"] = json::array();
	j["cameras"] = json::array();

	// Save models states
	for (int i = 0; i < models.size(); ++i) {		
		models[i]->saveState(j["models"][i]);
	}

	// Save cameras states
	for (int i = 0; i < cameras.size(); ++i) {
		cameras[i]->saveState(j["cameras"][i]);
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
	// TODO clean all scenes properly !
	models.clear();
	cameras.clear();

	selectedCamera = 0;
	selectedModel = 0;
}

void App::loadState(json &j, const std::string path) {

	clearScene();

	// Load cameras states
	for (auto &jCamera : j["cameras"]) {
		auto type = jCamera["type"].get<std::string>();
		auto camera = makeCamera(type);
		if (camera) {
			camera->loadState(jCamera);
			cameras.push_back(std::move(camera));
		}
	}

	// Load models states
	for (auto &jModel : j["models"]) {
		// Concatenate state.json file path with model path
		// in order to search the mesh file relatively to the state.json file
		std::string modelRelPath = jModel["path"];
		auto modelPath = 
			std::filesystem::path(path).remove_filename() / 
			std::filesystem::path(modelRelPath);
		
		// Try to load the model mesh
		if (!loadModel(modelPath.string()))
			continue;
		
		// Get last added model
		auto &model = models.back();
		// Load state into last loaded model
		model->loadState(jModel);

		// TODO! recompute cameras far / near
	}

	// Load app state
	cull_mode = j["cull_mode"].get<int>();
	setSelectedModel(j["selected_model"].get<int>());
	setSelectedCamera(j["selected_camera"].get<int>());

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