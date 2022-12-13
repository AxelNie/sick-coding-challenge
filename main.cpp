#include <iostream>
#include <chrono>
#include <fstream>
#include <cstring>
#include <cmath>

// Name:
// Email:
// Phone number (optional):

const int HEIGHT = 512;
const int WIDTH = 512;
const int K_MAX = 30;

void perform_operation(unsigned char *input_image, unsigned char *output_image, int ksize);

unsigned char getMostCommonNeighbour(const unsigned char* input_image, int i, int ksize);

int getPixelAtPos(int x, int y, int color_channel, const unsigned char* input_image);

class Pixel {
public:
    Pixel(int value_in,  int index) {
        int pixels_per_channel = HEIGHT * WIDTH;
        color_channel = std::floor(index / pixels_per_channel);

        int index_in_color_channel = index - (color_channel * pixels_per_channel);

        y = std::floor(index_in_color_channel / HEIGHT);
        x = index_in_color_channel % WIDTH;
        value = value_in;
        color_channel++;
    }
    int x;       
    int y;
    int value;
    int color_channel;
};

int main(int argc, char **argv)
{
    unsigned char input_image[HEIGHT * WIDTH * 3];
    unsigned char output_image[HEIGHT * WIDTH * 3];

    std::fstream fin("baboon_buf.dat", std::ios::in | std::ios::binary);
    fin.read((char *)input_image, HEIGHT * WIDTH * 3);

    auto start = std::chrono::high_resolution_clock::now();
    perform_operation(input_image, output_image, 0);
    auto end = std::chrono::high_resolution_clock::now();

    std::fstream fout("baboon_processed.dat", std::ios::out | std::ios::binary);
    fout.write((char *)output_image, HEIGHT * WIDTH * 3);

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << duration.count() << std::endl;

    return 0;
}


void perform_operation(unsigned char *input_image, unsigned char *output_image, int ksize)
{
    memcpy(output_image, input_image, HEIGHT * WIDTH * 3);

    
    for (int i = 0; i < (HEIGHT * WIDTH * 3); i++) {
        unsigned char most_common_pixel = getMostCommonNeighbour(input_image, i, ksize);
        output_image[i] = most_common_pixel;
    }
}

unsigned char getMostCommonNeighbour(const unsigned char* input_image, int i, int ksize)
{
    Pixel currentPixel = Pixel(input_image[i], i);
    unsigned char freq_table[256] = { 0 };

    int neighbours_count = 0;
    int most_common_count = 0;
    unsigned char most_common_value = 0;
    for (int i = -ksize; i <= ksize; i++) {
        for (int y = -ksize; y <= ksize; y++) {
            int current_value = getPixelAtPos(currentPixel.x + i, currentPixel.y + y, currentPixel.color_channel, input_image);
            int current_count = ++freq_table[current_value];

            //If current pixel is more common, update most common
            if (current_count > most_common_count) {
                most_common_count = current_count;
                most_common_value = current_value;
            }
            else if (current_count == most_common_count) { 
                if (current_value < most_common_value) {
                    most_common_count = current_count;
                    most_common_value = current_value;
                }
            }
            neighbours_count++;
        }
    }
    return most_common_value;
}

int getPixelAtPos(int x, int y, int color_channel, const unsigned char* input_image) {
    int pos = y * WIDTH + x;

    if (y < 0 || x < 0 || pos >(HEIGHT - 1) * (WIDTH - 1)) {
        return 0; //If pixel is outside of image
    }
    else {
        return input_image[(y * WIDTH + x)* color_channel];
    }
    
}



