#ifndef VECTOR_REDACTOR_MATRIX3F_HPP
#define VECTOR_REDACTOR_MATRIX3F_HPP

#include "Vec2F.hpp"

#include <array>
#include <cmath>
#include <cstddef>

// Матрица для двумерных аффинных преобразований в однородных координатах.
// Векторы считаются столбцами: p' = M * p.
class Matrix3F
{
    static constexpr std::size_t DIMENSION       = 3;
    static constexpr float       DEFAULT_EPSILON = 1e-7F;

public:
    using Storage = std::array<std::array<float, DIMENSION>, DIMENSION>;

    constexpr Matrix3F() : values_{Storage{{{1.0F, 0.0F, 0.0F}, {0.0F, 1.0F, 0.0F}, {0.0F, 0.0F, 1.0F}}}}

    {
    }

    constexpr explicit Matrix3F(const Storage& values) : values_(values)
    {
    }

    constexpr std::array<float, DIMENSION>& operator[](std::size_t row)
    {
        return values_[row];
    }

    constexpr const std::array<float, DIMENSION>& operator[](std::size_t row) const
    {
        return values_[row];
    }

    static constexpr Matrix3F identity()
    {
        return Matrix3F{};
    }

    static constexpr Matrix3F translation(float translation_x, float translation_y)
    {
        return Matrix3F{Storage{{{1.0F, 0.0F, translation_x}, {0.0F, 1.0F, translation_y}, {0.0F, 0.0F, 1.0F}}}};
    }

    static constexpr Matrix3F scaling(float scale_x, float scale_y)
    {
        return Matrix3F{Storage{{{scale_x, 0.0F, 0.0F}, {0.0F, scale_y, 0.0F}, {0.0F, 0.0F, 1.0F}}}};
    }

    static Matrix3F rotation(float radians)
    {
        const float cose = std::cos(radians);
        const float sine = std::sin(radians);

        return Matrix3F{Storage{{{cose, -sine, 0.0F}, {sine, cose, 0.0F}, {0.0F, 0.0F, 1.0F}}}};
    }

    static constexpr Matrix3F shear(float x_by_y, float y_by_x)
    {
        return Matrix3F{Storage{{{1.0F, x_by_y, 0.0F}, {y_by_x, 1.0F, 0.0F}, {0.0F, 0.0F, 1.0F}}}};
    }

    constexpr Matrix3F operator+(const Matrix3F& rhs) const
    {
        Matrix3F result = *this;
        result += rhs;
        return result;
    }

    constexpr Matrix3F operator-(const Matrix3F& rhs) const
    {
        Matrix3F result = *this;
        result -= rhs;
        return result;
    }

    constexpr Matrix3F operator*(float scalar) const
    {
        Matrix3F result = *this;
        result *= scalar;
        return result;
    }

    constexpr Matrix3F operator*(const Matrix3F& rhs) const
    {
        Matrix3F result{Storage{}};
        for (std::size_t row = 0; row < DIMENSION; ++row)
        {
            for (std::size_t column = 0; column < DIMENSION; ++column)
            {
                for (std::size_t k = 0; k < DIMENSION; ++k)
                {
                    result[row][column] += (*this)[row][k] * rhs[k][column];
                }
            }
        }
        return result;
    }

    constexpr Matrix3F& operator+=(const Matrix3F& rhs)
    {
        for (std::size_t row = 0; row < DIMENSION; ++row)
        {
            for (std::size_t column = 0; column < DIMENSION; ++column)
            {
                values_[row][column] += rhs.values_[row][column];
            }
        }
        return *this;
    }

    constexpr Matrix3F& operator-=(const Matrix3F& rhs)
    {
        for (std::size_t row = 0; row < DIMENSION; ++row)
        {
            for (std::size_t column = 0; column < DIMENSION; ++column)
            {
                values_[row][column] -= rhs.values_[row][column];
            }
        }
        return *this;
    }

    constexpr Matrix3F& operator*=(float scalar)
    {
        for (std::size_t row = 0; row < DIMENSION; ++row)
        {
            for (std::size_t column = 0; column < DIMENSION; ++column)
            {
                values_[row][column] *= scalar;
            }
        }
        return *this;
    }

    constexpr Matrix3F& operator*=(const Matrix3F& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    constexpr bool operator==(const Matrix3F& rhs) const
    {
        return values_ == rhs.values_;
    }

    constexpr bool operator!=(const Matrix3F& rhs) const
    {
        return !(*this == rhs);
    }

    Vec2F transformPoint(const Vec2F& point) const
    {
        const float transformed_x = ((*this)[0][0] * point.x) + ((*this)[0][1] * point.y) + (*this)[0][2];
        const float transformed_y = ((*this)[1][0] * point.x) + ((*this)[1][1] * point.y) + (*this)[1][2];
        const float homogeneous_w = ((*this)[2][0] * point.x) + ((*this)[2][1] * point.y) + (*this)[2][2];

        if (std::abs(homogeneous_w) <= DEFAULT_EPSILON)
        {
            return {transformed_x, transformed_y};
        }

        return {transformed_x / homogeneous_w, transformed_y / homogeneous_w};
    }

    constexpr Matrix3F transposed() const
    {
        Matrix3F result{Storage{}};
        for (std::size_t row = 0; row < DIMENSION; ++row)
        {
            for (std::size_t column = 0; column < DIMENSION; ++column)
            {
                result.values_[row][column] = values_[column][row];
            }
        }
        return result;
    }

    constexpr float determinant() const
    {
        return ((*this)[0][0] * (((*this)[1][1] * (*this)[2][2]) - ((*this)[1][2] * (*this)[2][1]))) -
               ((*this)[0][1] * (((*this)[1][0] * (*this)[2][2]) - ((*this)[1][2] * (*this)[2][0]))) +
               ((*this)[0][2] * (((*this)[1][0] * (*this)[2][1]) - ((*this)[1][1] * (*this)[2][0])));
    }

    bool tryInverse(Matrix3F& result, float epsilon = DEFAULT_EPSILON) const
    {
        const float det = determinant();
        if (std::abs(det) <= epsilon)
        {
            return false;
        }

        const float inv = 1.0F / det;
        result          = Matrix3F{};
        result[0][0]    = (((*this)[1][1] * (*this)[2][2]) - ((*this)[1][2] * (*this)[2][1])) * inv;
        result[0][1]    = (((*this)[0][2] * (*this)[2][1]) - ((*this)[0][1] * (*this)[2][2])) * inv;
        result[0][2]    = (((*this)[0][1] * (*this)[1][2]) - ((*this)[0][2] * (*this)[1][1])) * inv;
        result[1][0]    = (((*this)[1][2] * (*this)[2][0]) - ((*this)[1][0] * (*this)[2][2])) * inv;
        result[1][1]    = (((*this)[0][0] * (*this)[2][2]) - ((*this)[0][2] * (*this)[2][0])) * inv;
        result[1][2]    = (((*this)[0][2] * (*this)[1][0]) - ((*this)[0][0] * (*this)[1][2])) * inv;
        result[2][0]    = (((*this)[1][0] * (*this)[2][1]) - ((*this)[1][1] * (*this)[2][0])) * inv;
        result[2][1]    = (((*this)[0][1] * (*this)[2][0]) - ((*this)[0][0] * (*this)[2][1])) * inv;
        result[2][2]    = (((*this)[0][0] * (*this)[1][1]) - ((*this)[0][1] * (*this)[1][0])) * inv;
        return true;
    }

private:
    Storage values_;
};

constexpr Matrix3F operator*(float scalar, const Matrix3F& matrix)
{
    return matrix * scalar;
}

#endif
