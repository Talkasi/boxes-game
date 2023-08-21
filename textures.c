#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

int loadTextureFromFile(struct LTexture *Texture, char *path, SDL_Renderer *Render)
{
    freeTexture(Texture);
    SDL_Texture *newTexture = NULL;

    SDL_Surface *loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("SDL_image load Error: %s\n", IMG_GetError());
        return 0;
    }

    if (SDL_ConvertSurface(loadedSurface, loadedSurface->format, 0) == NULL) {
        printf("Unable to optimize image %s! SDL Error: %s\n", path, SDL_GetError());
        return 0;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0));

    newTexture = SDL_CreateTextureFromSurface(Render, loadedSurface);
    if (newTexture == NULL) {
        printf("SDL texture creating Error: %s\n", SDL_GetError());
        return 0;
    }

    SDL_RenderCopy(Render, newTexture, 0, 0);
    SDL_FreeSurface(loadedSurface);
    Texture->Texture = newTexture;
    return 1;
}

void renderTexture(struct LTexture *Texture, int x, int y, SDL_Rect *srcRect, SDL_Renderer *Render)
{
    SDL_Rect renderQuad = {x, y, Texture->Width, Texture->Height};
    SDL_RenderCopy(Render, Texture->Texture, srcRect, &renderQuad);
}

void renderText(char *text, SDL_Rect textRect, TTF_Font *Font, Uint32 wrapLength, SDL_Renderer *Render)
{
    SDL_Color Color = {0, 0, 0, 0};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid_Wrapped(Font, text, Color, wrapLength);
    SDL_Texture *Message = SDL_CreateTextureFromSurface(Render, surfaceMessage);
    SDL_RenderCopy(Render, Message, NULL, &textRect);
    SDL_DestroyTexture(Message);
    SDL_FreeSurface(surfaceMessage);
}

void freeTexture(struct LTexture *Texture)
{
    if (Texture->Texture != NULL) {
        SDL_DestroyTexture(Texture->Texture);
        Texture->Texture = NULL;
        Texture->Width = 0;
        Texture->Height = 0;
    }
}