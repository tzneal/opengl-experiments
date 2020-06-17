#include "engine.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#include <engine/shader/glerror.h>
#include <glad/glad.h>
#include <imgui/examples/imgui_impl_opengl3.h>
#include <imgui/examples/imgui_impl_sdl.h>
#include <spdlog/spdlog.h>

#include <nanovg.h>
#include <nanovg_gl.h>

sigslot::signal<> engine::on_glcontext_created;
sigslot::signal<int, int> engine::on_window_resized;

using namespace engine;

bool Engine::init(const EngineInitParams &params) {
  if (!init_video(params)) {
    return false;
  }

  this->params = params;

  return true;
}

bool Engine::init_video(const EngineInitParams &params) {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    spdlog::error("error initializing SDL: {}", SDL_GetError());
    return false;
  }

  if (!init_opengl()) {
    spdlog::error("error initializing OpenGL");
    return false;
  }

  SDL_WindowFlags windowFlags =
      (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  window = SDL_CreateWindow(params.WindowTitle.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, params.Width, params.Height, windowFlags);
  if (!window) {
    spdlog::error("error creating window: {}", SDL_GetError());
    return false;
  }

  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
    spdlog::warn("error setting scale quality");
  }

  // limit minimum window size
  SDL_SetWindowMinimumSize(window, 500, 300);

  if (!init_openglcontext(params)) {
    spdlog::error("error initializing OpenGL context");
    return false;
  }
  if (!init_openglmatrices(params)) {
    spdlog::error("error initializing OpenGL matrices");
    return false;
  }

  if (!init_imgui()) {
    spdlog::error("error initializing ImGui");
    return false;
  }

  if (!init_nanovg()) {
    spdlog::error("error initializing NanoVG");
    return false;
  }

  // Initialize PNG loading
  int imgFlags = IMG_INIT_PNG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    spdlog::error("SDL_image could not initialize: {}", IMG_GetError());
    return false;
  }

  on_glcontext_created();
  return true;
}

bool Engine::init_opengl() {  // set OpenGL attributes
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

  glsl_version = "";
#ifdef __APPLE__
  // GL 3.2 Core + GLSL 150
  glsl_version = "#version 150";
  SDL_GL_SetAttribute(  // required on Mac OS
      SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif __linux__
  // GL 3.2 Core + GLSL 140
  glsl_version = "#version 300 es";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#elif _WIN32
  // GL 3.0 + GLSL 130
  glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#endif
  return true;
}

bool Engine::init_imgui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // setup Dear ImGui style
  ImGui::StyleColorsDark();

  // setup platform/renderer bindings
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());

  return true;
}

Engine::~Engine() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  SDL_GL_DeleteContext(gl_context);

  if (window) {
    SDL_DestroyWindow(window);
  }

  IMG_Quit();
  SDL_Quit();
}
void Engine::render_frame() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glm::vec2 screen_size{params.Width, params.Height};
  nvgBeginFrame(nvg, params.Width, params.Height, 1.0);
  for (auto &sys : render_systems) {
    sys->render(registry, screen_size);
  }
  nvgEndFrame(nvg);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  ImGui::NewFrame();

  for (auto &ui : ui_systems) {
    ui->render_ui(registry);
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  SDL_GL_SwapWindow(window);
}

bool Engine::running() { return !shutdown; }

void Engine::poll() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type) {
      case SDL_QUIT:
        shutdown = true;
        break;

      case SDL_WINDOWEVENT: {
        const SDL_WindowEvent &wev = event.window;
        switch (wev.event) {
          case SDL_WINDOWEVENT_RESIZED:
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            params.Width = event.window.data1;
            params.Height = event.window.data2;
            glViewport(0, 0, params.Width, params.Height);
            on_window_resized(params.Width, params.Height);
            break;

          case SDL_WINDOWEVENT_CLOSE:
            shutdown = true;
            break;
        }
      } break;
    }
  }
}

void Engine::update() {
  auto currentTicks = SDL_GetTicks();
  if (previousTicks == 0) {
    previousTicks = currentTicks;
  }
  auto deltaTicks = currentTicks - previousTicks;
  auto deltaTime = deltaTicks / 1000.0f;
  previousTicks = currentTicks;
  glm::vec2 screen_size{params.Width, params.Height};
  for (auto &sys : update_systems) {
    sys->update(registry, screen_size, deltaTime);
  }
}
void Engine::add_ui_system(std::shared_ptr<UISystem> &&system) {
  ui_systems.emplace_back(std::move(system));
}

void Engine::add_update_system(std::shared_ptr<UpdateSystem> &&system) {
  update_systems.emplace_back(std::move(system));
}

void Engine::add_render_system(std::shared_ptr<RenderSystem> &&system) {
  auto nvgPt = std::dynamic_pointer_cast<NVGSystem>(system);
  if (nvgPt) {
    nvgPt->set_context(nvg);
  }
  render_systems.emplace_back(std::move(system));
}

entt::entity Engine::create_entity() { return registry.create(); }

bool Engine::init_openglcontext(const EngineInitParams initParams) {
  gl_context = SDL_GL_CreateContext(window);
  SDL_GL_MakeCurrent(window, gl_context);

  // enable VSync
  SDL_GL_SetSwapInterval(1);
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    spdlog::error("couldn't initialize glad");
    return false;
  }

  glViewport(0, 0, params.Width, params.Height);
  on_window_resized(params.Width, params.Height);

  return true;
}

void APIENTRY glDebugMsg(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length​,
                         const GLchar *message​, const void *userParam​) {
  spdlog::info("{} {} {}: {}", source, type, id, message​);
}

bool Engine::init_openglmatrices(const EngineInitParams &params) {
  // Initialize Projection Matrix
  /*  glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, params.Width, params.Height, 0.0, 1.0, -1.0);

  // Initialize Modelview Matrix
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
*/
  // Initialize clear color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClearDepth(1.0f);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_STENCIL_TEST);
  if (debug) {
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glDebugMsg, nullptr);
  }

  // Set blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Check for error
  GLenum error = glGetError();
  if (error != GL_NO_ERROR) {
    spdlog::error("error initializing OpenGL {}", glErrorString(error));
    return false;
  }
  return true;
}
entt::registry &Engine::get_registry() { return registry; }
bool Engine::init_nanovg() {
  int flags = NVG_ANTIALIAS | NVG_STENCIL_STROKES;
  if (debug) {
    flags |= NVG_DEBUG;
  }
  nvg = nvgCreateGL3(flags);
  if (!nvg) {
    return false;
  }
  return true;
}
void Engine::set_clear_color(const glm::vec3 &color) {
  clear_color = color;
  if (window) {
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
  }
}
