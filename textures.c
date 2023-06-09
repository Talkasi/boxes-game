#include "textures.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int loadTextureFromFile(struct LTexture *Texture, char *path, SDL_Renderer *Render, int w, int h)
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

    struct SDL_Rect dstRect;
    dstRect.x = 0;
    dstRect.y = 0;
    dstRect.w = w;
    dstRect.h = h;
    SDL_RenderCopy(Render, newTexture, 0, &dstRect);

    Texture->Width = dstRect.w;
    Texture->Height = dstRect.h;

    SDL_FreeSurface(loadedSurface);

    Texture->Texture = newTexture;
    return 1;
}

void renderTexture(struct LTexture *Texture, int x, int y, SDL_Renderer *Render)
{
    SDL_Rect renderQuad = {x, y, Texture->Width, Texture->Height};
    SDL_RenderCopy(Render, Texture->Texture, NULL, &renderQuad);
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
