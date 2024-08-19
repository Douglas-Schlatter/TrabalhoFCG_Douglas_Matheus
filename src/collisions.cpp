#include <glm/vec4.hpp>
#include "matrices.h"

void CheckBounds(glm::vec4 *minv, glm::vec4 *maxv) {
    float aux;
    if (minv->x > maxv->x) {
        aux = minv->x;
        minv->x = maxv->x;
        maxv->x = aux;
    }
    if (minv->y > maxv->y) {
        aux = minv->y;
        minv->y = maxv->y;
        maxv->y = aux;
    }
    if (minv->z > maxv->z) {
        aux = minv->z;
        minv->z = maxv->z;
        maxv->z = aux;
    }
}

int CuboPlano(glm::vec4 minCubo, glm::vec4 maxCubo, glm::vec4 pontoPlano, glm::vec4 normalPlano) {

    glm::vec4 a = glm::vec4(0,0,0,1);
    glm::vec4 b = glm::vec4(0,0,0,1);
    float result = 0;

    //xyz - Xyz
    a = glm::vec4(minCubo.x, minCubo.y, minCubo.z, 1);
    b = glm::vec4(maxCubo.x, minCubo.y, minCubo.z, 1);
    result = (dotproduct((pontoPlano - a), normalPlano))/(dotproduct((b - a), normalPlano));
    if ((result >= 0) && (result <= 1))
        return true;

    //xyz - xYz
    a = glm::vec4(minCubo.x, minCubo.y, minCubo.z, 1);
    b = glm::vec4(minCubo.x, maxCubo.y, minCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //xyz - xyZ
    a = glm::vec4(minCubo.x, minCubo.y, minCubo.z, 1);
    b = glm::vec4(minCubo.x, minCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //xYz - xYZ
    a = glm::vec4(minCubo.x, maxCubo.y, minCubo.z, 1);
    b = glm::vec4(minCubo.x, maxCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //xYz - XYz
    a = glm::vec4(minCubo.x, maxCubo.y, minCubo.z, 1);
    b = glm::vec4(maxCubo.x, maxCubo.y, minCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //xyZ - xYZ
    a = glm::vec4(minCubo.x, minCubo.y, maxCubo.z, 1);
    b = glm::vec4(minCubo.x, maxCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //xyZ - XyZ
    a = glm::vec4(minCubo.x, minCubo.y, maxCubo.z, 1);
    b = glm::vec4(maxCubo.x, minCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;
    //Xyz - XYz
    a = glm::vec4(maxCubo.x, minCubo.y, minCubo.z, 1);
    b = glm::vec4(maxCubo.x, maxCubo.y, minCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //Xyz - XyZ
    a = glm::vec4(maxCubo.x, minCubo.y, minCubo.z, 1);
    b = glm::vec4(maxCubo.x, minCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //xYZ - XYZ
    a = glm::vec4(minCubo.x, maxCubo.y, maxCubo.z, 1);
    b = glm::vec4(maxCubo.x, maxCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //XyZ - XYZ
    a = glm::vec4(maxCubo.x, minCubo.y, maxCubo.z, 1);
    b = glm::vec4(maxCubo.x, maxCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    //XYz - XYZ
    a = glm::vec4(maxCubo.x, maxCubo.y, minCubo.z, 1);
    b = glm::vec4(maxCubo.x, maxCubo.y, maxCubo.z, 1);
    result = dotproduct((pontoPlano - a), normalPlano)/dotproduct((b - a), normalPlano);
    if ((result >= 0) && (result <= 1))
        return true;

    return false;

}

bool CuboCubo(glm::vec4 min1, glm::vec4 max1, glm::vec4 min2, glm::vec4 max2) {
    CheckBounds(&min1, &max1);
    CheckBounds(&min2, &max2);

    if ((min1.x <= max2.x) && (max1.x >= min2.x))
        if ((min1.y <= max2.y) && (max1.y >= min2.y))
            if ((min1.z <= max2.z) && (max1.z >= min2.z))
                return true;
    return false;
}

bool EsferaEsfera(glm::vec4 centro1, float raio1, glm::vec4 centro2, float raio2) {
    if (norm(centro1 - centro2) <= (raio1 + raio2)) {
        return true;
    } else {
        return false;
    }

}
