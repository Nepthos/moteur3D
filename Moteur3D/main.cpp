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

// determinant value for three vectors using x and y coords
int determinant(VectInt p1, VectInt p2, VectInt p3) {
    return (p1.x - p3.x) * (p2.y - p3.y) - (p1.y - p3.y) * (p2.x - p3.x);
}


// cross product vector of two vectors
VectFloat crossProduct(VectFloat p1, VectFloat p2) {
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
    int v1 = determinant(test, p1, p2);
    int v2 = determinant(test, p2, p3);
    int v3 = determinant(test, p3, p1);
    return (v1 <= 0 && v2 <= 0 && v3 <= 0) || (v1 >= 0 && v2 >= 0 && v3 >= 0); // check with equals to avoid dead pixels on boundaries when drawing
}


// barycentric coordinates between triangle and given point
VectFloat barycentric(VectInt p1, VectInt p2, VectInt p3, VectInt test) {
    VectFloat AB(p2.x - p1.x, p2.y - p1.y, p2.z - p1.z);
    VectFloat AC(p3.x - p1.x, p3.y - p1.y, p3.z - p1.z);
    VectFloat AP(test.x - p1.x, test.y - p1.y, test.z - p1.z);

    float lambda3 = crossProduct(AB,AP).z / crossProduct(AB,AC).z;
    float lambda2 = crossProduct(AP,AC).z / crossProduct(AB,AC).z;
    float lambda1 = 1.f -lambda2-lambda3;

    return {lambda1,lambda2,lambda3};
}

// centroid of triangle
VectInt centroid(VectInt p1, VectInt p2, VectInt p3) {
    VectInt barycenter;
    barycenter.x = (p1.x + p2.x + p3.x) / 3;
    barycenter.y = (p1.y + p2.y + p3.x) / 3;
    barycenter.z = (p1.z + p2.z + p3.x) / 3;
    return barycenter;
}

/*
 * Method used to draw a filled triangle
 */
void fillTriangle(VectInt p1, VectInt p2, VectInt p3, TGAImage &image, float* zbuffer, TGAImage &texture, VectFloat vt1, VectFloat vt2, VectFloat vt3,VectFloat p1d, VectFloat p2d, VectFloat p3d) {
    // creating a square starting by the bottom left VectInt to the top right VectInt of the triangle
    VectInt bottomLeft = getCorner(p1,p2,p3, false);
    VectInt topRight = getCorner(p1,p2,p3,true);

    // color triangles using light
    VectFloat light(0, 0, 1);
    VectFloat AB(p2d.x - p1d.x, p2d.y - p1d.y, p2d.z - p1d.z);
    VectFloat AC(p3d.x - p1d.x, p3d.y - p1d.y, p3d.z - p1d.z);
    VectFloat norm = crossProduct(AB, AC);
    //std::cout << "Norm vector before : x" << norm.x << ", y" << norm.y << ", z" << norm.z << std::endl; // debug normalization
    norm.normalize();
    //std::cout << "Norm vector : x" << norm.x << ", y" << norm.y << ", z" << norm.z << std::endl; // debug normalization
    float intensity = dotProduct(norm, light);
    // std::cout << "Intensity : " << intensity << std::endl; // debug light intensity
    if(intensity > 0) {
        // old light
        // TGAColor lightColor(intensity*255, intensity*255, intensity*255, 255);

        // iterating over the square
        for(int x = bottomLeft.x ; x <= topRight.x ; x++) {
            for(int y = bottomLeft.y ; y <= topRight.y ; y++) {
                // get the current VectInt
                VectInt current;
                current.x = x;
                current.y = y;
                VectFloat bary = barycentric(p1,p2,p3,current);

                float color_value_x = vt1.x * bary.x + vt2.x * bary.y + vt3.x * bary.z;
                float color_value_y = vt1.y * bary.x + vt2.y * bary.y + vt3.y * bary.z;
                // std::cout << color_value_x << " - " << color_value_y << std::endl;
                TGAColor color = texture.get(color_value_x * texture.get_width(), color_value_y * texture.get_height()) * intensity;
                float z_value = p1.z * bary.x + p2.z * bary.y + p3.z * bary.z;
                int z_index = current.x + current.y * width;
                //if the current VectInt is in the triangle, draw it
                if(VectIntInTriangle(p1,p2,p3,current) && zbuffer[z_index] < z_value) {
                    zbuffer[z_index] = z_value;
                    image.set(x,y,color);
                }
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
void drawFacets(std::vector<VectInt> coordinates, std::vector<int> facets, TGAImage &image, std::vector<VectFloat> precise, std::vector<int> facets_txt, std::vector<VectFloat> vt, TGAImage &texture) {
    // browse the facets
    // facets format : v1/vt1 v2/vt2 v3/vt3
    // foreach facets

    float* zbuffer = new float[width * height + width];
    for(int j = 0 ; j < width * height + width ; j++) {
        zbuffer[j] = std::numeric_limits<float>::lowest();
    }
    for (int i = 3 ; i < facets.size() - 3 ; i += 3) {
        // get the real and integer points for the current facet
        VectInt p1 = coordinates.at(facets.at(i - 3) - 1);
        VectInt p2 = coordinates.at(facets.at(i - 2) - 1);
        VectInt p3 = coordinates.at(facets.at(i - 1) - 1);
        VectFloat p1d = precise.at(facets.at(i - 3) - 1);
        VectFloat p2d = precise.at(facets.at(i - 2) - 1);
        VectFloat p3d = precise.at(facets.at(i - 1) - 1);

        VectFloat vt1 = vt.at(facets_txt.at(i - 3) - 1);
        VectFloat vt2 = vt.at(facets_txt.at(i - 2) - 1);
        VectFloat vt3 = vt.at(facets_txt.at(i - 1) - 1);


        // draw a line from x,y of v1 to v2, of v2 to v3 and of v3 to v1 ; uncomment to draw simple triangles
        // drawTriangle(p1, p2, p3, image, white);

        // generate a random color ; uncomment to draw filled colored triangles
        // TGAColor randomColor(std::rand()%255,std::rand()%255,std::rand()%255,255);
        // fill a triangle using the vertices and the random color
        // fillTriangle(p1, p2, p3,image, randomColor);
        fillTriangle(p1, p2, p3,image, zbuffer, texture, vt1, vt2, vt3, p1d, p2d, p3d);

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

    std::vector<VectFloat> vt_vect_list; // vt

    // list of the facets read from the obj file
    std::vector<int> facets;
    std::vector<int> facets_textures; // vt indexes

    TGAImage text_image;
    if(text_image.read_tga_file("../obj/african_head/african_head_diffuse.tga")) {
        std::cout << "ok" << std::endl;
    }

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
                int indexes_text[3];
                int trashNumber;
                // storing needed facet data in array (facets format : v1/vt1 v2/vt2 v3/vt3)
                stream_line >> trash >> indexes[0] >> trash >> indexes_text[0] >> trash >> trashNumber >> indexes[1] >> trash >> indexes_text[1] >> trash >> trashNumber >> indexes[2] >> trash >> indexes_text[2];
                // std::cout << "F: "; //uncomment facets debug print
                for(int i = 0 ; i < 3 ; i++) {
                    facets.push_back(indexes[i]);
                    facets_textures.push_back(indexes_text[i]);
                    // std::cout << indexes[i] << " "; //uncomment facets debug print
                }
                //std::cout << std::endl; uncomment facets debug print
            } else if(current_line.rfind("vt ", 0) == 0) {
                VectFloat vt;
                stream_line >> trash >> trash >> vt.x >> vt.y >> vt.z;
                vt_vect_list.push_back(vt);
            }
        }
    } else {
        std::cout << "Cannot open file at " << fileName << std::endl;
    }

     // uncomment points debug list
    /* for(int i = 0 ; i < vt_vect_list.size() ; i ++) {
        VectFloat p = vt_vect_list.at(i);
        std::cout <<  "P: x:" << p.x << ", y:" << p.y << std::endl;

    }
    std::cout << int_vect_list.size() << std::endl;
    */
    text_image.flip_vertically();
    // draw the facets
    drawFacets(int_vect_list, facets, image, float_vect_list, facets_textures,vt_vect_list, text_image);
}


int main() {
    TGAImage image(width, height, TGAImage::RGB);
    read_obj(image);
    //image.flip_vertically();
    image.write_tga_file("framebuffer.tga");
    return 0;
}
