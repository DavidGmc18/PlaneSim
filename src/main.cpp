#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <format>

#include "rendering/Shader.hpp"
#include "rendering/light.hpp"
#include "rendering/Model.hpp"
#include "rendering/TextureCache.hpp"
#include "rendering/TextRenderer.hpp"
#include "physics/CONSTANTS.h"
#include "world/World.hpp"
#include "aircraft/F16.hpp"
#include "physics//Wing.hpp"
#include "physics//Engine.hpp"
#include "control/AircraftControls.hpp"
#include "camera/Camera.hpp"
#include "weapon/JDAM.hpp"

int w = 1280;
int h = 720;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 80.0f;

float mouse_sensitivity = 0.1f;
float mouse_scroll_sensitivity = 0.3f;
float speed = 10.0f;

int main() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
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


    std::cout << "Loading...\n";


    SDL_GameController* controller = NULL;
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                std::cout << "Opened controller: " << SDL_GameControllerName(controller) << '\n';
                break;
            }
        }
    }


    GLuint shader = compile_shader_program("shaders/default.vert", "shaders/default.frag");


    TextureCache tex_cache;


    ParallelLight sun(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.3f), glm::vec3(0.5f), glm::vec3(1.5f));


    F16 jet(glm::dvec3(-500.0, 5.0, 0.0), tex_cache);


    JDAM jdam(glm::dvec3(-490.0, 5.0, 0.0), tex_cache);


    TerrainGenerator generator(0.0f, 0.0f, 1.0f);
    World world(1024, generator, tex_cache);


    TextRenderer::init();
    TextRenderer text_renderer("assets/fonts/OpenSans-Regular.ttf", 18.0f, w, h);

    
    AircraftControls controls;


    Camera camera(CameraMode::ORBIT);


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

                case SDL_KEYDOWN:
                    controls.onKeyDown(event.key.keysym.scancode);
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
                    controls.onKeyUp(event.key.keysym.scancode);
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT) {
                        if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
                            SDL_SetRelativeMouseMode(SDL_TRUE);
                        }
                    }
                    break;

                case SDL_MOUSEMOTION:
                    camera.onMouseMove((float)event.motion.xrel * mouse_sensitivity, (float)event.motion.yrel * mouse_sensitivity);
                    break;
                    
                case SDL_MOUSEWHEEL:
                    camera.onMouseScroll((float)event.wheel.y * mouse_scroll_sensitivity);
                    break;

                case SDL_CONTROLLERAXISMOTION:
                    controls.onControllerAxis(event.caxis);
                    break;

                case SDL_CONTROLLERBUTTONDOWN:
                    controls.onControllerButtonDown((SDL_GameControllerButton)event.cbutton.button);
                    break;

                case SDL_CONTROLLERBUTTONUP:
                    controls.onControllerButtonUp((SDL_GameControllerButton)event.cbutton.button);
                    break;
            }
        }


        controls.update(frame_time);


        jet.update(&world, frame_time, &controls);
        jdam.update(&world, frame_time);


        glDepthMask(GL_TRUE);
        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);
        
        camera.setTarget(jet.getPosition());
        camera.update(frame_time);
        glm::vec3 camera_pos = camera.getPosition();
        glUniform3f(glGetUniformLocation(shader, "cameraPos"), camera_pos.x, camera_pos.y, camera_pos.z);

        glm::dmat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix((float)w / (float)h);

    // Light
        sun.use(shader);
        Light::setCount(shader, 0);
        

    // Opaque rendering
        glEnable(GL_CULL_FACE);

        jet.drawOpaque(shader, view, projection);
        jdam.drawOpaque(shader, view, projection);

        world.draw(shader, view, projection);


    // Transparent rendering
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);

        jet.drawTransparent(shader, view, projection);
        jdam.drawTransparent(shader, view, projection);


    // Debug
        glm::dvec3 pos = jet.getPosition();
        text_renderer.render(std::format("Pos X: {:10.3f}m  Y: {:10.3f}m  Z: {:10.3f}m", pos.x, pos.y, pos.z), glm::vec2(10, 18), glm::vec4(1));

        glm::vec3 vel = jet.getVelocity();
        text_renderer.render(std::format("Vel X: {:10.3f}m/s  Y: {:10.3f}m/s  Z: {:10.3f}m/s  A: {:8.1f}m/s ({:04.2f}M)", vel.x, vel.y, vel.z, glm::length(vel), glm::length(vel) / 343.0f), glm::vec2(10, 36), glm::vec4(1));

        glm::vec3 a = jet.getAcceleration();
        text_renderer.render(std::format("Acc X: {:+5.1f}G  Y: {:+5.1f}G  Z: {:+5.1f}G  A: {:+5.1f}G", a.x/STANDARD_GRAVITY, a.y/STANDARD_GRAVITY, a.z/STANDARD_GRAVITY, glm::length(a)/STANDARD_GRAVITY), glm::vec2(10, 54), glm::vec4(1));

        std::span<const VirtualAxis> axes = controls.getAxes();
        text_renderer.render(
            std::format(
                "PITCH {:+6.3f}  ROLL {:+6.3f}  YAW {:+6.3f}  THROTTLE {:4.1f}",
                axes[AircraftControls::PITCH].absolute,
                axes[AircraftControls::ROLL].absolute,
                axes[AircraftControls::YAW].absolute,
                axes[AircraftControls::THROTTLE].absolute * 100.0f
            ),
            glm::vec2(10, 72), glm::vec4(1)
        );

        int y = 90;
        std::span<const PhysicPart* const> parts = jet.getPhysicParts();
        for (const PhysicPart* const part : parts) {
            if (const Wing* wing = dynamic_cast<const Wing*>(part)) {
                AirfoilSample as = wing->getAirfoilSample();
                text_renderer.render(
                    std::format(
                        "{} - Alpha={:+5.1f}deg  Veff={:5.1f}m/s  Cl={:+6.3f}  Cd={:5.3f}  Cm={:+6.3f}",
                        wing->getName(), wing->getAlpha(), wing->getVeff(), as.cl, as.cd, as.cm
                    ),
                    glm::vec2(10, y), glm::vec4(1)
                );
                y += 18;
            } else if (const Engine* engine = dynamic_cast<const Engine*>(part)) {
                text_renderer.render(
                    std::format("Engine: RPM={:04.1f}%", engine->getRPM() * 100.0f),
                    glm::vec2(10, y), glm::vec4(1)
                );
                y += 18;
            }
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