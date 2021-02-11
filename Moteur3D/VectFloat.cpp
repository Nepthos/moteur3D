//
// Created by Adrien on 21/01/2021.
//

#include "VectFloat.h"
VectFloat::VectFloat() = default;


VectFloat::VectFloat(float px, float py, float pz) {
    x = px;
    y = py;
    z = pz;
}

/*
 * Get the norm of the vector
 */
float VectFloat::norm() {
    return std::sqrt(x * x + y * y + z * z);
}

/*
 * Normalize the vector
 */
void VectFloat::normalize() {
    float norm = this->norm();
    x /= norm;
    y /= norm;
    z /= norm;
}

/*
 * access vector data as an array
 */
float VectFloat::operator[](int idx) {
    return idx == 0 ? x : idx == 1 ? y : z;
}

Matrix VectFloat::getMatrix() {
    Matrix vecMatrix(4,1);
    vecMatrix.set_at(0,0, x);
    vecMatrix.set_at(1,0, y);
    vecMatrix.set_at(2,0, z);
    vecMatrix.set_at(3,0,1.0);
    return vecMatrix;
}

void VectFloat::print() {
    std::cout <<  "VectFloat x:" << x << " ,y:" << y << " ,z:" << z << std::endl;

}


VectFloat VectFloat::divide(float value) {
    if(value != 0) return { x /= value,y /= value,z /= value};
    else return {x, y, z};
}