#ifndef TEXT_H
#define TEXT_H
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>

class Text {
    public:
    Text(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, const std::string& messageText, const SDL_Color& color);

    void display(SDL_Renderer* renderer, int x, int y) const;
    SDL_Texture* loadFont(SDL_Renderer* renderer, const std::string &fontPath, int fontSize, const std::string& messageText, const SDL_Color& color);

    TTF_Font* font = nullptr;
    
    private:
    SDL_Texture* textTexture = nullptr;
    mutable SDL_Rect textRect;
};

#endif