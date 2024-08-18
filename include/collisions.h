#ifndef COLLISIONS_H
#define COLLISIONS_H

bool cuboPlano(glm::vec4 minCubo, glm::vec4 maxCubo, glm::vec4 pontoPlano, glm::vec4 normalPlano);
bool CuboCubo(glm::vec4 min1, glm::vec4 max1, glm::vec4 min2, glm::vec4 max2);
void CheckBounds(glm::vec4 *minv, glm::vec4 *maxv);
#endif // COLLISIONS_H
