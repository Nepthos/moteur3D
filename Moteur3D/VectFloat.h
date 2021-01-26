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

    /*
     * Get the norm of the vector
     */
    float norm() {
        return std::sqrt(x * x + y * y + z * z);
    }

    /*
     * Normalize the vector
     */
    void normalize() {
        float norm = this->norm();
        x /= norm;
        y /= norm;
        z /= norm;
    }

};


#endif //MOTEUR3D_VECTFLOAT_H
