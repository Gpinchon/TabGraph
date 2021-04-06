/*
* @Author: gpinchon
* @Date:   2021-03-11 12:22:36
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-11 12:28:04
*/
#pragma once

#include <vector>
#include <functional>
#include <glm/glm.hpp>

class SphericalHarmonics {
public:
    struct Sample {
        Sample(size_t coeffNbr)
            : coeff(coeffNbr, 0)
        {
        }
        /**
        * sph.x == theta[0..PI]
        * sph.y == phi[0..2*PI]
        * r is assumed to be 1
        */
        float theta{ 0 }, phi{ 0 };
        glm::vec3 vec{ 0 };
        std::vector<double> coeff{ 0 };
    };
    /**
     * @brief Constructs a Spherical Harmonics with specified samples and bands
     * @param samples : the number of samples in latitude & longitude, total samples will be N * N
     * @param bands : the number of bands in the final Spherical Harmonics, default is 4
    */
    SphericalHarmonics(uint32_t samples, uint32_t bands = 4);
    const std::vector<SphericalHarmonics::Sample>& GetSamples() const;
    std::vector<float> ProjectFunction(std::function<float(const SphericalHarmonics::Sample&)>) const;
    std::vector<glm::vec3> ProjectFunction(std::function<glm::vec3(const SphericalHarmonics::Sample&)>) const;

private:
    std::vector<SphericalHarmonics::Sample> _shSamples;
    const uint8_t                           _bands{ 0 };
};

int factorial(int x);
double K(int l, int m);
double P(int32_t l, int32_t m, double x);
double SHCoeff(int32_t l, int32_t m, double theta, double phi);