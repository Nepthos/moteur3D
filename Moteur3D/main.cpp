#include "tgaimage.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

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
            if (current_line.rfind("v ",0) == 0) {
                std::istringstream stream_line(current_line.c_str());
                double coords[3];
                char trash;
                for(int i = 0 ; i < 4 ; i ++) {
                  if(i == 0)  {
                      stream_line >> trash; // removing first element
                  } else {
                      stream_line >> coords[i - 1];
                      std::cout << i << ":" << coords[i - 1] << " ";
                  }
                }
                std::cout << std::endl;
            }
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
