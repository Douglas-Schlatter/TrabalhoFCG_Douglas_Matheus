#include <glm/vec4.hpp>
#include "matrices.h"

int cuboPlano(glm::vec4 minCubo, glm::vec4 maxCubo, glm::vec4 pontoPlano, glm::vec4 normalPlano) {

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
    if ((min1.x <= max2.x) && (max1.x >= min2.x))
        if ((min1.y <= max2.y) && (max1.y >= min2.y))
            if ((min1.z <= max2.z) && (max1.z >= min2.z))
                return true;
    return false;
}
