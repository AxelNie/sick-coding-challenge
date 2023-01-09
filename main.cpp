#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

// Name: Axel Nielsen
// Email: axel.nie17@gmail.com
// Phone number (optional): 0722232417

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

    std::size_t index = 0;
    std::for_each(input_image, input_image + (HEIGHT * WIDTH * 3), [&](unsigned char input_pixel)
                  {
            output_image[index] = getMostCommonNeighbour(input_image, index, ksize);

            ++index; });
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
            if (current_count > most_common_count || (current_count == most_common_count && current_value < most_common_value))
            {
                most_common_count = current_count;
                most_common_value = current_value;
            }
        }
    }

    return most_common_value;
}

unsigned char getPixelAtPos(int x, int y, int ch, const unsigned char *input_image)
{
    if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT)
    {
        return 0;
    }

    int index = (ch * HEIGHT * WIDTH) + (y * WIDTH) + x;
    return input_image[index];
}
