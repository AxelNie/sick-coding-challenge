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

unsigned char getMostCommonNeighbour(const unsigned char *input_image, int i, int ksize);

unsigned char getPixelAtPos(int x, int y, int offset, const unsigned char *input_image);

class Pixel
{
public:
    Pixel(int value_in, int index_in)
    {
        int pixels_per_ch = HEIGHT * WIDTH;
        ch = index_in / pixels_per_ch;
        int index_in_ch = index_in - ch * pixels_per_ch;

        y = index_in_ch / WIDTH;
        x = index_in_ch % WIDTH;
        value = value_in;
    }
    int x;
    int y;
    int value;
    int ch;
};

int main(int argc, char **argv)
{
    unsigned char input_image[HEIGHT * WIDTH * 3];
    unsigned char output_image[HEIGHT * WIDTH * 3];

    std::fstream fin("baboon_buf.dat", std::ios::in | std::ios::binary);
    fin.read((char *)input_image, HEIGHT * WIDTH * 3);

    auto start = std::chrono::high_resolution_clock::now();
    perform_operation(input_image, output_image, 23);
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

    for (int i = 0; i < (HEIGHT * WIDTH * 3); i++)
    {
        unsigned char most_common_pixel = getMostCommonNeighbour(input_image, i, ksize);
        output_image[i] = most_common_pixel;
    }
}

unsigned char getMostCommonNeighbour(const unsigned char *input_image, int i, int ksize)
{
    Pixel currentPixel = Pixel(input_image[i], i);
    unsigned char freq_table[256] = {0};

    unsigned char most_common_count = 0;
    unsigned char most_common_value = 0;
    for (int i = -ksize; i <= ksize; i++)
    {
        for (int y = -ksize; y <= ksize; y++)
        {
            unsigned char current_value = getPixelAtPos(currentPixel.x + i, currentPixel.y + y, currentPixel.ch, input_image);
            freq_table[current_value] += 1;
            unsigned char current_count = freq_table[current_value];

            // If current pixel is more common, update most common
            if (current_count > most_common_count)
            {
                most_common_count = current_count;
                most_common_value = current_value;
            }
            else if (current_count == most_common_count)
            {
                if (current_value < most_common_value)
                {
                    most_common_count = current_count;
                    most_common_value = current_value;
                }
            }
        }
    }
    return most_common_value;
}

unsigned char getPixelAtPos(int x, int y, int ch, const unsigned char *input_image)
{
    int index_in_color_channel = y * WIDTH + x;
    // int index_global = index_in_color_channel * 3 + offset;
    int index_global = HEIGHT * WIDTH * ch + y * WIDTH + x;

    if (y < 0 || x < 0 || x > WIDTH - 1 || y > HEIGHT - 1)
    {
        return 0; // If pixel is outside of image
    }
    else
    {
        return input_image[index_global];
    }
}
