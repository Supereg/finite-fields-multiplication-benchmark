#include <iostream>
#include <cassert>
#include "poly.h"

using namespace std;

#define assertm(exp, msg) assert(((void)msg, exp))

uint8_t ANTILOG[8] = {0}; // maps from i to (poly.to_base_10() - 1)
uint8_t LOG[8] = {0}; // maps from (poly.to_base_10() - 1) to i

void fillLogAndAntiLogTables() {
    // we hardcode generatorPoly^0
    ANTILOG[0] = 0; // i=0 maps to poly=1
    LOG[0] = 0; // poly=1 maps to i=0

    Poly g = generatorPoly;

    for (int i = 1; i < 8; ++i) { // 0 to 7 because the generator will generate every element (max 9) excluding the zero
        uint8_t key = g.to_base_10() - 1;
        ANTILOG[i] = key;
        LOG[key] = i;

        g = g.multiply(generatorPoly);
    }
}

[[nodiscard]] uint8_t Poly::to_base_10() const {
    return 1*this->coeff_0 + 3*this->coeff_1 + 9*this->coeff_2;
}

string Poly::toString() const {
    string base;
    if (coeff_2 > 0) {
        base += (coeff_2 > 1? to_string(coeff_2): "") + "x^2";
    }
    if (coeff_1 > 0) {
        if (!base.empty()) {
            base += "+";
        }
        base += (coeff_1 > 1? to_string(coeff_1): "") + "x";
    }
    if (coeff_0 > 0) {
        if (!base.empty()) {
            base += "+";
        }
        base += to_string(coeff_0);
    }
    return base;
}

Poly Poly::add(Poly poly) const {
    assertm(this->coeff_2 == 0, "this Poly isn't properly reduced!");
    assertm(poly.coeff_2 == 0, "poly isn't properly reduced!");

    int8_t new_coeff_0 = (this->coeff_0 + poly.coeff_0) % P;
    int8_t new_coeff_1 = (this->coeff_1 + poly.coeff_1) % P;

    Poly new_poly = {
            .coeff_1 = new_coeff_1,
            .coeff_0 = new_coeff_0,
    };
    return new_poly;
}

Poly Poly::multiply(Poly poly) const {
    assertm(this->coeff_2 == 0, "this Poly isn't properly reduced!");
    assertm(poly.coeff_2 == 0, "poly isn't properly reduced!");

    int8_t new_coeff_2 = (this->coeff_1 * poly.coeff_1) % P;
    int8_t new_coeff_1 = (this->coeff_1 * poly.coeff_0 + this->coeff_0 * poly.coeff_1) % P;
    int8_t new_coeff_0 = (this->coeff_0 * poly.coeff_0) % P;

    Poly new_poly = {
            .coeff_2 = new_coeff_2,
            .coeff_1 = new_coeff_1,
            .coeff_0 = new_coeff_0,
    };

    while (new_poly.coeff_2 > 0) {
        // reduction Polynom is x^2 + 1
        new_poly.coeff_2 -= reductionPoly.coeff_2;
        new_poly.coeff_1 -= reductionPoly.coeff_1;
        new_poly.coeff_0 -= reductionPoly.coeff_0;
    }
    assertm(new_poly.coeff_2 == 0, "coeff_2 was found to not be zero after reduction!");

    // after we reduction we may have negative coefficients, so we may need to correct that
    if (new_poly.coeff_1 < 0) {
        new_poly.coeff_1 = reverseElement(new_poly.coeff_1);
    }
    if (new_poly.coeff_0 < 0) {
        new_poly.coeff_0 = reverseElement(new_poly.coeff_0);
    }

    return new_poly;
}

[[nodiscard]] Poly Poly::multiply_table(Poly poly) const {
    assertm(this->coeff_2 == 0, "this Poly isn't properly reduced!");
    assertm(poly.coeff_2 == 0, "poly isn't properly reduced!");

    uint8_t logA = LOG[this->to_base_10() - 1];
    uint8_t logB = LOG[poly.to_base_10() - 1];

    return from_base_10(ANTILOG[logA + logB] + 1);
}

void benchmarkMultiplicationTable() {
    Poly table[Q][Q];

    long long int totalTime = 0;

    for (int it = 0; it < 1000000; it++) {
        for (int i = 0; i < Q; ++i) {
            for (int j = 0; j < Q; ++j) {
                Poly a = from_base_10(i);
                Poly b = from_base_10(j);

                Poly result;
                auto begin = chrono::high_resolution_clock::now();
                result = a.multiply(b);
                auto end = chrono::high_resolution_clock::now();

                long long int period = chrono::duration_cast<chrono::nanoseconds>(end-begin).count();
                table[i][j] = result;

                totalTime += period;
            }
        }
    }

    long long int totalTime_table = 0;

    for (int it = 0; it < 1000000; it++) {
        for (int i = 0; i < Q; ++i) {
            for (int j = 0; j < Q; ++j) {
                Poly a = from_base_10(i);
                Poly b = from_base_10(j);

                Poly result;
                auto begin = chrono::high_resolution_clock::now();
                result = a.multiply_table(b);
                auto end = chrono::high_resolution_clock::now();

                long long int period = chrono::duration_cast<chrono::nanoseconds>(end-begin).count();
                table[i][j] = result;

                totalTime_table += period;
            }
        }
    }

    cout << "Total time for multiplication: " + to_string(totalTime) + "ns" << endl;
    cout << "Average time for multiplication: " + to_string(totalTime / (81* 1000000)) + "ns" << endl;
    cout << "Total time for multiplication with log and antilog: " + to_string(totalTime_table) + "ns" << endl;
    cout << "Average time for multiplication with log and antilog: " + to_string(totalTime_table / (81* 1000000)) + "ns" << endl;
}

int main() {
    fillLogAndAntiLogTables();

    /*
    Poly poly0 = {
            .coeff_1 =  1,
            .coeff_0 = 2,
    };
    Poly poly1 = {
            .coeff_1 = 1,
            .coeff_0 = 1,
    };

    Poly result = poly0.add(poly1);
    Poly mulResult = poly0.multiply(poly1);
    Poly mulTableResult = poly0.multiply_table(poly1);

    cout << poly0.toString() + " + " + poly1.toString() + " = " + result.toString() << endl;
    cout << poly0.toString() + " * " + poly1.toString() + " = " + mulResult.toString() << endl;
    cout << poly0.toString() + " * " + poly1.toString() + " = " + mulTableResult.toString() << endl;
    cout << reductionPoly.toString() << endl;
     */

    benchmarkMultiplicationTable();
    /**
     * Benchmark prints something like the following.
     * Using antilog and log tables for multiplication can result in a 14%-22% (avg 17.8%) performance increase when using -O3
     *
     * Total time for multiplication: 4476309036ns
     * Average time for multiplication: 55ns
     * Total time for multiplication with log and antilog: 3711718152ns
     * Average time for multiplication with log and antilog: 45ns
     */
    return 0;
}
