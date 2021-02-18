#include "tgaimage.h"
#include "VectInt.h"
#include "VectFloat.h"
#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <tgmath.h>
#include <array>

constexpr int width = 1000;
constexpr int height = 1000;

VectFloat camera(-1,1,3); // eye
VectFloat center(0,0,0); // target
VectFloat vertical(0,1,0); // vertical vector used for cross products to get other axis
Matrix model_view(4); // model view matrix
Matrix viewport(4); // viewport matrix
Matrix projection(4); // perspective projection matrix
Matrix uniform_M(4);
Matrix uniform_MIT(4);
VectFloat light(1,1,1); // input light for the scene

VectFloat final_light; // final light calculated with uniform matrix
int depth = 3000; // depth of the z-buffer

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
// we get the determinant of 2 vertices of the triangle and our test vertice
// if the determinants are all positive, or all negative, we are inside the triangle
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

// creates the model view matrix used to move the scene
void move_scene(VectFloat source, VectFloat center, VectFloat up) {
    VectFloat z(source.x-center.x,source.y-center.y,source.z-center.z); // look direction z axis
    z.normalize();
    VectFloat x = crossProduct(up,z); // cross product to get x axis
    x.normalize();
    VectFloat y = crossProduct(z,x); // cross product to get y axis
    y.normalize();
    for(int i = 0; i < 3 ; i++) {
        model_view.set_at(0,i,x[i]);
        model_view.set_at(1,i,y[i]);
        model_view.set_at(2,i,z[i]);
    }
    model_view.set_at(0, 3, dotProduct(x, -center));
    model_view.set_at(1, 3, dotProduct(y, -center));
    model_view.set_at(2, 3, dotProduct(z, -center));
}

// creates the view port matrix used to transform clip coords to screen coords
void create_viewport(int x, int y, int w, int h) {
    viewport = Matrix(4);
    viewport.set_at(0,3, x+w/2.f);
    viewport.set_at(1,3, y+h/2.f);
    viewport.set_at(2,3, depth/2.f);
    viewport.set_at(0,0, w/2.f);
    viewport.set_at(1,1, h/2.f);
    viewport.set_at(2,2, depth/2.f);
}

float intensity_flat_shading(std::array<VectFloat, 3> points) {
    // color triangles using light
    VectFloat AB(points[1].x - points[0].x, points[1].y - points[0].y, points[1].z - points[0].z);
    VectFloat AC(points[2].x - points[0].x, points[2].y - points[0].y, points[2].z - points[0].z);
    VectFloat norm = crossProduct(AB,AC);
    //std::cout << "Norm vector before : x" << norm.x << ", y" << norm.y << ", z" << norm.z << std::endl; // debug normalization
    norm.normalize();
    //std::cout << "Norm vector : x" << norm.x << ", y" << norm.y << ", z" << norm.z << std::endl; // debug normalization
    return std::max(0.f, dotProduct(norm, light));
}

float intensity_gouraud_shading(VectFloat *points_vn, VectFloat barycenter) {
    float lg_x = dotProduct(light,points_vn[0]);
    float lg_y = dotProduct(light, points_vn[1]);
    float lg_z = dotProduct(light, points_vn[2]);
    VectFloat lg(lg_x,lg_y,lg_z);
    return std::max(0.f, dotProduct(lg,barycenter));
}

float intensity_normal_mapping(VectInt col, TGAImage &nmap) {
    TGAColor val_nm = nmap.get(col.x,col.y);
    VectFloat val_nm_vct(val_nm[0]-130, val_nm[1]-130, val_nm[2]-130); // subtract value to reduce light output and enhance details, otherwise too much light
    // val_nm_vct.print();
    val_nm_vct.normalize(); // from -130 125 to -1 1
    VectFloat n = (uniform_MIT * val_nm_vct.getMatrix()).getVect(0);
    n.normalize();
    return std::max(0.f, (dotProduct(final_light,n)));
}

/*
 * Method used to draw a filled triangle
 */
void fillTriangle(TGAImage &image, float* zbuffer, TGAImage &texture, VectFloat* points, VectFloat* points_tx, VectFloat* points_vn, TGAImage &nmap) {
    // getting final points from the world view matrix
    Vect4Float p1f = (viewport * projection * model_view * points[0].getMatrix()).getVect4(0);
    Vect4Float p2f = (viewport * projection * model_view * points[1].getMatrix()).getVect4(0);
    Vect4Float p3f = (viewport * projection * model_view * points[2].getMatrix()).getVect4(0);

    // from 4d world points to 3d screen points
    VectInt p1((p1f.x/p1f.a), (p1f.y/p1f.a), (p1f.z/p1f.a));
    VectInt p2((p2f.x/p2f.a), (p2f.y/p2f.a),(p2f.z/p2f.a));
    VectInt p3((p3f.x/p3f.a), (p3f.y/p3f.a),(p3f.z/p3f.a));

    // creating a square starting by the bottom left VectInt to the top right VectInt of the triangle
    VectInt bottomLeft = getCorner(p1,p2,p3, false);
    VectInt topRight = getCorner(p1,p2,p3,true);


    // iterating over the square
        #pragma omp parallel for
        for(int x = bottomLeft.x ; x <= topRight.x ; x++) {
            for(int y = bottomLeft.y ; y <= topRight.y ; y++) {

                // get the current VectInt
                VectInt current;
                current.x = x;
                current.y = y;
                VectFloat bary = barycentric(p1,p2,p3,current);


                float z_value = p1.z * bary.x + p2.z * bary.y + p3.z * bary.z;
                int z_index = current.x + current.y * height;
                if(z_index < width * height && z_index >= 0) { // check boundaries for z index array
                    //if the current VectInt is in the triangle, and zvalue is above current stored zbuffer draw it
                    if(VectIntInTriangle(p1,p2,p3,current) && zbuffer[z_index] < z_value) {
                        // current scene light
                        int color_value_x = (points_tx[0].x * bary.x + points_tx[1].x * bary.y + points_tx[2].x * bary.z) * texture.get_width();
                        int color_value_y = (points_tx[0].y * bary.x + points_tx[1].y * bary.y + points_tx[2].y * bary.z) * texture.get_height();
                        VectInt col(color_value_x , color_value_y, 0);
                        //float intensity = intensity_flat_shading({VectFloat(p1f),VectFloat(p2f),VectFloat(p3f)}); // flat
                        //float intensity = intensity_gouraud_shading(points_vn, bary); // gouraud
                        float intensity = intensity_normal_mapping(col, nmap);
                        TGAColor color = texture.get(col.x, col.y) * (intensity);
                        TGAColor debug_light = TGAColor(255,255,255) * intensity;
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
void render(TGAImage &image, std::vector<int> facets_points, std::vector<VectFloat> points, std::vector<int> facets_tx, std::vector<VectFloat> points_tx,  TGAImage &texture, std::vector<int> facets_nm, std::vector<VectFloat> points_nm, TGAImage &nmap) {
    // browse the facets_points
    // facets_points format : v1/vt1 v2/vt2 v3/vt3
    // foreach facets_points

    float* zbuffer = new float[width * height];
    for(int j = 0 ; j < width * height ; j++) {
        zbuffer[j] = std::numeric_limits<float>::lowest();
    }
    for (int i = 3 ; i < facets_points.size() - 3 ; i += 3) {

        VectFloat curr_points[3];
        curr_points[0] = points.at(facets_points.at(i - 3) - 1);
        curr_points[1] = points.at(facets_points.at(i - 2) - 1);
        curr_points[2] = points.at(facets_points.at(i - 1) - 1);

        VectFloat curr_tx[3];
        curr_tx[0] = points_tx.at(facets_tx.at(i - 3) - 1);
        curr_tx[1] = points_tx.at(facets_tx.at(i - 2) - 1);
        curr_tx[2] = points_tx.at(facets_tx.at(i - 1) - 1);

        VectFloat curr_vn[3];
        curr_vn[0] = points_nm.at(facets_nm.at(i - 3) - 1);
        curr_vn[1] = points_nm.at(facets_nm.at(i - 2) - 1);
        curr_vn[2] = points_nm.at(facets_nm.at(i - 1) - 1);

        // draw a line from x,y of v1 to v2, of v2 to v3 and of v3 to v1 ; uncomment to draw simple triangles
        // drawTriangle(p1, p2, p3, image, white);

        // generate a random color ; uncomment to draw filled colored triangles
        // TGAColor randomColor(std::rand()%255,std::rand()%255,std::rand()%255,255);
        // fill a triangle using the vertices and the random color
        // fillTriangle(p1, p2, p3,image, randomColor);
        fillTriangle(image, zbuffer, texture, curr_points, curr_tx, curr_vn, nmap);

    }
}

/*
 * Reads the obj file
 */
void read_obj(TGAImage &image) {

    // list of real vertices read from the obj file
    std::vector<VectFloat> float_vect_list; // vertices
    std::vector<VectFloat> vt_vect_list; // textures vectors vt
    std::vector<VectFloat> vn_vect_list; // normal vectors vn

    // list of the facets read from the obj file
    std::vector<int> facets;
    std::vector<int> facets_textures; // vt indexes
    std::vector<int> facets_vn; // vn indexes

    TGAImage text_image;
    TGAImage nm_image;
    //../obj/diablo3_pose/diablo3_pose_diffuse.tga
    //../obj/african_head/african_head_diffuse.tga
    if(text_image.read_tga_file("../obj/african_head/african_head_diffuse.tga")) {
        std::cout << "Texture loaded" << std::endl;
    }
    //../obj/african_head/african_head_nm.tga
    //../obj/diablo3_pose/diablo3_pose_nm.tga
    if(nm_image.read_tga_file("../obj/african_head/african_head_nm.tga")) {
        std::cout << "Normal map loaded" << std::endl;
    }

    // obj file to read
    //../obj/african_head/african_head.obj
    //../obj/diablo3_pose/diablo3_pose.obj
    std::string const fileName("../obj/african_head/african_head.obj");
    std::ifstream buffer(fileName.c_str());
    if(buffer) {
        std::string current_line;
        while(std::getline(buffer,current_line)) {

            std::istringstream stream_line(current_line.c_str());
            char trash;
            VectFloat pf;
            if (current_line.rfind("v ",0) == 0) {
                stream_line >> trash >> pf.x >> pf.y >> pf.z; // storing real vertices points in object
                float_vect_list.push_back(pf);
            } else if(current_line.rfind("f ",0) == 0){
                int indexes[3];
                int indexes_text[3];
                int indexes_vn[3];
                // storing needed facet data in array (facets format : v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3)
                stream_line >> trash >> indexes[0] >> trash >> indexes_text[0] >> trash >> indexes_vn[0] >> indexes[1] >> trash >> indexes_text[1] >> trash >> indexes_vn[1] >> indexes[2] >> trash >> indexes_text[2] >> trash >> indexes_vn[2];
                // std::cout << "F: "; //uncomment facets debug print
                for(int i = 0 ; i < 3 ; i++) {
                    facets.push_back(indexes[i]);
                    facets_textures.push_back(indexes_text[i]);
                    facets_vn.push_back(indexes_vn[i]);
                    // std::cout << indexes[i] << " "; //uncomment facets debug print
                }
                //std::cout << std::endl; uncomment facets debug print
            } else if(current_line.rfind("vt ", 0) == 0) {
                VectFloat vt;
                stream_line >> trash >> trash >> vt.x >> vt.y >> vt.z;
                vt_vect_list.push_back(vt);
            } else if(current_line.rfind("vn ", 0) == 0) {
                VectFloat vn;
                stream_line >> trash >> trash >> vn.x >> vn.y >> vn.z;
                vn.normalize();
                vn_vect_list.push_back(vn);
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
    nm_image.flip_vertically();
    render(image,facets,float_vect_list,facets_textures,vt_vect_list,text_image,facets_vn,vn_vect_list,nm_image);
}



int main() {
    move_scene(camera, center, vertical);
    create_viewport(100, 100, width-200, height-200);

    projection.set_at(3,2,-1.f/camera.z);
    uniform_M = projection * model_view;
    uniform_MIT = Matrix::invert((projection * model_view).transpose());

    //uniform_M.print();
    //projection.print();
    //uniform_MIT.print();

    // calculating final light vector
    light.normalize();
    final_light = (uniform_M * light.getMatrix()).getVect(0);
    final_light.normalize();


    TGAImage image(width, height, TGAImage::RGB);
    read_obj(image);
    
    //image.flip_vertically();
    image.write_tga_file("framebuffer.tga");
    return 0;
}
