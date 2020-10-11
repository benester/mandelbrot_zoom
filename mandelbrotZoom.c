#include <SDL2/SDL.h>   //sudo apt-get install -y libsdl2-2.0-0
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>    //sudo apt-get install -y libsdl2-image-dev

int WIDTH = 1200;
int HEIGHT = 1200;

long double MIN = -2.84;
long double MAX = 1.0;
//Scaling factor for each zoom level
long double factor = 1.0;

int MAX_ITTERATION = 200;

//Mapps an input value, which can be between in_min, in_max to a value between the range out_min, out_max
long double map(long double value, long double in_min, long double in_max, long double out_min, long double out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void colorCalculator(int* red, int* green, int* blue, int depth, int maxdepth){
    //Map depth to a value between 0 and 255
    int mapped = map(depth, 0, MAX_ITTERATION, 0, 255);
    //Cleaner output.
    if(depth == MAX_ITTERATION || mapped < 5){
        mapped = 0;
    }
    //Calculte each color value, for the rgb values respectivly
    *blue = map(mapped * mapped, 0, 255*255, 0, 255);
    *green = mapped;
    *red = map(sqrt(mapped), 0, sqrt(255), 0, 255);
}


int main(void){

    int red,green,blue;
    //SDL_Init will return 0 on sucess, exit program if not successfull
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
        //Print usefull error to the user, as to why program did not initilize.
        printf("An error occured while initilizing SDL: %s\n", SDL_GetError());
        return 1;
    }

    //Create the window to draw in
    SDL_Window* window = SDL_CreateWindow("Mandelbrot By Benjamin", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,0);
    printf("Init success");

    //SDL_CreateWindow reutns 0 on sucess, so exit program gracefully if not 0
    if(!window){
        printf("An error occured while initializing the window %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create the flags for renderer to use graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    //Create the renderer, with the above mentioned flags. 
    //-1 tells the createrenderer function not to care about what graphics drivers to use.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    //If renderer returns null, exit program
    if(!renderer){
        printf("Error createing renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT);

    //Create an event to be able to close the window
    SDL_Event event;

    //Keep track of renderd frames
    int count = 0;

    //Continue rendering and zooming forever
    while(1){

        SDL_RenderPresent(renderer);

        //Create the mandelbrot image
        for(int x = 0; x < WIDTH; x++){   //X = a
            for(int y = 0; y < HEIGHT; y++){ //Y = b

                //enable exiting of program
                if(SDL_PollEvent(&event) && event.type== SDL_QUIT){
                     //Clear the window
                    SDL_RenderClear(renderer);
                    //Cleanup resources before exiting
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return 0;
                }
                    
                //
                long double a = map(x, 0, WIDTH, MIN,MAX);
                long double b = map(y, 0, HEIGHT,MIN, MAX);


                long double a_initial = a;
                long double b_initial = b; 

                int depth = 0;

                for(int i = 0; i < MAX_ITTERATION; i++){
                    long double a_real = a * a - b*b;
                    long double b_real = 2*a*b;

                    a = a_real + a_initial;
                    b = b_real + b_initial;

                    if((a+b) > 2) {
                        break;
                    }
                    depth++;

                }
                //Map the depth value to a color. We send the adresses to the rgb ints, so that we can calculate all three at the same time
                 colorCalculator(&red, &green, &blue, depth, MAX_ITTERATION);

                    //Set the color to render 
                    SDL_SetRenderDrawColor(renderer, red,green,blue,255);
                    //Draw the pixel
                    SDL_RenderDrawPoint(renderer, x, y);
                
            }
        }

        MAX -= 0.1 * factor;
        MIN += 0.15 * factor;
        factor *= 0.9349;
        MAX_ITTERATION +=10;

        if(count > 30){
            MAX_ITTERATION *= 1.2;
        }
        //How many times have we zoomed?
        count++;

        //Save each zoom as a bmp file

        //Create a surface of the window
        SDL_Surface *surface = SDL_GetWindowSurface(window);
        SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ABGR8888, surface -> pixels, surface -> pitch);
        char buf[12];
        snprintf(buf, 12, "%d.bnp", count);

        SDL_SaveBMP(surface, buf);

        SDL_FreeSurface(surface);


    }


    //Clear the window
    SDL_RenderClear(renderer);

    //Cleanup resources before exiting
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}