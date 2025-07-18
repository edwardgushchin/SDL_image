/*
  SDL_image:  An example image loading library for use with SDL
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/*
 * PCX file reader:
 * Supports:
 *  1..4 bits/pixel in multiplanar format (1 bit/plane/pixel)
 *  8 bits/pixel in single-planar format (8 bits/plane/pixel)
 *  24 bits/pixel in 3-plane format (8 bits/plane/pixel)
 *
 * (The <8bpp formats are expanded to 8bpp surfaces)
 *
 * Doesn't support:
 *  single-planar packed-pixel formats other than 8bpp
 *  4-plane 32bpp format with a fourth "intensity" plane
 */

#include <SDL3/SDL_endian.h>

#include <SDL3_image/SDL_image.h>

#ifdef LOAD_PCX

struct PCXheader {
    Uint8 Manufacturer;
    Uint8 Version;
    Uint8 Encoding;
    Uint8 BitsPerPixel;
    Sint16 Xmin, Ymin, Xmax, Ymax;
    Sint16 HDpi, VDpi;
    Uint8 Colormap[48];
    Uint8 Reserved;
    Uint8 NPlanes;
    Sint16 BytesPerLine;
    Sint16 PaletteInfo;
    Sint16 HscreenSize;
    Sint16 VscreenSize;
    Uint8 Filler[54];
};

/* See if an image is contained in a data source */
bool IMG_isPCX(SDL_IOStream *src)
{
    Sint64 start;
    bool is_PCX;
    const int ZSoft_Manufacturer = 10;
    const int PC_Paintbrush_Version = 5;
    const int PCX_Uncompressed_Encoding = 0;
    const int PCX_RunLength_Encoding = 1;
    struct PCXheader pcxh;

    if (!src) {
        return false;
    }

    start = SDL_TellIO(src);
    is_PCX = false;
    if (SDL_ReadIO(src, &pcxh, sizeof(pcxh)) == sizeof(pcxh) ) {
        if ( (pcxh.Manufacturer == ZSoft_Manufacturer) &&
             (pcxh.Version == PC_Paintbrush_Version) &&
             (pcxh.Encoding == PCX_RunLength_Encoding ||
              pcxh.Encoding == PCX_Uncompressed_Encoding) ) {
            is_PCX = true;
        }
    }
    SDL_SeekIO(src, start, SDL_IO_SEEK_SET);
    return is_PCX;
}

/* Load a PCX type image from an SDL datasource */
SDL_Surface *IMG_LoadPCX_IO(SDL_IOStream *src)
{
    Sint64 start;
    struct PCXheader pcxh;
    SDL_Surface *surface = NULL;
    int width, height;
    int y;
    size_t bpl;
    Uint8 *row, *buf = NULL;
    char *error = NULL;
    int bits, src_bits;
    int count = 0;
    Uint8 ch;
    Uint32 format;

    if ( !src ) {
        /* The error message has been set in SDL_IOFromFile */
        return NULL;
    }
    start = SDL_TellIO(src);

    if (SDL_ReadIO(src, &pcxh, sizeof(pcxh)) != sizeof(pcxh) ) {
        error = "file truncated";
        goto done;
    }
    pcxh.Xmin = SDL_Swap16LE(pcxh.Xmin);
    pcxh.Ymin = SDL_Swap16LE(pcxh.Ymin);
    pcxh.Xmax = SDL_Swap16LE(pcxh.Xmax);
    pcxh.Ymax = SDL_Swap16LE(pcxh.Ymax);
    pcxh.BytesPerLine = SDL_Swap16LE(pcxh.BytesPerLine);

#if 0
    printf("Manufacturer = %d\n", pcxh.Manufacturer);
    printf("Version = %d\n", pcxh.Version);
    printf("Encoding = %d\n", pcxh.Encoding);
    printf("BitsPerPixel = %d\n", pcxh.BitsPerPixel);
    printf("Xmin = %d, Ymin = %d, Xmax = %d, Ymax = %d\n", pcxh.Xmin, pcxh.Ymin, pcxh.Xmax, pcxh.Ymax);
    printf("HDpi = %d, VDpi = %d\n", pcxh.HDpi, pcxh.VDpi);
    printf("NPlanes = %d\n", pcxh.NPlanes);
    printf("BytesPerLine = %d\n", pcxh.BytesPerLine);
    printf("PaletteInfo = %d\n", pcxh.PaletteInfo);
    printf("HscreenSize = %d\n", pcxh.HscreenSize);
    printf("VscreenSize = %d\n", pcxh.VscreenSize);
#endif

    /* Create the surface of the appropriate type */
    width = (pcxh.Xmax - pcxh.Xmin) + 1;
    height = (pcxh.Ymax - pcxh.Ymin) + 1;
    src_bits = pcxh.BitsPerPixel * pcxh.NPlanes;
    if((pcxh.BitsPerPixel == 1 && pcxh.NPlanes >= 1 && pcxh.NPlanes <= 4)
       || (pcxh.BitsPerPixel == 8 && pcxh.NPlanes == 1)) {
        bits = 8;
        format = SDL_PIXELFORMAT_INDEX8;
    } else if(pcxh.BitsPerPixel == 8 && pcxh.NPlanes == 3) {
        bits = 24;
        format = SDL_PIXELFORMAT_RGB24;
    } else {
        error = "unsupported PCX format";
        goto done;
    }
    surface = SDL_CreateSurface(width, height, format);
    if ( surface == NULL ) {
        goto done;
    }

    bpl = pcxh.NPlanes * pcxh.BytesPerLine;
    buf = (Uint8 *)SDL_calloc(bpl, 1);
    if ( !buf ) {
        error = "Out of memory";
        goto done;
    }
    row = (Uint8 *)surface->pixels;
    for ( y=0; y<surface->h; ++y ) {
        /* decode a scan line to a temporary buffer first */
        size_t i;
        if ( pcxh.Encoding == 0 ) {
            if (SDL_ReadIO(src, buf, bpl) != bpl ) {
                error = "file truncated";
                goto done;
            }
        } else {
            for ( i = 0; i < bpl; i++ ) {

                if ( !count ) {
                    if (SDL_ReadIO(src, &ch, 1) != 1 ) {
                        error = "file truncated";
                        goto done;
                    }
                    if ( ch < 0xc0 ) {
                        count = 1;
                    } else {
                        count = ch - 0xc0;
                        if (SDL_ReadIO(src, &ch, 1) != 1 ) {
                            error = "file truncated";
                            goto done;
                        }
                    }
                }
                buf[i] = ch;
                count--;
            }
        }

        if ( src_bits <= 4 ) {
            /* expand planes to 1 byte/pixel */
            Uint8 *innerSrc = buf;
            int plane;
            for ( plane = 0; plane < pcxh.NPlanes; plane++ ) {
                int j, k, x = 0;
                for( j = 0; j < pcxh.BytesPerLine; j++ ) {
                    Uint8 byte = *innerSrc++;
                    for( k = 7; k >= 0; k-- ) {
                        unsigned bit = (byte >> k) & 1;
                        /* skip padding bits */
                        if (j * 8 + k >= width)
                            continue;
                        row[x++] |= bit << plane;
                    }
                }
            }
        } else if ( src_bits == 8 ) {
            /* Copy the row directly */
            SDL_memcpy(row, buf, SDL_min((size_t)width, bpl));
        } else if ( src_bits == 24 ) {
            /* de-interlace planes */
            Uint8 *innerSrc = buf;
            Uint8 *end1 = buf+bpl;
            int plane;
            for ( plane = 0; plane < pcxh.NPlanes; plane++ ) {
                int x;
                Uint8 *dst = row + plane;
                Uint8 *end2= row + surface->pitch;
                for ( x = 0; x < width; x++ ) {
                    if ( (innerSrc + x) >= end1 || dst >= end2 ) {
                        error = "decoding out of bounds (corrupt?)";
                        goto done;
                    }
                    *dst = innerSrc[x];
                    dst += pcxh.NPlanes;
                }
                innerSrc += pcxh.BytesPerLine;
            }
        }

        row += surface->pitch;
    }

    if ( bits == 8 ) {
        int nc = 1 << src_bits;
        SDL_Palette *palette;
        int i;

        palette = SDL_CreateSurfacePalette(surface);
        if (!palette) {
            error = "Couldn't create palette";
            goto done;
        }
        if (nc > palette->ncolors) {
            nc = palette->ncolors;
        }
        palette->ncolors = nc;

        if ( src_bits == 8 ) {
            Uint8 pch;
            Uint8 colormap[768];

            /* look for a 256-colour palette */
            do {
                if (SDL_ReadIO(src, &pch, 1) != 1 ) {
                    /* Couldn't find the palette, try the end of the file */
                    SDL_SeekIO(src, -768, SDL_IO_SEEK_END);
                    break;
                }
            } while ( pch != 12 );

            if (SDL_ReadIO(src, colormap, sizeof(colormap)) != sizeof(colormap) ) {
                error = "file truncated";
                goto done;
            }
            for ( i = 0; i < 256; i++ ) {
                palette->colors[i].r = colormap[i * 3 + 0];
                palette->colors[i].g = colormap[i * 3 + 1];
                palette->colors[i].b = colormap[i * 3 + 2];
            }
        } else {
            for ( i = 0; i < nc; i++ ) {
                palette->colors[i].r = pcxh.Colormap[i * 3 + 0];
                palette->colors[i].g = pcxh.Colormap[i * 3 + 1];
                palette->colors[i].b = pcxh.Colormap[i * 3 + 2];
            }
        }
    }

done:
    SDL_free(buf);
    if ( error ) {
        SDL_SeekIO(src, start, SDL_IO_SEEK_SET);
        if ( surface ) {
            SDL_DestroySurface(surface);
            surface = NULL;
        }
        SDL_SetError("%s", error);
    }
    return surface;
}

#else

/* See if an image is contained in a data source */
bool IMG_isPCX(SDL_IOStream *src)
{
    (void)src;
    return false;
}

/* Load a PCX type image from an SDL datasource */
SDL_Surface *IMG_LoadPCX_IO(SDL_IOStream *src)
{
    (void)src;
    return NULL;
}

#endif /* LOAD_PCX */
