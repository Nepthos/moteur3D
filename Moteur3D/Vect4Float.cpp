//
// Created by Adrien on 11/02/2021.
//

#include "Vect4Float.h"
// TODO : refactor me using templates

Vect4Float::Vect4Float() = default;


Vect4Float::Vect4Float(float px, float py, float pz, float pa) {
    x = px;
    y = py;
    z = pz;
    a = pa;
}

/*
 * Get the norm of the vector
 */
float Vect4Float::norm() {
    return std::sqrt(x * x + y * y + z * z + a * a);
}

/*
 * Normalize the vector
 */
void Vect4Float::normalize() {
    float norm = this->norm();
    x /= norm;
    y /= norm;
    z /= norm;
    a /= norm;
}

/*
 * access vector data as an array
 */
float Vect4Float::operator[](int idx) {
    return idx == 0 ? x : idx == 1 ? y : idx == 2 ? z : a;
}

Matrix Vect4Float::getMatrix() {
    Matrix vecMatrix(4,1);
    vecMatrix.set_at(0,0, x);
    vecMatrix.set_at(1,0, y);
    vecMatrix.set_at(2,0, z);
    vecMatrix.set_at(3,0, a);
    return vecMatrix;
}

void Vect4Float::print() {
    std::cout <<  "Vect4Float x:" << x << " ,y:" << y << " ,z:" << z << " ,a:" << a << std::endl;

}


Vect4Float Vect4Float::divide(float value) {
    if(value != 0) return { x /= value,y /= value,z /= value, a/=value};
    else return {x, y, z, a};
}