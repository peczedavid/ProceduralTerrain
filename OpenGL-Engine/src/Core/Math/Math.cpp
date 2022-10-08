#include "pch.h"
#include "Math.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>

bool DecomposeTransform(const glm::mat4& transform, glm::vec3& translation, glm::vec3& rotation, glm::vec3& scale)
{
    glm::mat4 localMatrix(transform);

    if (glm::epsilonEqual(localMatrix[3][3], 0.0f, glm::epsilon<float>()))
        return false;

    if (
        glm::epsilonNotEqual(localMatrix[0][3], 0.0f, glm::epsilon<float>()) ||
        glm::epsilonNotEqual(localMatrix[1][3], 0.0f, glm::epsilon<float>()) ||
        glm::epsilonNotEqual(localMatrix[2][3], 0.0f, glm::epsilon<float>()))
    {
        localMatrix[0][3] = localMatrix[1][3] = localMatrix[2][3] = 0.0f;
        localMatrix[3][3] = 1.0f;
    }

    translation = glm::vec3(localMatrix[3]);
    localMatrix[3] = glm::vec4(0, 0, 0, localMatrix[3].w);

    glm::vec3 rows[3];

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            rows[i][j] = localMatrix[i][j];
        }
    }

    scale.x = glm::length(rows[0]);
    rows[0] = glm::detail::scale(rows[0], 1.0f);
    scale.y = glm::length(rows[1]);
    rows[1] = glm::detail::scale(rows[1], 1.0f);
    scale.z = glm::length(rows[2]);
    rows[2] = glm::detail::scale(rows[2], 1.0f);

    rotation.y = glm::asin(-rows[0][2]);
    if (glm::cos(rotation.y) != 0.0)
    {
        rotation.x = glm::atan2(rows[1][2], rows[2][2]);
        rotation.z = glm::atan2(rows[0][1], rows[0][0]);
    }
    else
    {
        rotation.x = glm::atan2(-rows[2][0], rows[1][1]);
        rotation.z = 0.0f;
    }

    return true;
}
