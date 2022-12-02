#ifndef TEXT_H
#define TEXT_H
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <string>

class Text {
    public:
    Text(SDL_Renderer* aRenderer, std::string aFontPath, int aFontSize, const std::string& messageText);

    void display(SDL_Renderer* renderer, int x, int y) const;
    void UpdateText(const std::string& newString);

    SDL_Texture* loadFont(SDL_Renderer* renderer, const std::string &fontPath, int fontSize, const std::string& messageText);

    TTF_Font* font = nullptr;
    
    private:
    SDL_Renderer* renderer = nullptr;
    std::string fontPath;
    int fontSize;
    const SDL_Color color = {0, 0, 0, 255};

    SDL_Texture* textTexture = nullptr;
    mutable SDL_Rect textRect;
};

#endif