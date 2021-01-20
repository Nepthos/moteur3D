#include "tgaimage.h"
#include "Point.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

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


Point getCorner(Point p1, Point p2, Point p3, bool top) {
    Point corner;
    if(top) {
        corner.x = std::max(p1.x, std::max(p2.x, p3.x));
        corner.y = std::max(p1.y, std::max(p2.y, p3.y));
        std::cout << "TOP : x" << corner.x << " ,y" << corner.y << std::endl;
    } else {
        corner.x = std::min(p1.x, std::min(p2.x, p3.x));
        corner.y = std::min(p1.y, std::min(p2.y, p3.y));
        std::cout << "BOTTOM : x" << corner.x << " ,y" << corner.y << std::endl;
    }
    return corner;
}

// cross product of the three vectors
int crossProduct(Point p1, Point p2, Point p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
}

// method used to check if a given vertice test is in the triangle represented by
// the vertices p1 p2 and p3
// we make the cross product of 2 vertices of the triangle and our test vertice
// if the cross products are all positive, or all negative, we are inside the triangle
// otherwise if some are positive and some are not we are not inside of the triangle
bool pointInTriangle(Point p1, Point p2, Point p3, Point test) {
    int v1 = crossProduct(test, p1, p2);
    int v2 = crossProduct(test, p2, p3);
    int v3 = crossProduct(test, p3, p1);
    return (v1 <= 0 && v2 <= 0 && v3 <= 0) || (v1 >= 0 && v2 >= 0 && v3 >= 0); // check with equals to avoid dead pixels on boundaries when drawing
}

// unused method for now, might be used when working with Z
// barycentric coordinates of triangle (in geometry synonym of centroid)
Point barycenter(Point p1, Point p2, Point p3) {
    Point barycenter;
    barycenter.x = (p1.x + p2.x + p3.x) / 3;
    barycenter.y = (p1.y + p2.y + p3.x) / 3;
    barycenter.z = (p1.z + p2.z + p3.x) / 3;
    return barycenter;
}

void fillTriangle(Point p1, Point p2, Point p3, TGAImage &image, TGAColor color) {
    // Point bc = barycenter(p1,p2,p3);
    // creating a square starting by the bottom left point to the top right point of the triangle
    Point bottomLeft = getCorner(p1,p2,p3, false);
    Point topRight = getCorner(p1,p2,p3,true);

    // iterating over the square
    for(int x = bottomLeft.x ; x <= topRight.x ; x++) {
        for(int y = bottomLeft.y ; y <= topRight.y ; y++) {
            // get the current point
            Point current;
            current.x = x;
            current.y = y;
            //if the current point is in the triangle, draw it
            if(pointInTriangle(p1,p2,p3,current)) {
                image.set(x,y,color);
            }
        }
    }


}


void drawTriangle(Point p1, Point p2, Point p3, TGAImage &image, TGAColor color) {
    line(p1.x, p1.y, p2.x, p2.y, image, color);
    line(p2.x, p2.y, p3.x, p3.y, image, color);
    line(p3.x, p3.y, p1.x, p1.y, image, color);
}

void drawFacets(std::vector<Point> coordinates, std::vector<int> facets, TGAImage &image) {
    // browse the facets
    // facets format : v1/vt1 v2/vt2 v3/vt3
    // foreach facets
    for (int i = 3 ; i < facets.size() - 3 ; i += 3) {
       //  std::cout << i << std::endl;
        Point p1 = coordinates.at(facets.at(i - 3) - 1);
        Point p2 = coordinates.at(facets.at(i - 2) - 1);
        Point p3 = coordinates.at(facets.at(i - 1) - 1);

        // draw a line from x,y of v1 to v2, of v2 to v3 and of v3 to v1
        // drawTriangle(p1, p2, p3, image, white);

        // generate a random color
        TGAColor randomColor(std::rand()%255,std::rand()%255,std::rand()%255,255);
        // fill a triangle using the vertices and the random color
        fillTriangle(p1, p2, p3,image, randomColor);

        // old code for rgb drawing and/or filling
      /*  if(i / 3 % 3 == 0) {
            //drawTriangle(p1, p2, p3, image, red);
            fillTriangle(p1, p2, p3, image, red);
        } else if (i / 3 % 3 == 1) {
            //drawTriangle(p1, p2, p3, image, green);
            fillTriangle(p1, p2, p3, image, green);
        } else {
            //drawTriangle(p1, p2, p3, image, blue);
            fillTriangle(p1, p2, p3, image, blue);
        }
        */
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
                stream_line >> trash >> coord[0] >> coord[1] >> coord[2];
                p.x = coord[0] * (width/2) + (width/2);
                p.y = coord[1] * (height/2) + (height/2);
                p.z = coord[2] * (width/2) + (width/2);
                // uncomment to draw red dots
                //image.set(p.x, p.y, red);
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
    } else {
        std::cout << "Cannot open file at " << fileName << std::endl;
    }

    for(int i = 0 ; i < coordinates.size() ; i ++) {
        Point p = coordinates.at(i);
        std::cout <<  "P: x:" << p.x << ", y:" << p.y << std::endl;

    }

    drawFacets(coordinates, facets, image);
    std::cout << coordinates.size() << std::endl;

}


int main() {
    TGAImage image(width, height, TGAImage::RGB);
    read_obj(image);
    //image.flip_vertically();
    image.write_tga_file("framebuffer.tga");
    return 0;
}
