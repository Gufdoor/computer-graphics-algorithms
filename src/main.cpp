#include <chrono>
#include <cinttypes>
#include <iostream>
#include <string_view>
#include <thread>
#include <vector>
#include <cmath>

#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer2.h>

static constexpr auto WINDOW_WIDTH = 1280.f;
static constexpr auto WINDOW_HEIGHT = 720.f;


// Structure to store points
struct Point {
    int x, y;
};

// Funções de transformação geométrica
static void translate(Point& p, int tx, int ty) {
    p.x += tx;
    p.y += ty;
}

static void rotate(Point& p, float angle, Point origin) {
    float rad = angle * M_PI / 180.0;
    int x_new = origin.x + (p.x - origin.x) * cos(rad) - (p.y - origin.y) * sin(rad);
    int y_new = origin.y + (p.x - origin.x) * sin(rad) + (p.y - origin.y) * cos(rad);
    p.x = x_new;
    p.y = y_new;
}

static Point calculateCenter(const std::vector<Point>& points) {
    int sumX = 0, sumY = 0;
    for (const auto& p : points) {
        sumX += p.x;
        sumY += p.y;
    }
    int centerX = sumX / points.size();
    int centerY = sumY / points.size();
    return { centerX, centerY };
}

static void scale(Point& p, float sx, float sy, Point origin = { 0, 0 }) {
    p.x = origin.x + (p.x - origin.x) * sx;
    p.y = origin.y + (p.y - origin.y) * sy;
}

static void reflect(Point& p, bool reflectX, bool reflectY) {
    if (reflectX) p.x = -p.x;
    if (reflectY) p.y = -p.y;
}

// Algoritmo de rasterização - Bresenham (Linhas)
static void drawLineBresenham(SDL_Renderer* renderer, Point p1, Point p2) {
    int dx = abs(p2.x - p1.x);
    int dy = abs(p2.y - p1.y);

    int sx = (p1.x < p2.x) ? 1 : -1;
    int sy = (p1.y < p2.y) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        SDL_RenderDrawPoint(renderer, p1.x, p1.y);
        if (p1.x == p2.x && p1.y == p2.y) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            p1.x += sx;
        }
        if (e2 < dx) {
            err += dx;
            p1.y += sy;
        }
    }
}

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    std::cerr << "Error: " << SDL_GetError() << '\n';
    return -1;
  }

  SDL_Window *window = SDL_CreateWindow(
      "Computer Graphics Algorithms", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
      WINDOW_HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == nullptr) {
    SDL_Log("Error creating SDL_Renderer!");
    return 0;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();

  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  // List of points for drawing lines
  std::vector<Point> originalPoints;
  std::vector<Point> transformedPoints;
  bool isDrawing = false;

  bool running = true;

  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);

      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
            event.window.windowID == SDL_GetWindowID(window)) {
          running = false;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
          bool isImGuiInteraction = ImGui::GetIO().WantCaptureMouse;
          bool isLeftButtonClick = event.button.button == SDL_BUTTON_LEFT;

          if (!isImGuiInteraction && isLeftButtonClick) {
			  Point newPoint = { event.button.x, event.button.y };
			  originalPoints.push_back(newPoint);
              isDrawing = true;
          }
          break;
      }
    }

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Graphic Functions");

    // Variables for transformations
    static int tx = 0, ty = 0;
    static float angle = 0.0f;
    static float sx = 1.0f, sy = 1.0f;
    static bool reflectX = false, reflectY = false;

    ImGui::SliderInt("X Translation", &tx, -100, 100);
    ImGui::SliderInt("Y Translation", &ty, -100, 100);
    ImGui::SliderFloat("Rotation", &angle, 0.0f, 360.0f);
    ImGui::SliderFloat("X Scale", &sx, 0.1f, 2.0f);
    ImGui::SliderFloat("Y Scale", &sy, 0.1f, 2.0f);
    ImGui::Checkbox("X Reflection", &reflectX);
    ImGui::Checkbox("Y Reflection", &reflectY);
    if (ImGui::Button("Clear Drawing")) {
        originalPoints.clear();  
        transformedPoints.clear();
    }
    ImGui::End();

    // Processamento de lógica de transformação e rasterização

    // Limpar a tela
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    if (transformedPoints.size() != 0) {
		transformedPoints.clear();
    }

    // Aplicar transformações e desenhar pontos/linhas
    if (originalPoints.size() >= 2) {
        Point center = calculateCenter(originalPoints);

        for (const auto& p : originalPoints) {
            Point transformedPoint = p;

            // Apply transformations
            translate(transformedPoint, tx, ty);
            rotate(transformedPoint, angle, center);  // Rotate around the center of the shape
            scale(transformedPoint, sx, sy, center);  // Scale around the center of the shape
            reflect(transformedPoint, reflectX, reflectY);
            transformedPoints.push_back(transformedPoint);
        }

        // Draw lines between transformed points
        for (size_t i = 0; i < transformedPoints.size() - 1; ++i) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            drawLineBresenham(renderer, transformedPoints[i], transformedPoints[i + 1]);
        }
	}

    // Renderizar a interface ImGui
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

    // Atualizar a tela
    SDL_RenderPresent(renderer);
  }

  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}