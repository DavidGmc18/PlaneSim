#include <iostream>
#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "light.hpp"
#include "model.hpp"
#include "TextureCache.hpp"

#include "world/World.hpp"

#include "vehicle/F16.hpp"

int w = 1280;
int h = 720;

float yaw = -90.0f;
float pitch = 0.0f;
float fov = 80.0f;

float mouse_sensitivity = 0.1f;
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

    std::cout << "Loading assets, please wait...\n";

    SDL_GL_SetSwapInterval(-1);

    // SDL_SetRelativeMouseMode(SDL_TRUE);

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    Camera camera(glm::vec3(0.0f, 0.2f, 7.0f), yaw, pitch, fov, 0.001f, 256.0f, (float)w / (float)h);


    GLuint shader = compile_shader_program(SHADER_PATH "default.vert", SHADER_PATH "default.frag");


    TextureCache tex_cache;


    ParallelLight sun(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.3f), glm::vec3(0.5f), glm::vec3(1.5f));


    F16 jet(glm::vec3(0.0f, 5.0f, 0.0f), tex_cache);


    TerrainGenerator generator(0.0f, 5.0f, 1.0f);
    World world(1024, generator, tex_cache);


    SDL_ShowWindow(window);

    bool keys[SDL_NUM_SCANCODES] = {};

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

                        default:
                            keys[event.key.keysym.scancode] = true;
                    }
                    break;

                case SDL_KEYUP:
                    keys[event.key.keysym.scancode] = false;
                    break;

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
                        camera.setAspect((float)w / (float)h);
                    } else if (event.window.event == SDL_WINDOWEVENT_DISPLAY_CHANGED) {
                        SDL_GL_SetSwapInterval(-1);
                    }
                    break;

                case SDL_MOUSEMOTION:
                    camera.addYaw((float)event.motion.xrel * mouse_sensitivity);
                    camera.addPitch((float)-event.motion.yrel * mouse_sensitivity);
                    break;
                    
                case SDL_MOUSEWHEEL:
                    camera.addFov((float)-event.wheel.y);
                    break;
            }
        }

        glm::vec3 move_vector = glm::vec3(0.0f);
        if (keys[SDL_SCANCODE_W]) move_vector.z += 1.0f;
        if (keys[SDL_SCANCODE_S]) move_vector.z -= 1.0f;
        if (keys[SDL_SCANCODE_D]) move_vector.x += 1.0f;
        if (keys[SDL_SCANCODE_A]) move_vector.x -= 1.0f;
        if (keys[SDL_SCANCODE_LSHIFT]) move_vector.y += 1.0f;
        if (keys[SDL_SCANCODE_LCTRL]) move_vector.y -= 1.0f;
        if (glm::length(move_vector) > 0.0f) {
            move_vector = glm::normalize(move_vector) * speed * frame_time;
            camera.move(move_vector);
        }


        jet.update(frame_time, &world);


        glDepthMask(GL_TRUE);
        glClearColor(0.3f, 0.5f, 0.7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader);

        camera.use(shader);

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


        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
            std::cout << "GL error: " << err << '\n';

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}