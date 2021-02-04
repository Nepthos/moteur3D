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