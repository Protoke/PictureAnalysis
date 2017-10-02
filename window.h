//
// Created by ad on 18/09/17.
//

#ifndef PICTUREANALYSIS_WINDOW_H
#define PICTUREANALYSIS_WINDOW_H


#include <SDL2/SDL_video.h>
#include <vector>
#include <opencv2/core/mat.hpp>

class Window {

public:
    /************************************/
    /* Constructors                     */
    /************************************/

    Window();

    Window(const char* title,
           const unsigned int width,
           const unsigned int height,
           const int x = SDL_WINDOWPOS_CENTERED,
           const int y = SDL_WINDOWPOS_CENTERED,
           const Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);


    /***********************************/
    /* Get                             */
    /***********************************/
    int window_width();
    int window_height();
    int key_state(const SDL_Keycode key);

    /***********************************/
    /* Set                             */
    /***********************************/
    void clear_key_state(const SDL_Keycode key);

    /***********************************/
    /* Others                          */
    /***********************************/
    int events();
    void fill_texture_from_CVmat(const cv::Mat & mat);
    void render();


    /***********************************/
    /* Close the window and quit SDL   */
    /***********************************/
    void close();

protected:
    SDL_Window * _window;
    SDL_Renderer * _renderer;
    SDL_Texture * _texture;
    int _width;
    int _height;
    std::vector<unsigned char> _key_states;
    SDL_KeyboardEvent _last_key;

};


#endif //PICTUREANALYSIS_WINDOW_H
