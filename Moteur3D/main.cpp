#include "tgaimage.h"
#include <iostream>
#include <fstream>
#include <string>

constexpr int width = 100;
constexpr int height = 100;

const TGAColor white =  { 255 , 255 , 255 , 255};
const TGAColor red = { 255 , 0 , 0 , 255};

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx = x1-x0;
    int dy = y1-y0;
    float derror = std::abs(dy/float(dx));
    float error = 0;
    int y = y0;
    for (int x=x0; x<=x1; x++) {
        if (steep) {
            image.set(y, x, color);
        } else {
            image.set(x, y, color);
        }
        error += derror;
        if (error>.5) {
            y += (y1>y0?1:-1);
            error -= 1.;
        }
    }
}


void read_obj() {
    std::string const fileName("../obj/african_head/african_head.obj");
    std::ifstream buffer(fileName.c_str());

    if(buffer) {
        std::string current_line;
        while(std::getline(buffer,current_line)) {
            std::cout << current_line << std::endl;
        }
    } else {
        std::cout << "Cannot open file at " << fileName << std::endl;
    }

}
int main() {
    TGAImage image(width, height, TGAImage::RGB);
    line(30,20,60,90,image, white);
    image.write_tga_file("framebuffer.tga");
    read_obj();
    return 0;
}
