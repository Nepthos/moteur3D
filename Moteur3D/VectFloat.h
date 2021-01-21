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
    VectFloat(float px, float py, float pz) {
        x = px;
        y = py;
        z = pz;
    }

    void normalise() {
        x = std::sqrt(x * x);
        y = std::sqrt(y * y);
        z = std::sqrt(z * z);
    }
};


#endif //MOTEUR3D_VECTFLOAT_H
