//
// Created by Andi on 16.11.20.
//

#ifndef HW1_POLY_H
#define HW1_POLY_H

#include <iostream>

#define P 3
#define N 2
#define Q 9 // q=p^n

// finit field p=3 n=2 with q elements => F={0, 1, 2, x, x+1, x+2, 2x, 2x+1, 2x+2}

int8_t reverseElement(int8_t element) {
    // As we work on the fixed finit field F_9 we can hardcode the reverse elements
    if (element == 0) {
        return 0;
    } else if (element == -1) {
        return 2;
    } else if (element == -2) {
        return 1;
    } else {
        throw std::invalid_argument("Could not determine reverse element for " + std::to_string(element));
    }
}


struct Poly {
    int8_t coeff_2 = 0; // x^2 * coeff_2
    int8_t coeff_1 = 0; // x^1 * coeff_1
    int8_t coeff_0 = 0; // x^0 * coeff_0

    [[nodiscard]] uint8_t to_base_10() const;

    [[nodiscard]] std::string toString() const;

    [[nodiscard]] Poly add(Poly poly) const;

    [[nodiscard]] Poly multiply(Poly poly) const;
    [[nodiscard]] Poly multiply_table(Poly poly) const;

};

Poly from_base_10(uint8_t value) {
    uint8_t next = value / 3; // 2
    int8_t coeff_0 = value % 3;
    int8_t coeff_1 = next % 3;

    return {
        .coeff_1 = coeff_1,
        .coeff_0 = coeff_0,
    };
}

Poly reductionPoly = {
        .coeff_2 = 1,
        .coeff_0 = 1,
};

Poly generatorPoly = {
        .coeff_1 = 1,
        .coeff_0 = 1,
};

#endif //HW1_POLY_H
