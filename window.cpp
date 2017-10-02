//
// Created by ad on 18/09/17.
//

#include <SDL2/SDL.h>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include "window.h"

Window::Window() : _width(0), _height(0) {

}

Window::Window(const char* title, const unsigned int width, const unsigned int height, const int x, const int y, const Uint32 flags) : _window(NULL), _width(width), _height(height) {
    // init sdl
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        printf("[error] SDL_Init() failed:\n%s\n", SDL_GetError());
        close();
    }
    // create the window
    _window = SDL_CreateWindow(title, x, y, width, height, flags);
    if(!_window) {
        printf("[error] SDL_CreateWindow() failed:\n%s\n", SDL_GetError());
        close();
    }

    // create renderer
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);

    // get the keyboard's state
    int keys;
    const unsigned char *state= SDL_GetKeyboardState(&keys);
    _key_states.assign(state, state + keys);

    SDL_SetWindowDisplayMode(_window, NULL);

    // save window's dimensions
    SDL_GetWindowSize(_window, &_width, &_height);
}


int Window::events() {
    // manage events
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_WINDOWEVENT:
                // resize the window
                if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    // update the dimensions
                    _width= event.window.data1;
                    _height= event.window.data2;
                    SDL_SetWindowSize(_window, _width, _height);
                }
                break;

            case SDL_KEYDOWN:
                // modify the keyboard's state
                if((size_t) event.key.keysym.scancode < _key_states.size())
                {
                    _key_states[event.key.keysym.scancode]= 1;
                    _last_key = event.key;    // save the last key event
                }
                break;

            case SDL_KEYUP:
                // modify the keyboard's state
                if((size_t) event.key.keysym.scancode < _key_states.size())
                {
                    _key_states[event.key.keysym.scancode]= 0;
                    _last_key = event.key;    // save the last key event
                }
                break;

            case SDL_MOUSEBUTTONDOWN:

            case SDL_MOUSEBUTTONUP:
                //_last_button = event.button;
                //break;

            case SDL_MOUSEWHEEL:
                //_last_wheel = event.wheel;
                //break;

            case SDL_QUIT:
                return 0;            // close the application

        }
    }
    return 1;
}


void Window::fill_texture_from_CVmat(const cv::Mat & mat) {
    IplImage opencvimg2 = (IplImage)mat;
    IplImage* opencvimg = &opencvimg2;

    //Convert to SDL_Surface
    SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(
            (void*)opencvimg->imageData,
            opencvimg->width, opencvimg->height,
            opencvimg->depth*opencvimg->nChannels,
            opencvimg->widthStep,
            0xff0000, 0x00ff00, 0x0000ff, 0);

    if(surface == NULL)
    {
        SDL_Log("Couldn't convert Mat to Surface.");
    }

    //Convert to SDL_Texture
    _texture = SDL_CreateTextureFromSurface(
            _renderer, surface);
    if(_texture == NULL)
    {
        SDL_Log("Couldn't convert Mat(converted to surface) to Texture.");
    }
}


void Window::render() {
    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _texture, NULL, NULL);
    SDL_RenderPresent(_renderer);
}


int Window::window_width() {
    return _width;
}


int Window::window_height() {
    return _height;
}


int Window::key_state(const SDL_Keycode key)
{
    SDL_Scancode code = SDL_GetScancodeFromKey(key);
    assert((size_t) code < _key_states.size());
    return (int)  _key_states[code];
}

void Window::clear_key_state(const SDL_Keycode key)
{
    SDL_Scancode code = SDL_GetScancodeFromKey(key);
    assert((size_t) code < _key_states.size());
    _key_states[code]= 0;
}


void Window::close() {
    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
    SDL_Quit();
}