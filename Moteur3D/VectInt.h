//
// Created by adrien on 07/01/2021.
//
#include <cmath>
#include "Matrix.h"
#ifndef MOTEUR3D_POINT_H
#define MOTEUR3D_POINT_H
class Matrix;


class VectInt {
public:
    VectInt(VectFloat aFloat, int w_size);

    int x;
    int y;
    int z;
    VectInt();
    VectInt(int px, int py, int pz);
    Matrix getMatrix();
    void print();
};


#endif //MOTEUR3D_POINT_H
