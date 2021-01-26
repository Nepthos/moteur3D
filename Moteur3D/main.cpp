#include "tgaimage.h"
#include "VectInt.h"
#include "VectFloat.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <tgmath.h>

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


VectInt getCorner(VectInt p1, VectInt p2, VectInt p3, bool top) {
    VectInt corner;
    if(top) {
        corner.x = std::max(p1.x, std::max(p2.x, p3.x));
        corner.y = std::max(p1.y, std::max(p2.y, p3.y));
        //std::cout << "TOP : x" << corner.x << " ,y" << corner.y << std::endl; // debug corner
    } else {
        corner.x = std::min(p1.x, std::min(p2.x, p3.x));
        corner.y = std::min(p1.y, std::min(p2.y, p3.y));
        //std::cout << "BOTTOM : x" << corner.x << " ,y" << corner.y << std::endl; // debug corner
    }
    return corner;
}

// cross product value for three vectors
int crossProduct(VectInt p1, VectInt p2, VectInt p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
}


// cross product vector of two vectors
VectFloat crossProductVectD(VectFloat p1, VectFloat p2) {
    VectFloat vect;
    vect.x = p1.y * p2.z - p1.z * p2.y;
    vect.y = p1.z * p2.x - p1.x * p2.z;
    vect.z = p1.x * p2.y - p1.y * p2.x;
    return vect;
}

float dotProduct(VectFloat p1, VectFloat p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}


// method used to check if a given vertice test is in the triangle represented by
// the vertices p1 p2 and p3
// we make the cross product of 2 vertices of the triangle and our test vertice
// if the cross products are all positive, or all negative, we are inside the triangle
// otherwise if some are positive and some are not we are not inside of the triangle
bool VectIntInTriangle(VectInt p1, VectInt p2, VectInt p3, VectInt test) {
    int v1 = crossProduct(test, p1, p2);
    int v2 = crossProduct(test, p2, p3);
    int v3 = crossProduct(test, p3, p1);
    return (v1 <= 0 && v2 <= 0 && v3 <= 0) || (v1 >= 0 && v2 >= 0 && v3 >= 0); // check with equals to avoid dead pixels on boundaries when drawing
}

// TODO fixme this is centroid, not barycenter
// unused method for now, might be used when working with Z
// barycentric coordinates of triangle
VectInt barycenter(VectInt p1, VectInt p2, VectInt p3) {
    VectInt barycenter;
    barycenter.x = (p1.x + p2.x + p3.x) / 3;
    barycenter.y = (p1.y + p2.y + p3.x) / 3;
    barycenter.z = (p1.z + p2.z + p3.x) / 3;
    return barycenter;
}

/*
 * Method used to draw a filled triangle
 */
void fillTriangle(VectInt p1, VectInt p2, VectInt p3, TGAImage &image, TGAColor color) {
    // creating a square starting by the bottom left VectInt to the top right VectInt of the triangle
    VectInt bottomLeft = getCorner(p1,p2,p3, false);
    VectInt topRight = getCorner(p1,p2,p3,true);

    // iterating over the square
    for(int x = bottomLeft.x ; x <= topRight.x ; x++) {
        for(int y = bottomLeft.y ; y <= topRight.y ; y++) {
            // get the current VectInt
            VectInt current;
            current.x = x;
            current.y = y;
            //if the current VectInt is in the triangle, draw it
            if(VectIntInTriangle(p1,p2,p3,current)) {
                image.set(x,y,color);
            }
        }
    }
}

/*
 * Method used to draw the borders of a tirangle
 */
void drawTriangle(VectInt p1, VectInt p2, VectInt p3, TGAImage &image, TGAColor color) {
    line(p1.x, p1.y, p2.x, p2.y, image, color);
    line(p2.x, p2.y, p3.x, p3.y, image, color);
    line(p3.x, p3.y, p1.x, p1.y, image, color);
}

/*
 * Method used to draw the facets : they can be drawn as filled triangles or as simple triangles
 */
void drawFacets(std::vector<VectInt> coordinates, std::vector<int> facets, TGAImage &image, std::vector<VectFloat> precise) {
    // browse the facets
    // facets format : v1/vt1 v2/vt2 v3/vt3
    // foreach facets
    for (int i = 3 ; i < facets.size() - 3 ; i += 3) {
        // get the real and integer points for the current facet
        VectInt p1 = coordinates.at(facets.at(i - 3) - 1);
        VectInt p2 = coordinates.at(facets.at(i - 2) - 1);
        VectInt p3 = coordinates.at(facets.at(i - 1) - 1);
        VectFloat p1d = precise.at(facets.at(i - 3) - 1);
        VectFloat p2d = precise.at(facets.at(i - 2) - 1);
        VectFloat p3d = precise.at(facets.at(i - 1) - 1);

        // draw a line from x,y of v1 to v2, of v2 to v3 and of v3 to v1 ; uncomment to draw simple triangles
        // drawTriangle(p1, p2, p3, image, white);

        // generate a random color ; uncomment to draw filled colored triangles
        // TGAColor randomColor(std::rand()%255,std::rand()%255,std::rand()%255,255);
        // fill a triangle using the vertices and the random color
        // fillTriangle(p1, p2, p3,image, randomColor);

        // color triangles using light
        VectFloat light(0, 0, 1);
        VectFloat AB(p2d.x - p1d.x, p2d.y - p1d.y, p2d.z - p1d.z);
        VectFloat AC(p3d.x - p1d.x, p3d.y - p1d.y, p3d.z - p1d.z);
        VectFloat norm = crossProductVectD(AB, AC);
        //std::cout << "Norm vector before : x" << norm.x << ", y" << norm.y << ", z" << norm.z << std::endl; // debug normalization
        norm.normalize();
        //std::cout << "Norm vector : x" << norm.x << ", y" << norm.y << ", z" << norm.z << std::endl; // debug normalization
        float intensity = dotProduct(norm, light);
        // std::cout << "Intensity : " << intensity << std::endl; // debug light intensity
        if(intensity > 0) {
            TGAColor lightColor(intensity*255, intensity*255, intensity*255, 255);
            fillTriangle(p1, p2, p3,image, lightColor);
        }
    }
}

/*
 * Reads the obj file
 */
void read_obj(TGAImage &image) {
    // list of vertices to be drawn using integers
    std::vector<VectInt> int_vect_list;
    // list of real vertices read from the obj file
    std::vector<VectFloat> float_vect_list;
    // list of the facets read from the obj file
    std::vector<int> facets;

    // obj file to read
    std::string const fileName("../obj/african_head/african_head.obj");
    std::ifstream buffer(fileName.c_str());
    if(buffer) {
        std::string current_line;
        while(std::getline(buffer,current_line)) {
            std::istringstream stream_line(current_line.c_str());
            char trash;
            VectInt p;
            VectFloat pf;
            if (current_line.rfind("v ",0) == 0) {
                double coord[3];
                stream_line >> trash >> coord[0] >> coord[1] >> coord[2]; // storing real vertices points in array
                // assign draw coords in integer vector
                p.x = coord[0] * (width/2) + (width/2);
                p.y = coord[1] * (height/2) + (height/2);
                p.z = coord[2] * (width/2) + (width/2);

                // assign real coords in float vector
                pf.x = coord[0];
                pf.y = coord[1];
                pf.z = coord[2];

                //image.set(p.x, p.y, red); // uncomment to draw red dots map

                // storing current vertice in list
                int_vect_list.push_back(p);
                float_vect_list.push_back(pf);
            } else if(current_line.rfind("f ",0) == 0){
                int indexes[3];
                int trashNumber;
                // storing needed facet data in array (facets format : v1/vt1 v2/vt2 v3/vt3)
                stream_line >> trash >> indexes[0] >> trash >> trashNumber >> trash >> trashNumber >> indexes[1] >> trash >> trashNumber >> trash >> trashNumber >> indexes[2];
                // std::cout << "F: "; //uncomment facets debug print
                for(int i = 0 ; i < 3 ; i++) {
                    facets.push_back(indexes[i]);
                    // std::cout << indexes[i] << " "; //uncomment facets debug print
                }
                //std::cout << std::endl; uncomment facets debug print
            }
        }
    } else {
        std::cout << "Cannot open file at " << fileName << std::endl;
    }

    /* // uncomment points debug list
    for(int i = 0 ; i < int_vect_list.size() ; i ++) {
        VectInt p = int_vect_list.at(i);
        std::cout <<  "P: x:" << p.x << ", y:" << p.y << std::endl;

    }
    std::cout << int_vect_list.size() << std::endl;
    */

    // draw the facets
    drawFacets(int_vect_list, facets, image, float_vect_list);
}


int main() {
    TGAImage image(width, height, TGAImage::RGB);
    read_obj(image);
    //image.flip_vertically();
    image.write_tga_file("framebuffer.tga");
    return 0;
}
