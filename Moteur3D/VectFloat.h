//
// Created by Adrien on 21/01/2021.
//

#ifndef MOTEUR3D_VECTFLOAT_H
#define MOTEUR3D_VECTFLOAT_H
#include <cmath>

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

};


#endif //MOTEUR3D_VECTFLOAT_H
