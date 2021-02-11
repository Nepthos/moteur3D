//
// Created by adrien on 07/01/2021.
//

#include "VectInt.h"

VectInt::VectInt() = default;

VectInt::VectInt(int px, int py, int pz) {
    x = px;
    y = py;
    z = pz;
}
Matrix VectInt::getMatrix() {
    Matrix vecMatrix(4,1);
    vecMatrix.set_at(0,0, x);
    vecMatrix.set_at(1,0, y);
    vecMatrix.set_at(2,0, z);
    vecMatrix.set_at(3,0,1.0);
    return vecMatrix;
}

void VectInt::print() {
    std::cout << "VectInt x:" << x << " ,y:" << y << " ,z:" << z << std::endl;
}

VectInt::VectInt(VectFloat aFloat, int world_size) {
    x = int(aFloat.x * (world_size/2) + (world_size/2));
    y = int(aFloat.y * (world_size/2) + (world_size/2));
    z = int(aFloat.z * (world_size/2) + (world_size/2));
}

