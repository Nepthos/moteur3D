//
// Created by adrien on 07/01/2021.
//

#ifndef MOTEUR3D_POINT_H
#define MOTEUR3D_POINT_H


class Point {
public:
    int x;
    int y;
    int z;
    Point();
    Point(int px, int py, int pz) {
        x = px;
        y = py;
        z = pz;
    }
};


#endif //MOTEUR3D_POINT_H
