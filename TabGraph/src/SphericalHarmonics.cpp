/*
* @Author: gpinchon
* @Date:   2021-03-11 12:23:50
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-11 13:03:42
*/

#include "SphericalHarmonics.hpp"
#include "Tools/Tools.hpp"

#define _USE_MATH_DEFINES // for C++
#include <algorithm>
#include <math.h>

int factorial(int x) {
    int res{ 1 };
    while (x > 1) {
        res *= x;
        --x;
    }
    return res;
}

double K(int l, int m) {
    //normalization constant
    //b == bands
    //l = [ 0..b]
    //m = [-l..l]
    //for l == 1 && m == -1
    //((2 * 1 + 1) * fact(1 - (-1)) / (4 * PI * fact(1 + (-1)))
    //(3 * fact(2)) / (4 * 3.14 * fact(0))
    //(3 * 2) / (12,56 * 1)
    //6 / 12.56
    //0.47
    double temp{ ((2 * l + 1) * factorial(l - m)) / (4 * M_PI * factorial(l + m)) };
    return sqrt(temp);
}

double P(int32_t l, int32_t m, double x) {
    //Legendre Polynomial at X
    //@ref https://en.wikipedia.org/wiki/Associated_Legendre_polynomials
    double pmm{ 1.0 };
    if (m > 0) {
        double somx2{ sqrt((1 - x) * (1 + x)) };
        double fact{ 1.0 };
        for (int i{ 1 }; i <= m; ++i) {
            pmm *= -fact * somx2;
            fact += 2;
        }
    }
    if (l == m)
        return pmm;
    double pmmp1{ x * (2 * m + 1) * pmm };
    if (l == m + 1)
        return pmmp1;
    double pll{ 0 };
    for (int ll{ m + 2 }; ll <= l; ++ll) {
        pll = ((2 * ll - 1) * x * pmmp1 - (ll + m - 1) * pmm) / (ll - m);
        pmm = pmmp1;
        pmmp1 = pll;
    }
    return pll;
}

double SHCoeff(int32_t l, int32_t m, double theta, double phi) {
    //theta = [0..PI]
    //phi   = [0..2*PI]
    const double sqrt2{ sqrt(2.0) };
    if (m == 0)
        return K(l, 0) * P(l, 0, cos(theta));
    else if (m > 0)
        return sqrt2 * K(l, m) * cos(m * phi) * P(l, m, cos(theta));
    else
        return sqrt2 * K(l, -m) * sin(-m * phi) * P(l, -m, cos(theta));
}

SphericalHarmonics::SphericalHarmonics(uint32_t samples, uint32_t bands)
    : _shSamples(uint64_t(samples) * samples, bands * bands)
{
    auto i{ 0u };
    double samplesRcp{ 1 / double(samples) };
    for (auto a{ 0u }; a < samples; ++a) {
        for (auto b{ 0u }; b < samples; ++b) {
            glm::dvec2 random{
                Tools::Halton23(i + 1) * 2.f - 1.f
            };
            auto x{ std::clamp((a + random.x) * samplesRcp, 0.0, 1.0) };
            auto y{ std::clamp((b + random.y) * samplesRcp, 0.0, 1.0) };
            auto theta{ 2.0 * std::acos(std::sqrt(1.0 - x)) };
            auto phi{ 2.0 * M_PI * y };
            auto sinTheta{ std::sin(theta) };
            auto cosTheta{ std::cos(theta) };
            auto sinPhi{ std::sin(phi) };
            auto cosPhi{ std::cos(phi) };
            glm::vec3 vec{
                sinTheta * cosPhi,
                sinTheta * sinPhi,
                cosTheta
            };
            _shSamples.at(i).vec = vec;
            _shSamples.at(i).sph = glm::vec2(theta, phi);
            for (auto l{ 0 }; l < bands; ++l) {
                for (int m{ -l }; m <= l; ++m) {
                    auto index{ l * (l + 1) + m };
                    _shSamples.at(i).coeff.at(index) = SHCoeff(l, m, theta, phi);
                }
            }
            ++i;
        }
    }
    _shCartesian = ProjectCartesian(_shSamples);
}

const std::vector<SphericalHarmonics::Sample>& SphericalHarmonics::GetSamples() const
{
    return _shSamples;
}

const std::vector<glm::vec3>& SphericalHarmonics::GetCartesianCoeffs() const
{
    return _shCartesian;
}

std::vector<float> SphericalHarmonics::ProjectFunction(std::function<float(const SphericalHarmonics::Sample&)> fn)
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

std::vector<glm::vec3> SphericalHarmonics::ProjectFunction(std::function<glm::vec3(const SphericalHarmonics::Sample&)> fn)
{
    std::vector<glm::vec3> result;
    for (const auto& sample : GetSamples()) {
        result.resize(sample.coeff.size());
        for (auto n = 0u; n < sample.coeff.size(); ++n) {
            result.at(n) += fn(sample) * float(sample.coeff.at(n));
        }
    }
    const auto weight{ 4 * M_PI };
    const auto factor{ weight / GetSamples().size() };
    for (auto& coeff : result)
        coeff *= factor;
    return result;
}

std::vector<glm::vec3> SphericalHarmonics::ProjectCartesian(const std::vector<SphericalHarmonics::Sample>& samples)
{
    std::vector<glm::vec3> result;
    for (const auto& sample : samples) {
        result.resize(sample.coeff.size());
        for (auto n = 0u; n < sample.coeff.size(); ++n) {
            result.at(n) += sample.vec * float(sample.coeff.at(n));
        }

    }
    //TODO find why weight is 2PI here...
    const auto weight{ 2 * M_PI };
    const auto factor{ weight / float(samples.size()) };
    for (auto& coeff : result)
        coeff *= factor;
    return result;
}
