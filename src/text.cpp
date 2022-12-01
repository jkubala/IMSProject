#include "text.hpp"
#include <iostream>

    Text::Text(SDL_Renderer* renderer, const std::string &fontPath, int fontSize, const std::string& messageText, const SDL_Color& color)
    {
        textTexture = loadFont(renderer, fontPath, fontSize, messageText, color);
        SDL_QueryTexture(textTexture, nullptr, nullptr, &textRect.w, &textRect.h);
    }

    void Text::display(SDL_Renderer* renderer, int x, int y) const
    {
        textRect.x = x;
        textRect.y = y;
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    }

    SDL_Texture* Text::loadFont(SDL_Renderer* renderer, const std::string &fontPath, int fontSize, const std::string& messageText, const SDL_Color& color)
    {
        this->font = TTF_OpenFont(fontPath.c_str(), fontSize);
        if(!font)
        {
            std::cerr << "Failed to load font!\n" << TTF_GetError();
        }
        SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, messageText.c_str(), color, 100);
        if(!textSurface)
        {
            std::cerr << "Failed to create text surface!\n";
        }

        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(!textTexture)
        {
            std::cerr << "Failed to create text texture!\n";
        }

        SDL_FreeSurface(textSurface);
        return textTexture;
    }