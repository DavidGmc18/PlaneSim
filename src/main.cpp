#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <format>

#include "shader.hpp"
#include "Camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "TextureCache.hpp"
#include "TextRenderer.hpp"
#include "KeyHandler.hpp"
#include "CONSTANTS.h"

#include "world/World.hpp"

#include "vehicle/F16.hpp"

int w = 1280;
int h = 720;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 80.0f;

float mouse_sensitivity = 0.1f;
float mouse_scroll_sensitivity = 0.3f;
float speed = 10.0f;

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);
    SDL_Window* window = SDL_CreateWindow("PlaneSim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    SDL_GL_SetSwapInterval(-1);
    // SDL_SetRelativeMouseMode(SDL_TRUE);

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    std::cout << "Loading assets, please wait...\n";

    GLuint shader = compile_shader_program("shaders/default.vert", "shaders/default.frag");


    TextureCache tex_cache;


    ParallelLight sun(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.3f), glm::vec3(0.5f), glm::vec3(1.5f));


    F16 jet(glm::vec3(-500.0f, 5.0f, 0.0f), tex_cache);


    TerrainGenerator generator(0.0f, 0.0f, 1.0f);
    World world(1024, generator, tex_cache);


    TextRenderer::init();
    TextRenderer text_renderer("assets/fonts/OpenSans-Regular.ttf", 18.0f, w, h);

    KeyHandler key_handler;

    SDL_ShowWindow(window);

    float frame_start = SDL_GetTicks() / 1000.0f;
    bool running = true;
    bool isFullscreen = false;
    while (running) {    
        float now = SDL_GetTicks() / 1000.0f;
        float frame_time = now - frame_start;
        frame_start = now;

        SDL_Event event;
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    key_handler.onKeyDown(event.key.keysym.scancode);

                    switch (event.key.keysym.sym) {
                        case SDLK_F11:
                            isFullscreen = !isFullscreen;
                            SDL_SetWindowFullscreen(window, isFullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                            SDL_GL_SetSwapInterval(-1);
                            break;

                        case SDLK_ESCAPE:
                            SDL_SetRelativeMouseMode(SDL_FALSE);
                            SDL_ShowCursor(SDL_ENABLE);
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    key_handler.onKeyUp(event.key.keysym.scancode);

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
                            SDL_SetRelativeMouseMode(SDL_TRUE);
                        }
                    }
                    break;

                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        w = event.window.data1;
                        h = event.window.data2;
                        glViewport(0, 0, w, h);
                        text_renderer.onScreenResize(w, h);
                    } else if (event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) {
                        SDL_GL_SetSwapInterval(-1);
                    }
                    break;

                case SDL_MOUSEMOTION:
                    jet.onMouseMove((float)event.motion.xrel * mouse_sensitivity, (float)event.motion.yrel * mouse_sensitivity);
                    break;
                    
                case SDL_MOUSEWHEEL:
                    jet.onMouseScroll((float)event.wheel.y * mouse_scroll_sensitivity);
                    break;
            }
        }


        jet.update(frame_time, &world, key_handler);


        glDepthMask(GL_TRUE);
        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        jet.useCamera(shader, (float)w / (float)h);

        sun.use(shader);
        Light::setCount(shader, 0);
        

    // Opaque rendering
        glEnable(GL_CULL_FACE);

        jet.drawOpaque(shader);

        world.draw(shader);


    // Transparent rendering
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        jet.drawTransparent(shader);


    // Debug
        glm::vec3 pos = jet.getPosition();
        text_renderer.render(std::format("Pos X: {:10.3f}m  Y: {:10.3f}m  Z: {:10.3f}m", pos.x, pos.y, pos.z), glm::vec2(10, 18), glm::vec4(1));

        glm::vec3 vel = jet.getVelocity();
        text_renderer.render(std::format("Vel X: {:10.3f}m/s  Y: {:10.3f}m/s  Z: {:10.3f}m/s", vel.x, vel.y, vel.z), glm::vec2(10, 36), glm::vec4(1));

        glm::vec3 a = jet.getAcceleration();
        text_renderer.render(std::format("Acc X: {:+5.1f}G  Y: {:+5.1f}G  Z: {:+5.1f}G", a.x/STANDARD_GRAVITY, a.y/STANDARD_GRAVITY, a.z/STANDARD_GRAVITY), glm::vec2(10, 54), glm::vec4(1));

        int y = 72;
        const std::vector<Engine>& engines = jet.getEngines();
        for (int i = 0; i < engines.size(); i++) {
            text_renderer.render(
                std::format("Engine {}: THR={:04.1f}%  N1={:04.1f}%", i+1,  engines[i].getThrottle() * 100.0f, engines[i].getRPM() * 100.0f),
                glm::vec2(10, y), glm::vec4(1)
            );
            y += 18;
        }

        const std::vector<Wing>& wings = jet.getWings();
        for (const Wing& wing : wings) {
            AirfoilSample as = wing.getAirfoilSample();
            text_renderer.render(
                std::format(
                    "{} - Alpha={:+5.1f}deg  Veff={:5.1f}m/s  Cl={:+6.3f}  Cd={:5.3f}  Cm={:+6.3f}",
                    wing.getName(), wing.getAlpha(), wing.getVeff(), as.cl, as.cd, as.cm
                ),
                glm::vec2(10, y), glm::vec4(1)
            );
            y += 18;
        }


        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout << "GL error: " << err << '\n';

        SDL_GL_SwapWindow(window);
    }

    TextRenderer::terminate();

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}