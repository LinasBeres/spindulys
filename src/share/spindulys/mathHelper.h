#ifndef MATH_HELPER_H
#define MATH_HELPER_H

#include <math.h>

#include "../spindulys.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

template<typename T, typename A>
A USDToVectMatrixMultiply(const A& vector, const T& usdMatrix)
{
    return A(vector.x * usdMatrix[0][0] +
            vector.y * usdMatrix[1][0] +
            vector.z * usdMatrix[2][0],
        vector.x * usdMatrix[0][1] +
            vector.y * usdMatrix[1][1] +
            vector.z * usdMatrix[2][1],
        vector.x * usdMatrix[0][2] +
            vector.y * usdMatrix[1][2] +
            vector.z * usdMatrix[2][2]);
}

template<typename T, typename A>
A USDToUSDMatrixMultiply(const A& usdVector, const T& usdMatrix)
{
    return A(usdMatrix.TransformDir(usdVector));
}

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // MATH_HELPER_H
