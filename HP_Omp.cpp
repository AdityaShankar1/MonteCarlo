#include <iostream>
#include <cstdlib>
#include <cmath>
#include <omp.h>

double monteCarloArea(double a, double b, int numPoints) {
    int pointsInside = 0;

    #pragma omp parallel for reduction(+:pointsInside)
    for (int i = 0; i < numPoints; i++) {
        double x = ((double) rand() / RAND_MAX) * 2 * a - a;
        double y = ((double) rand() / RAND_MAX) * 2 * b - b;
        if ((x*x)/(a*a) + (y*y)/(b*b) <= 1) {
            pointsInside++;
        }
    }

    return (double) pointsInside / numPoints * (4 * a * b);
}

double perimeter(double a, double b) {
    return 2 * M_PI * sqrt((a*a + b*b) / 2);
} 