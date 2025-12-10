// ============================================================================
// 256 Colour Palette Experiments.
//
// Finding different ways to express / explore the 256 colour palette in VIDC 1.
// Convert 16 VIDC registers into 256 RGB4 colour values for easier manipulation.
// Helper for RGB4 to Archie colour index based on nearest match of VIDC palette.
// Floating point HSV to RGB4 conversion helper.
// 12-bit HSV lookup table (V:4 S:2 H:6) based on nearest match.
// Full RGB4 lookup table  (R:4 G:4 B:4) based on nearest match.
//
// TODO: Try same experiments with RGB332 arrangements (and RGB233, RGB323 etc.)
// TODO: Create a dithering table, so best 2x colour indices to use for closest RGB4/HSV colour match.
//
// ============================================================================

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "colour.h"
#include "../lib/plot.h"

// Standard RISCOS VIDC1 palette RGB4 values in 256 colour MODE.
// Gives 64 colours (R:2 G:2 B:2) with 4 'tints' (brightness / greys).
// However the palette is arranged in a very unintuitive order.

// NOTE: These values are 0x0RGB arrange, whilst VIDC palette registers are 0x0BGR!!
// TODO: Decide whether it would be better to keep these as VIDC1 registers to avoid confusion...
//
static u16 defaultPalette[16] = {0x0000, 0x0111, 0x0222, 0x0333, 0x0400, 0x0511, 0x0622, 0x0733, 0x0004, 0x0115, 0x0226, 0x0337, 0x0404, 0x0515, 0x0626, 0x0737};
// TODO: RGB332 palette setup.

static u16 archie256[256];      // colour index [0,255] -> 0x0RGB
static u8 usedCount[256];       // for debugging.

static u8 HsvPalette[4096];     // 0x0VsH (V:4 S:2 H:6) value -> colour index [0, 255]
static u8 Rgb4Palette[4096];    // 0x0RGB (R:4 G:4 B:4) value -> colour index [0, 255]

void MakeArchie256Palette(const u16* basePalette, u16* palette256)
{
	for (int i = 0; i < 256; i++)
	{
        u8 R,G,B;

        // RGB4 output by VIDC based on colour index and VIDC palette.

        u16 paletteColor = basePalette[i & 15];	// lowest 4 bits are logical colour

		// B=L7 D10 D9 D8   G=L6 L5 D5 D4   R=L4 D2 D1 D0
        R=(i & 0x10) >> 1 | ((paletteColor>>8) & 0x7);
        G=(i & 0x60) >> 3 | ((paletteColor>>4) & 0x3);
        B=(i & 0x80) >> 4 | ((paletteColor>>0) & 0x7);

        palette256[i]=(R<<8) | (G<<4) | (B<<0);
	}
}

void MakeHsvPalette(u8* hsvPalette)
{
    // 4 bits of Value
	for (int v = 0; v <= 15; v++)
    {
        // 2 bits of Saturation
        for (int s = 0; s <= 3; s++)
        {
            // 6 bits of Hue
            for (int h = 0; h <= 63; h++)
            {
                *hsvPalette++ = rgbToArchie(hsvToRgb(h/64.0f, s/3.0f, v/15.0f));
            }
        }
    }
}

void MakeRgb4Palette(u8* rgb4palette)
{
    // 4 bits of Red
	for (int r = 0; r <= 15; r++)
    {
        // 4 bits of Green
        for (int g = 0; g <= 15; g++)
        {
            // 4 bits of Blue
            for (int b = 0; b <= 15; b++)
            {
                u16 rgb4 = (r<<8)|(g<<4)|(b<<0);
                *rgb4palette++ = rgbToArchie(rgb4);
            }
        }
    }
}

void MakeDefaultPalette()
{
    MakeArchie256Palette(defaultPalette, archie256);
    MakeHsvPalette(HsvPalette);
    MakeRgb4Palette(Rgb4Palette);
}

void plotColours()
{
    int x=0, y=8;

    // Plot 256 colour index values.
    #if 0
    for(int c=0; c<256; c++)
    {
        plotBlock(x,y,c);
        x+=4;
        if(x>=64){x=0; y+=4;}
    }
    #endif

    // Exploring HSV conversion.
    #if 0
    for(int s=0; s<16; s++) {
        x=0;
        for(int h=0; h<64; h++)
        {
            u8 c = rgbToArchie(hsvToRgb(h/64.0f, s/16.0f, 1.0f));
            plotBlock(x,y,c);
            usedCount[c]+=1;
            x+=4;
        }
        y+=4;
    }

    for(int v=15; v>=0; v--) {
        x=0;
        for(int h=0; h<64; h++)
        {
            u8 c = rgbToArchie(hsvToRgb(h/64.0f, 1.0f, v/16.0f));
            plotBlock(x,y,c);
            usedCount[c]+=1;
            x+=4;
        }
        y+=4;
    }

    for(int v=15; v>=0; v--) {
        x=0;
        for(int h=0; h<64; h++)
        {
            u8 c = rgbToArchie(hsvToRgb(h/64.0f, 0.0f, v/16.0f));
            plotBlock(x,y,c);
            usedCount[c]+=1;
            x+=4;
        }
        y+=4;
    }
    #endif

    // Plot our HSV or RGB4 palette to visualise.
    #if 1
    y=0;
    for(int i=0; i < 4096; i++)
    {
        u8 c=HsvPalette[i];
        //u8 c=Rgb4Palette[i];
        plotBlock(x,y,c);
        x=(x+4) & 0xff;
        if (x==0) y+=4;
        usedCount[c]+=1;
    }
    #endif

    // Count how many colour indices were used.
    int used=0;
    for(int i=0;i<256;i++)
    {
        if (usedCount[i]!=0) used++;
    }
    printf("%d", used);
}

// Convert an RGB4 (0x0RGB) value to Archie colour index [0, 255] based on VIDC palette.
// Does an exhaustive search to find the nearest RGB value.
u8 rgbToArchie(u16 rgb)
{
    u8 R, G, B;
    int minDist2=0;
    int minIndex=-1;
    
    R = (rgb>>8)&0xf;
    G = (rgb>>4)&0xf;
    B = (rgb>>0)&0xf;

    //u8 baseIndex = ((R&0x8)<<1) | ((G&0xc)<<3) | ((B&0x8)<<4); // [0x0X, 0xFX]

    for(int i=0; i<256; i++)
    {
        // Test all possible palette entries to see which is closest.
        u8 pR=(archie256[i] >> 8) & 0xf;
        u8 pG=(archie256[i] >> 4) & 0xf;
        u8 pB=(archie256[i] >> 0) & 0xf;

        #if 1 // Weighted Euclidian distance for RGB based on https://www.compuphase.com/cmetric.htm (via Wikipedia)
        u8 rHat = (pR + R)/2;   // average R
        int dist2 = 4*(G-pG)*(G-pG);

        if (rHat < 8)   // Where average R is < 0.5 use a weighting with more blue.
            dist2 += 2*(R-pR)*(R-pR) + 3*(B-pB)*(B-pB);
        else
            dist2 += 3*(R-pR)*(R-pR) + 2*(B-pB)*(B-pB);
        #else
        int dist2 = (R-pR)*(R-pR) + (G-pG)*(G-pG) + (B-pB)*(B-pB);  // Pure Euclidian distance.
        #endif

        if(minIndex==-1 || dist2<minDist2) {
            minIndex=i;
            minDist2=dist2;
        }
    }

    assert(minIndex!=-1);
    return minIndex;
}

// Floating point HSV values to RGB4. Yoinked from https://stackoverflow.com/questions/51203917/math-behind-hsv-to-rgb-conversion-of-colors
u16 hsvToRgb(float h, float s, float v)
{
    float r, g, b;
    u8 R, G, B;

    int i = (int)floorf(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch(i % 6){
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        default:
        case 5: r = v, g = p, b = q; break;
    }

    // Most code has this *255 for RGB8.
    R=(u8)(r*15);
    G=(u8)(g*15);
    B=(u8)(b*15);

    return (R<<8) | (G<<4) | (B<<0);
}
