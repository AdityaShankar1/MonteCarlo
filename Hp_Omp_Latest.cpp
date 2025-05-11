// I've added time to show the power of parallelism using speedup

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <omp.h>

double monteCarloAreaSerial(double a, double b, int numPoints) {
    int pointsInside = 0;

    for (int i = 0; i < numPoints; i++) {
        double x = ((double) rand() / RAND_MAX) * 2 * a - a;
        double y = ((double) rand() / RAND_MAX) * 2 * b - b;
        if ((x*x)/(a*a) + (y*y)/(b*b) <= 1) {
            pointsInside++;
        }
    }

    return (double) pointsInside / numPoints * (4 * a * b);
}

double monteCarloAreaParallel(double a, double b, int numPoints) {
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

int main() {
    double a = 3.0; // Semi-major axis
    double b = 2.0; // Semi-minor axis
    int numPoints = 100000000; // 100 million points for better accuracy

    // Seed random number generator
    srand(time(0));

    // SERIAL execution
    double startSerial = omp_get_wtime();
    double areaSerial = monteCarloAreaSerial(a, b, numPoints);
    double endSerial = omp_get_wtime();
    double timeSerial = endSerial - startSerial;

    // PARALLEL execution
    double startParallel = omp_get_wtime();
    double areaParallel = monteCarloAreaParallel(a, b, numPoints);
    double endParallel = omp_get_wtime();
    double timeParallel = endParallel - startParallel;

    // SPEEDUP calculation
    double speedup = timeSerial / timeParallel;

    std::cout << "Estimated Area (Serial):   " << areaSerial << "\n";
    std::cout << "Estimated Area (Parallel): " << areaParallel << "\n";
    std::cout << "Serial Time:   " << timeSerial << " seconds\n";
    std::cout << "Parallel Time: " << timeParallel << " seconds\n";
    std::cout << "Speedup:       " << speedup << "x\n";

    return 0;
}
