#ifndef BOXES_GAME_TEXTURES_H
#define BOXES_GAME_TEXTURES_H

typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Rect SDL_Rect;

struct LTexture {
    SDL_Texture *Texture;
    int Width;
    int Height;
};

void freeTexture(struct LTexture *Texture);
void renderTexture(struct LTexture *Texture, int x, int y, SDL_Rect srcRect, SDL_Renderer *Render);
int loadTextureFromFile(struct LTexture *Texture, char *path, SDL_Renderer *Render, int w, int h);

#endif
