//
// Created by adrien on 07/01/2021.
//
#include <cmath>

#ifndef MOTEUR3D_POINT_H
#define MOTEUR3D_POINT_H


class VectInt {
public:
    int x;
    int y;
    int z;
    VectInt();
    VectInt(int px, int py, int pz) {
        x = px;
        y = py;
        z = pz;
    }
};


#endif //MOTEUR3D_POINT_H
