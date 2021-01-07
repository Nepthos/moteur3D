#include "tgaimage.h"
#include "Point.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

constexpr int width = 300;
constexpr int height = 300;

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


void read_obj(TGAImage &image) {
    std::vector<Point> coordinates;
    std::string const fileName("../obj/african_head/african_head.obj");
    std::ifstream buffer(fileName.c_str());

    if(buffer) {
        std::string current_line;
        while(std::getline(buffer,current_line)) {
            if (current_line.rfind("v ",0) == 0) {
                std::istringstream stream_line(current_line.c_str());
                double coord[2];
                char trash;
                for(int i = 0 ; i < 3 ; i ++) { // change to 4 here to read also Z
                  if(i == 0)  {
                      stream_line >> trash; // removing first element
                  } else {
                      stream_line >> coord[i - 1];
                  }
                  Point p;
                  p.x = coord[0];
                  p.y = coord[1];
                  coordinates.push_back(p);
                }
                std::cout << std::endl;
            }
        }
    } else {
        std::cout << "Cannot open file at " << fileName << std::endl;
    }

    for(int i = 0 ; i < coordinates.size() ; i ++) {
        Point p = coordinates.at(i);
        std::cout <<  "P: x:" << p.x << ", y:" << p.y << std::endl;
        image.set((p.x+0.5) * 300, (p.y+0.5) * 300, red);
    }

}
int main() {
    TGAImage image(width, height, TGAImage::RGB);
    read_obj(image);
    image.write_tga_file("framebuffer.tga");
    return 0;
}
