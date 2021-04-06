/*
* @Author: gpinchon
* @Date:   2021-03-11 12:23:50
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-21 00:19:29
*/

#include "SphericalHarmonics.hpp"
#include "Tools/Tools.hpp"

#define _USE_MATH_DEFINES // for C++
#include <algorithm>
#include <math.h>

auto Factorial(int x)
{
    static const double factorial_cache[16] = {
        1, 1, 2, 6, 24, 120, 720, 5040,
        40320, 362880, 3628800, 39916800,
        479001600, 6227020800,
        87178291200, 1307674368000
    };

    if (x < 16)
        return factorial_cache[x];
    else {
        double s = factorial_cache[16 - 1];
        for (int n = 16; n <= x; n++)
            s *= n;
        return s;
    }
}

auto DoubleFactorial(int x)
{
    static const double dbl_factorial_cache[16] = {
        1, 1, 2, 3, 8, 15, 48, 105,
        384, 945, 3840, 10395, 46080,
        135135, 645120, 2027025
    };

    if (x < 16)
        return dbl_factorial_cache[x];
    else {
        double s = dbl_factorial_cache[16 - (x % 2 == 0 ? 2 : 1)];
        double n = x;
        while (n >= 16) {
            s *= n;
            n -= 2.0;
        }
        return s;
    }
}

double K(int l, int m)
{
    //normalization constant
    //b == bands
    //l = [ 0..b]
    //m = [-l..l]
    double kml = (2.0 * l + 1) * Factorial(l - abs(m)) / (4.0 * M_PI * Factorial(l + abs(m)));
    return sqrt(kml);
    //double temp{ ((2 * l + 1) * Factorial(l - m)) / (4 * M_PI * Factorial(l + m)) };
    //return sqrt(temp);
}

double LegendrePolynomial(int32_t l, int32_t m, double x)
{
    //Legendre Polynomial at X
    //@ref https://en.wikipedia.org/wiki/Associated_Legendre_polynomials
    double pmm { 1.0 };
    if (m > 0) {
        double sign = (m % 2 == 0 ? 1 : -1);
        pmm = sign * DoubleFactorial(2 * m - 1) * pow(1 - x * x, m / 2.0);
    }
    if (l == m)
        return pmm;

    double pmm1 { x * (2 * m + 1) * pmm };
    if (l == m + 1)
        return pmm1;
    for (int n = m + 2; n <= l; n++) {
        double pmn = (x * (2 * n - 1) * pmm1 - (n + m - 1) * pmm) / (n - m);
        pmm = pmm1;
        pmm1 = pmn;
    }
    return pmm1;
}

double SHCoeff(int32_t l, int32_t m, double theta, double phi)
{
    //theta = [0..PI]
    //phi   = [0..2*PI]
    const double sqrt2 { sqrt(2.0) };
    const double kml = K(l, m);
    if (m > 0) {
        return sqrt(2.0) * kml * cos(m * phi) * LegendrePolynomial(l, m, cos(theta));
    } else if (m < 0) {
        return sqrt(2.0) * kml * sin(-m * phi) * LegendrePolynomial(l, -m, cos(theta));
    } else //m == 0
        return kml * LegendrePolynomial(l, 0, cos(theta));
}

double SHCoeff(int l, int m, const SphericalHarmonics::Sample& sample)
{
    const glm::vec3 N { sample.vec };
    const glm::vec3 N2 { N * N };
    switch (l) {
    case 0:
        return 0.282095;
    case 1:
        switch (m) {
        case -1:
            return -0.488603 * N.y;
        case 0:
            return 0.488603 * N.z;
        case 1:
            return -0.488603 * N.x;
        }
    case 2:
        switch (m) {
        case -2:
            return 1.092548 * N.x * N.y;
        case -1:
            return -1.092548 * N.y * N.z;
        case 0:
            return 0.315392 * (-N2.x - N2.y + 2.0 * N2.z);
        case 1:
            return -1.092548 * N.x * N.z;
        case 2:
            return 0.546274 * (N2.x - N2.y);
        }
    case 3:
        switch (m) {
        case -3:
            return -0.590044 * N.y * (3 * N2.x - N2.y);
        case -2:
            return 2.890611 * N.x * N.y * N.z;
        case -1:
            return -0.457046 * N.y * (4 * N2.z - N2.x - N2.y);
        case 0:
            return 0.373176 * N.z * (2 * N2.z - 3 * N2.x - 3 * N2.y);
        case 1:
            return -0.457046 * N.x * (4 * N2.z - N2.x - N2.y);
        case 2:
            return 1.445306 * N.z * (N2.x - N2.y);
        case 3:
            return -0.590044 * N.x * (N2.x - 3 * N2.y);
        }
    }
    return SHCoeff(l, m, sample.theta, sample.phi);
}

SphericalHarmonics::SphericalHarmonics(uint32_t samples, uint32_t bands)
    : _shSamples(uint64_t(samples) * samples, bands * bands)
    , _bands(bands)
{
    auto i { 0u };
    double samplesRcp { 1 / double(samples) };
    for (auto a { 0u }; a < samples; ++a) {
        for (auto b { 0u }; b < samples; ++b) {
            glm::dvec2 random {
                Tools::Halton23(i + 1) * 2.f - 1.f
            };
            auto x { std::clamp((a + random.x) * samplesRcp, 0.0, 1.0) };
            auto y { std::clamp((b + random.y) * samplesRcp, 0.0, 1.0) };
            auto theta { std::acos(2.0 * x - 1.0) };
            auto phi { 2.0 * M_PI * y };
            auto sinTheta { std::sin(theta) };
            auto cosTheta { std::cos(theta) };
            auto sinPhi { std::sin(phi) };
            auto cosPhi { std::cos(phi) };
            glm::vec3 vec {
                sinTheta * cosPhi,
                sinTheta * sinPhi,
                cosTheta
            };
            auto& sample { _shSamples.at(i) };
            sample.vec = vec;
            sample.theta = theta;
            sample.phi = phi;
            for (auto l { 0 }; l < bands; ++l) {
                for (int m { -l }; m <= l; ++m) {
                    auto index { l * (l + 1) + m };
                    sample.coeff.at(index) = SHCoeff(l, m, sample);
                    //SHCoeff(l, m, theta, phi);
                }
            }
            ++i;
        }
    }
}

const std::vector<SphericalHarmonics::Sample>& SphericalHarmonics::GetSamples() const
{
    return _shSamples;
}

std::vector<float> SphericalHarmonics::ProjectFunction(std::function<float(const SphericalHarmonics::Sample&)> fn) const
{
    std::vector<float> result;
    for (const auto& sample : GetSamples()) {
        result.resize(sample.coeff.size());
        for (auto n = 0u; n < sample.coeff.size(); ++n) {
            result.at(n) += fn(sample) * float(sample.coeff.at(n));
        }
    }
    //weight function is the surface area of a sphere
    const auto weight { 4 * M_PI };
    const auto factor { weight / GetSamples().size() };
    for (auto& coeff : result)
        coeff *= factor;
    return result;
}

std::vector<glm::vec3> SphericalHarmonics::ProjectFunction(std::function<glm::vec3(const SphericalHarmonics::Sample&)> fn) const
{
    std::vector<glm::vec3> result(size_t(_bands) * _bands, glm::vec3(0));
    for (const auto& sample : GetSamples()) {
        const auto color { fn(sample) };
        for (auto n = 0u; n < sample.coeff.size(); ++n) {
            result.at(n) += color * float(sample.coeff.at(n));
        }
    }
    const auto weight { 4 * M_PI };
    const auto factor { weight / GetSamples().size() };
    for (auto& coeff : result)
        coeff *= factor;
    return result;
}
