/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

extern unsigned char icon_bmp[];
extern unsigned int icon_bmp_len;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", 800, 600, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Load the icon */
    texture = IMG_LoadTexture_IO(renderer, SDL_IOFromConstMem(icon_bmp, icon_bmp_len), true);
    if (!texture) {
        SDL_Log("Couldn't load icon: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    int w = 0, h = 0;
    SDL_FRect dst;
    const float scale = 4.0f;

    /* Center the icon and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_GetTextureSize(texture, &dst.w, &dst.h);
    dst.x = ((w / scale) - dst.w) / 2;
    dst.y = ((h / scale) - dst.h) / 2;

    /* Draw the icon */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, &dst);
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}

/* Data used by this example */
unsigned char icon_bmp[] = {
  0x42, 0x4d, 0x42, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00,
  0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x20, 0x00,
  0x00, 0x00, 0x01, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
  0x00, 0x00, 0x6d, 0x0b, 0x00, 0x00, 0x6d, 0x0b, 0x00, 0x00, 0x03, 0x00,
  0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x22, 0x22, 0x22,
  0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22,
  0x21, 0x11, 0x11, 0x12, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x12, 0x21, 0x11, 0x11, 0x11, 0x11, 0x12, 0x21, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x11, 0x11, 0x11, 0x11, 0x11,
  0x11, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x21, 0x11,
  0x22, 0x22, 0x22, 0x22, 0x11, 0x12, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x12, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x21, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x22, 0x22, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x22,
  0x22, 0x21, 0x12, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x22, 0x22, 0x22, 0x22, 0x21, 0x12, 0x22, 0x22, 0x22, 0x22, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x22, 0x22, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x22,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00,
  0x01, 0x22, 0x22, 0x11, 0x11, 0x22, 0x22, 0x11, 0x11, 0x22, 0x22, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x11, 0x01, 0x22, 0x22, 0x11,
  0x01, 0x22, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x22, 0x11,
  0x11, 0x22, 0x22, 0x11, 0x11, 0x22, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x12, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x21, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
  0x22, 0x22, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x22,
  0x22, 0x22, 0x22, 0x22, 0x22, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x01, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x10, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x22, 0x22, 0x22, 0x22,
  0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x11, 0x11, 0x11, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00
};
unsigned int icon_bmp_len = 578;
