//
// Created by Adrien on 21/01/2021.
//

#ifndef MOTEUR3D_VECTFLOAT_H
#define MOTEUR3D_VECTFLOAT_H
#include <cmath>
#include "Matrix.h"
class Matrix;

class VectFloat {
public:
    float x;
    float y;
    float z;

    VectFloat();
    VectFloat(float px, float py, float pz);
    float norm();
    void normalize();
    float operator[](int idx);
    VectFloat divide(float value);
    Matrix getMatrix();
    void print();
    VectFloat operator-();

};


#endif //MOTEUR3D_VECTFLOAT_H
