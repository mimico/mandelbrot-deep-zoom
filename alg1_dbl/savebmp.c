#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RGB;

static RGB table[256] = {{0,0,0}}; /* Initialize to all black */
static RGB escapeColour = {0,0,0}; /* escapecolur is black */

void colourGradient(RGB colour1, RGB colour2, int base){
    float rstep = (colour2.r - colour1.r)/64.0f;
    float gstep = (colour2.g - colour1.g)/64.0f;
    float bstep = (colour2.b - colour1.b)/64.0f;
    int i;
    for(i = 0; i < 64; i++) {//blue to green
        table[i + base].r = colour1.r + rstep * i;
        table[i + base].g = colour1.g + gstep * i;
        table[i + base].b = colour1.b + bstep * i;
    }
}

RGB linearColourInterpolation(RGB colour1, RGB colour2, float percentage){
    RGB colour = {0,0,0};
    float rstep = (colour2.r - colour1.r);
    float gstep = (colour2.g - colour1.g);
    float bstep = (colour2.b - colour1.b);

    colour.r = colour1.r + rstep * percentage;
    colour.g = colour1.g + gstep * percentage;
    colour.b = colour1.b + bstep * percentage;

    return colour;
}

void initColours(){
    //define color presets
    RGB cream = {234, 230, 202};
    RGB blue = {50,18,160};
    RGB violet = {138,43,226};
    RGB orange = {255,193,37};
    RGB red = {208,32,144};

    RGB col0 = blue;
    RGB col1 = cream;
    RGB col2 = violet;
    RGB col3 = orange;
    RGB col4 = red;
    colourGradient(col0, col1, 0); //blue to cream
    colourGradient(col1, col2, 64);//cream to violet
    colourGradient(col2, col3, 128);//violet to orange
    colourGradient(col3, col4, 192);//orange to red

}


void saveBMP(char* filename, float* result, int w, int h, int max_colors){
    initColours();
        FILE *f;
        unsigned char *img = NULL;
        int filesize = 54 + 3*w*h;  //w is your image width, h is image height, both int

        unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
        unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
        unsigned char bmppad[3] = {0,0,0};

        bmpfileheader[ 2] = (unsigned char)(filesize    );
        bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
        bmpfileheader[ 4] = (unsigned char)(filesize>>16);
        bmpfileheader[ 5] = (unsigned char)(filesize>>24);

        bmpinfoheader[ 4] = (unsigned char)(       w    );
        bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
        bmpinfoheader[ 6] = (unsigned char)(       w>>16);
        bmpinfoheader[ 7] = (unsigned char)(       w>>24);
        bmpinfoheader[ 8] = (unsigned char)(       h    );
        bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
        bmpinfoheader[10] = (unsigned char)(       h>>16);
        bmpinfoheader[11] = (unsigned char)(       h>>24);

        f = fopen(filename,"wb");
        fwrite(bmpfileheader,1,14,f);
        fwrite(bmpinfoheader,1,40,f);

        img = (unsigned char *)malloc(3*w);
        assert(img);

        int i,j;
        for(j=0; j<h; j++)
        {
            for(i=0; i<w; i++)
                {
                  float index = result[j*w+i];
          int r,g,b;
          if(index == 0){
            r = escapeColour.r;
            g = escapeColour.g;
            b = escapeColour.b;
          }
          else{
            RGB col = linearColourInterpolation(table[(int)floor(fmod(index,255.0f))],
                                                table[(int)floor(fmod((index + 1),255.0f))],
                                                fmod(index,1.0f));
            r = col.r;
            g = col.g;
            b = col.b;
          }
                    img[i*3+2] = (unsigned char)(r);
                    img[i*3+1] = (unsigned char)(g);
                    img[i*3+0] = (unsigned char)(b);
                }
                fwrite(img,3,w,f);
            fwrite(bmppad,1,(4-(w*3)%4)%4,f);
        }
        fclose(f);
}
