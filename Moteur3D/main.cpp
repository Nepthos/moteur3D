#include "tgaimage.h"
#include "Point.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

constexpr int width = 1000;
constexpr int height = 1000;

const TGAColor white =  { 255 , 255 , 255 , 255};
const TGAColor red = { 255 , 0 , 0 , 255};
const TGAColor green = { 0 , 255 , 0 , 255};
const TGAColor blue = { 0 , 0 , 255 , 255};
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
    std::vector<int> facets;

    std::string const fileName("../obj/african_head/african_head.obj");
    std::ifstream buffer(fileName.c_str());

    if(buffer) {
        std::string current_line;
        while(std::getline(buffer,current_line)) {
            std::istringstream stream_line(current_line.c_str());
            char trash;
            Point p;
            if (current_line.rfind("v ",0) == 0) {
                double coord[3];
                stream_line >> trash >> p.x >> p.y >> p.z;
                // uncomment to draw dots
                //image.set(p.x * 100 + 500, p.y * 100 + 500, red);
                coordinates.push_back(p);
            } else if(current_line.rfind("f ",0) == 0){
                int indexes[3];
                int trashNumber;
                stream_line >> trash >> indexes[0] >> trash >> trashNumber >> trash >> trashNumber >> indexes[1] >> trash >> trashNumber >> trash >> trashNumber >> indexes[2];
                std::cout << "F: ";
                for(int i = 0 ; i < 3 ; i++) {
                    facets.push_back(indexes[i]);
                    std::cout << indexes[i] << " ";
                }
                std::cout << std::endl;
            }



        }
        // browse the faces
        // faces format : v1/vt1 v2/vt2 v3/vt3
        // foreach face draw a line from x,y of v1 to v2, of v2 to v3 and of v3 to v1
        std::cout << facets.size() << "@@" << coordinates.size() << std::endl;
        for (int i = 3 ; i < facets.size() - 3 ; i += 3) {
            std::cout << i << std::endl;
            Point p1 = coordinates.at(facets.at(i - 3) - 1);
            Point p2 = coordinates.at(facets.at(i - 2) - 1);
            Point p3 = coordinates.at(facets.at(i - 1) - 1);

            line(p1.x * 500 + 500, p1.y * 500 + 500, p2.x * 500 + 500, p2.y * 500 + 500, image, green);
            line(p2.x * 500 + 500, p2.y * 500 + 500, p3.x * 500 + 500, p3.y * 500 + 500, image, green);
            line(p3.x * 500 + 500, p3.y * 500 + 500, p1.x * 500 + 500, p1.y * 500 + 500, image, green);
        }
    } else {
        std::cout << "Cannot open file at " << fileName << std::endl;
    }

    for(int i = 0 ; i < coordinates.size() ; i ++) {
        Point p = coordinates.at(i);
        std::cout <<  "P: x:" << p.x << ", y:" << p.y << std::endl;

    }

    std::cout << coordinates.size() << std::endl;

}
int main() {
    TGAImage image(width, height, TGAImage::RGB);
    read_obj(image);
    //image.flip_vertically();
    image.write_tga_file("framebuffer.tga");
    return 0;
}
