#include "app.h"

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

static void load_texture_1d(const std::string &path, unsigned int & texture, int & width, int & height, int & nChannels) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_1D, texture);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *new_data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
	
	if (new_data) {
		std::cout << "Load: " << path << ", w: " << width << ", h: " << height << ", channels: " << nChannels << std::endl;
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, width, 0, GL_RGB, GL_UNSIGNED_BYTE, new_data);
		
		// glGenerateMipmap(GL_TEXTURE_1D);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_1D, texture);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(new_data);
}

static void load_texture_2d(const std::string &path, unsigned int & texture, int & width, int & height, int & nChannels) {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char *new_data = stbi_load(path.c_str(), &width, &height, &nChannels, 0);
	
	if (new_data) {
		std::cout << "w: " << width << ", h: " << height << ", channels: " << nChannels << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, new_data);
		
		// glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(new_data);
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

	app->_cur_button = action == GLFW_PRESS ? button : -1;

	// Redirect to virtual
	app->mouse_button(button, action, mods);
}

static void set_cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (!app)
		return;
	
	// Redirect to virtual
	app->mouse_move(xpos, ypos);

	if (app->_cur_button >= 0) {
		app->mouse_drag(app->_cur_button, xpos, ypos);
	}
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));

    if (!app)
		return;

	app->SCR_WIDTH = width;
	app->SCR_HEIGHT = height;

	glViewport(0, 0, width, height);
    
	// Update color attachment texture
    glBindTexture(GL_TEXTURE_2D, app->colorAttachmentTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glBindTexture(GL_TEXTURE_2D, app->screenColorAttachmentTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    
    // Update depth/stencil renderbuffer
	glBindRenderbuffer(GL_RENDERBUFFER, app->rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, app->screenRbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    
    // Verify framebuffer completeness
    glBindFramebuffer(GL_FRAMEBUFFER, app->screenFbo);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	app->camera->SetScreenSize(width, height);
}

// TODO be able to load tet, surf, etc
void App::load_model(const std::string& filename) {
	#ifdef _DEBUG
	std::cout << "read model..." << std::endl;
	std::chrono::steady_clock::time_point begin_read_model = std::chrono::steady_clock::now();
	#endif

	/*UM::VolumeAttributes*/ attributes = UM::read_by_extension(filename, hex);

	std::cout << "number of point attributes: " << attributes.points.size() << std::endl;
	std::cout << "nverts = " << hex.nverts() << std::endl;
	std::cout << "nfacets = " << hex.nfacets() << std::endl;
	std::cout << "model read." << std::endl;
	attrs.clear();

	for (auto a : attributes.points) {
		attrs.push_back({Element::POINTS, a.first});
	}
	for (auto a : attributes.cell_corners) {
		attrs.push_back({Element::CORNERS, a.first});
	}
	for (auto a : attributes.cell_facets) {
		attrs.push_back({Element::FACETS, a.first});
	}
	for (auto a : attributes.cells) {
		attrs.push_back({Element::CELLS, a.first});
	}

    // mesh = std::make_unique<HexRenderer>(hex, attributes);
    Shader shader("shaders/hex.vert", "shaders/hex.frag");
    hex_renderer = std::make_unique<HexRenderer>(shader);
    // mesh = std::make_shared<HexRenderer>(shader);
	// mesh->setAttributes(attributes);
	hex_renderer->to_gl(hex);


	#ifdef _DEBUG
	std::chrono::steady_clock::time_point end_read_model = std::chrono::steady_clock::now();
    std::cout << "read model in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_read_model - begin_read_model).count() << "ms" << std::endl;
	#endif
}

void App::run()
{
	std::cout << "App run !" << std::endl;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	// projection = glm::perspective(glm::radians(45.f), (float)SCR_WIDTH/(float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);

	std::chrono::steady_clock::time_point begin_setup = std::chrono::steady_clock::now();


	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// MSAA
	glfwWindowHint(GLFW_SAMPLES, 8);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mesh", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	std::cout << "GLFW window created !" << std::endl;


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
    // ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}    

	std::chrono::steady_clock::time_point end_setup = std::chrono::steady_clock::now();
    std::cout << "GLFW / ImGui setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_setup - begin_setup).count() << "ms" << std::endl;


	// OpenGL
	glEnable(GL_MULTISAMPLE);  

	auto gl_version = glGetString(GL_VERSION);
	std::cout << "OpenGL: " << gl_version << std::endl;

	GLint64 maxUnits;
	glGetInteger64i_v(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0, &maxUnits);
	std::cout << "Max texture units: " << maxUnits << std::endl;

	// Load colormap texture
	int width, height, nrChannels;
	load_texture_1d("assets/CET-R41px.png", colormaps[0], width, height, nrChannels);
	load_texture_2d("assets/CET-R41px.png", colormaps2D[0], width, height, nrChannels);
	load_texture_1d("assets/CET-L08px.png", colormaps[1], width, height, nrChannels);
	load_texture_2d("assets/CET-L08px.png", colormaps2D[1], width, height, nrChannels);

	// Framebuffer !
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	// Framebuffer texture
	glGenTextures(1, &colorAttachmentTexture);
	glBindTexture(GL_TEXTURE_2D, colorAttachmentTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);
	// Attach to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachmentTexture, 0);

	// Create depth texture
	glGenTextures(1, &depthAttachmentTexture);
	glBindTexture(GL_TEXTURE_2D, depthAttachmentTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// Attach to FBO
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachmentTexture, 0);

	// unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Main framebuffer is not complete!" << std::endl;

	// glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Framebuffer !
	// colorFbo;
	glGenFramebuffers(1, &screenFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);

	// Framebuffer texture
	// unsigned int colorTextureColorBuffer;
	glGenTextures(1, &screenColorAttachmentTexture);
	glBindTexture(GL_TEXTURE_2D, screenColorAttachmentTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenColorAttachmentTexture, 0);

	// colorRbo;
	glGenRenderbuffers(1, &screenRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, screenRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, screenRbo);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Screen framebuffer is not complete!" << std::endl;

	// glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

	load_model("assets/catorus_hex_attr.geogram");
	// load_model("assets/joint.geogram");

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "preload in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;

	

	mesh_ps = std::make_unique<PointsCloud>(hex.points);
	mesh_ps->to_gl();
	mesh_ps->init();


	hex_renderer->setLight(isLightEnabled);
	hex_renderer->setMeshShrink(meshShrink);
	hex_renderer->setMeshSize(meshSize);
	hex_renderer->setColorMode(ColorMode::COLOR);

	// Model init
	hex_renderer->init(hex);

	Shader screenShader("shaders/screen.vert", "shaders/screen.frag");

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
	unsigned int quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	
	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);

	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    screenShader.use();
    screenShader.setInt("screenTexture", 0);


	// vec3 center;
	// for (auto v : hex.iter_vertices()) {
	// 	center += v.pos();
	// }
	// center /= hex.nverts();

	// camera = std::make_unique<ArcBallCamera>(glm::vec3(0.f, 0.f, -3.f), mesh->position, glm::vec3(0.f, 1.f, 0.f), projection);
	camera = std::make_unique<ArcBallCamera>(glm::vec3(0.f, 0.f, -3.f), hex_renderer->position, glm::vec3(0.f, 1.f, 0.f), glm::vec3(45.f, SCR_WIDTH, SCR_HEIGHT));

	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Init inherited class 
	init();

	glm::mat4 model(1.f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.f, 0.f, 0.f));

	glm::mat4 view(1.f);
	view = glm::translate(view, glm::vec3(0.f, 0.f, -3.f));

	float lastTimeValue = glfwGetTime();

	while(!glfwWindowShouldClose(window)) {

		// Compute dela time
		float timeValue = glfwGetTime();
		float dt = timeValue - lastTimeValue;

		processInput(window);

		update(dt);


		if (leftMouse) {
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);
			camera->Move(glm::vec2(SCR_WIDTH, SCR_HEIGHT), glm::vec2(xPos, yPos), lastMousePos);
			// camera.SmoothMove(glm::vec2(SCR_WIDTH, SCR_HEIGHT), glm::vec2(xPos, yPos), lastMousePos);
			lastMousePos = glm::vec2(xPos, yPos);

			auto pickIDs = pick(window, xPos, yPos, cursor_radius);
			for (long pickID : pickIDs) {
				if (camera->IsLocked() && pickID >= 0 && pickID < hex.ncells()) {
					// mesh->setHighligth(pickID, 1.f);
					hex_renderer->setFilter(pickID, true);
				}
			}
		} else {
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);

			// std::vector<bool> h(hex.ncells(), false);
			// for (auto lh : _lastHovered) {
			// 	h[lh] = false;
			// }
			// mesh->setHighlight(h);

			auto pickIDs = pick(window, xPos, yPos, cursor_radius);
			for (long pickID : pickIDs) {

				if (camera->IsLocked() && pickID >= 0 && pickID < hex.ncells()) {
					_lastHovered.push_back(pickID);
					// mesh->setHighlight(pickID, 0.2f);
					// mesh->setFilter(pickID, true);
				}
			}
		}

		if (rightMouse) {
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);
			camera->MovePlane(glm::vec2(xPos, yPos) - lastMousePos2);
			lastMousePos2 = glm::vec2(xPos, yPos);
		}

		if (dblClick) {
			double xPos, yPos;
			glfwGetCursorPos(window, &xPos, &yPos);
			long pickID = pick(xPos, yPos);

			if (pickID > 0 && pickID < hex.ncells()) {
				Volume::Cell c(hex, pickID);
				auto p = 
					(c.vertex(0).pos() + 
					c.vertex(1).pos() +
					c.vertex(2).pos() +
					c.vertex(3).pos() +
					c.vertex(4).pos() +
					c.vertex(5).pos() +
					c.vertex(6).pos() +
					c.vertex(7).pos()) / 8.;

				camera->LookAt(glm::vec3(p.x, p.y, p.z));
				// dist
				// float targetDist = glm::distance(camera->GetEye(), glm::vec3(p.x, p.y, p.z));
				// camera->MoveForward(targetDist);
				std::cout << "dblClick on cell: " << pickID << std::endl;
			}
		}
		

		// camera->NewZoom(scrollDelta.y);
		// scrollDelta = glm::vec2(0, 0);
		
		view = camera->GetViewMatrix();
		projection = camera->GetProjectionMatrix();
		


		// Go to color framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.05, 0.1, 0.15, 1.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_1D, colormaps[selectedColormap]);
		glCullFace(cull_mode);

		// Render surface
        hex_renderer->bind();
		hex_renderer->setFragRenderMode(RenderMode::Color);
		hex_renderer->shader.setMat4("view", view);
		hex_renderer->shader.setMat4("projection", projection);
        hex_renderer->render();

		// Render points
		mesh_ps->bind();
		mesh_ps->shader.setMat4("view", view);
		mesh_ps->shader.setMat4("projection", projection);
		glm::mat4 inverse_projection_view_model = glm::inverse(projection * view * model);
		glm::mat4 mvp = projection * view * model;
		mesh_ps->shader.setMat4("inverse_projection_view_model", inverse_projection_view_model);
		mesh_ps->shader.setMat4("model_view_projection_matrix", mvp);
		// mesh_ps->render();

        // Go to ID framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, screenFbo);
		glEnable(GL_DEPTH_TEST);
		glClearColor(1.f, 1.f, 0.5f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(cull_mode);
        hex_renderer->bind();
		hex_renderer->shader.use();
        hex_renderer->shader.setInt("fragRenderMode", pickMode);
        hex_renderer->render();
		hex_renderer->shader.setMat4("view", view);


		// DRAW SCREEN !
		// Go back to default framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(quadVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, colorAttachmentTexture);
		// glBindTexture(GL_TEXTURE_2D, depthTexture);

		glCullFace(GL_BACK);

		screenShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw UI
		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		
		ImGui::NewFrame();
		
		draw_gui();

		// Render the ImGui frame
		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers and get IO
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		lastTimeValue = timeValue;
	}

	// Clean up
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);

	glDeleteRenderbuffers(1, &rbo);
	glDeleteFramebuffers(1, &fbo);

	for (int i = 0; i < IM_ARRAYSIZE(colormaps); ++i)
		glDeleteTextures(1, &colormaps[i]);
	for (int i = 0; i < IM_ARRAYSIZE(colormaps2D); ++i)
		glDeleteTextures(1, &colormaps2D[i]);

	hex_renderer->clean();
	mesh_ps->clean();

	// // Free callback
	// // Setup inputs
	// glfwSetKeyCallback(window, NULL);
	// glfwSetScrollCallback(window, NULL);
	// glfwSetMouseButtonCallback(window, NULL);
	// glfwSetCursorPosCallback(window, NULL);  

	// Terminate & quit
	glfwTerminate();
	return;
}

int App::getWidth() {
	return SCR_WIDTH;
}

int App::getHeight() {
	return SCR_HEIGHT;
}

long App::pick_cell() {
	auto last_pick_mode = pickMode;
	pickMode = Element::CELLS;
	long id = pick(mousePos.x, mousePos.y);
	pickMode = last_pick_mode;
	// TODO should I check that here ?
	return id >= 0 && id < hex.ncells() ? id : -1;
}


// float getLinearDepth(float depth, float near, float far) {
//     return (2.0f * near * far) / (far + near - depth * (far - near));
// }
float getLinearDepth(float depth, float near, float far) {
	return near * far / (far + depth * (near - far));
}

float App::get_depth(double x, double y) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    float depth;
    glReadPixels(x, SCR_HEIGHT - y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
    return depth;
}

// unsigned char App::get_depth2(double x, double y) {
// 	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
//     unsigned char depth;
//     glReadPixels(x, SCR_HEIGHT - y, 1, 1, GL_RED, GL_UNSIGNED_BYTE, &depth);
//     return depth;
// }

void getNDC(int x, int y, int w, int h, float &ndcX, float &ndcY) {
	ndcX = (2.f*x) / w - 1.0f;
	ndcY = 1.0f - (2.f*y) / h;
}

void App::unproject(int x, int y, float depth, glm::vec3 &p) {
	
	// Screen coordinates to NDC
	float ndcX, ndcY;
	getNDC(x, y, SCR_WIDTH, SCR_HEIGHT, ndcX, ndcY);

	// Clip space coordinates
	glm::vec4 clipSpace(ndcX, ndcY, depth, 1.0f);

	// Unproject clip space to view space
	glm::mat4 invProj = glm::inverse(camera->GetProjectionMatrix());
	glm::vec4 viewSpace = invProj * clipSpace;

	// Unproject view space to world space
	glm::mat4 invView = glm::inverse(camera->GetViewMatrix());
	glm::vec4 worldSpace = invView * (viewSpace / viewSpace.w);
	p = glm::vec3(worldSpace);
}

glm::vec3 App::pick_point(double x, double y) {
	// Read depth value
    float depth = get_depth(x, y);
	depth = depth * 2.f - 1.f; // Convert to NDC range [-1, 1]
    // float zCoordinate = getLinearDepth(depth, NEAR_PLANE, FAR_PLANE);
	// std::cout << "z: " << zCoordinate << std::endl;

	glm::vec3 p;
	unproject(x, y, depth, p);
	return p;
}


long App::pick(double x, double y) {
	if (glm::length(lastMousePos) < 0.01)
		lastMousePos = glm::vec2(x, y);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, screenFbo);
	
	unsigned char pixel[4];
	glReadPixels(x, SCR_HEIGHT - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	long pickID = pixel[3] == 0 ? -1 :
		pixel[0] +
		pixel[1] * 256 +
		pixel[2] * 256 * 256;

	// std::cout << 
	// 	static_cast<int>(pixel[0]) << "," << 
	// 	static_cast<int>(pixel[1]) << "," << 
	// 	static_cast<int>(pixel[2]) << "," << 
	// 	static_cast<int>(pixel[3]) << 
	// std::endl;

	// std::cout << "pick id: " << pickID << std::endl;
	// std::cout << "---" << std::endl;
	return pickID;
}

std::vector<long> App::pick(GLFWwindow *window, double xPos, double yPos, int radius) {

	glBindFramebuffer(GL_READ_FRAMEBUFFER, screenFbo);
	const int diameter = radius * 2 + 1;
	std::vector<long> pickIDs;

	// Allocate buffer for square that bounds our circle
	unsigned char* pixelData = new unsigned char[diameter * diameter * 4];

	// Read pixels in square that bounds our circle
	glReadPixels(
		xPos - radius,
		SCR_HEIGHT - yPos - radius,
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

				if(pickID != -1) {
					pickIDs.push_back(pickID);
				}
			}
		}
	}

	delete[] pixelData;
	return pickIDs;
}

void App::reset_zoom() {
	// TODO add lua cmd
	camera->SetFov(45.f);
	camera->UpdateViewMatrix();
}

void App::look_at_center() {
	// TODO add lua cmd
	camera->LookAt(glm::vec3(0.f, 0.f, 0.f));
}

    
void App::setLight(bool enabled) {
	isLightEnabled = enabled;
	hex_renderer->setLight(enabled);
}

void App::setClipping(bool enabled) {
	isClippingEnabled = enabled;
	hex_renderer->setClipping(enabled);
}

void App::processInput(GLFWwindow *window) {

	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);
	mousePos = glm::vec2(xPos, yPos);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		leftMouse = true;
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		

		// Check whether is the first occurrence
		if (glm::length(lastMousePos) < 0.01)
			lastMousePos = glm::vec2(xPos, yPos);


	} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		// Was a click ?
		if (leftMouse) {
			auto now = std::chrono::steady_clock::now();
			auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClick).count();
			dblClick = delta < _dbl_click_interval;
			if (!dblClick)
				lastClick = now;
		} else 
			dblClick = false;


		leftMouse = false;
		lastMousePos = glm::vec2(0, 0);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
		rightMouse = true;
		double xPos, yPos;
		glfwGetCursorPos(window, &xPos, &yPos);
		if (glm::length(lastMousePos2) < 0.01)
			lastMousePos2 = glm::vec2(xPos, yPos);

	} else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		rightMouse = false;
		lastMousePos2 = glm::vec2(0, 0);		
	}

}