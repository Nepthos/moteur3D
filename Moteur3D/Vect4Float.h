//
// Created by Adrien on 11/02/2021.
//

#ifndef MOTEUR3D_VECT4FLOAT_H
#define MOTEUR3D_VECT4FLOAT_H
// TODO : refactor me using templates
#include <cmath>
#include "Matrix.h"
class Matrix;
class Vect4Float {
public:
    float x;
    float y;
    float z;
    float a;

    Vect4Float();
    Vect4Float(float px, float py, float pz, float pa);
    float norm();
    void normalize();
    float operator[](int idx);
    Vect4Float divide(float value);
    Matrix getMatrix();
    void print();

};


#endif //MOTEUR3D_VECT4FLOAT_H
