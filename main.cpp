#include <chrono>
#include <iostream>
#include <math.h>
#include <omp.h>
#include <stdlib.h>

int DIVISIONS = 4;
const int THREADS = 6;
const int SIZE = 5001;
char image[SIZE][SIZE][3];
int COLORS[12][3] = {
    {255, 0, 0},
    {255, 127, 0},
    {255, 255, 0},
    {127, 255, 0},
    {0, 255, 0},
    {0, 255, 127},
    {0, 255, 255},
    {0, 127, 255},
    {0, 0, 255},
    {127, 0, 255},
    {255, 0, 255},
    {255, 0, 127}
};

void drawUlam();
void paintUlamPixel(const int id, const int row, const int col);
void threadCalculations(int startX, int finishX, int startY, int finishY, int id);
int max(int a, int b);
int getValueFromUlam(int row, int col, int size);
bool isPrime(int n);
void paintUlam(int tid, int p, int r);
void saveImageToFile();

int main(int argc, char** argv)
{
    auto timeStart = std::chrono::steady_clock::now();

    drawUlam();
    
    auto timeEnd = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeElapsed = timeEnd - timeStart;
    std::cout << "Czas wykonywania: " << timeElapsed.count() << " s\n";

    saveImageToFile();

    return 0;
}

void drawUlam()
{
    int part = SIZE / DIVISIONS;
    int remainder = (SIZE - part * DIVISIONS);

    #pragma omp parallel num_threads(THREADS)
    {
        #pragma omp for collapse(2) schedule(dynamic)
        for (int x = 0; x < DIVISIONS; x++)
        {
            for (int y = 0; y < DIVISIONS; y++)
            {
                int startX = 0;
                int finishX = 0;
                int startY = 0;
                int finishY = 0;

                if (x == DIVISIONS - 1 && y == DIVISIONS - 1)
                {
                    startX = x * part;
                    finishX = (x + 1) * part + remainder;
                    startY = y * part;
                    finishY = (y + 1) * part + remainder;
                }
                else if (x == DIVISIONS - 1 && y != DIVISIONS - 1)
                {
                    startX = x * part;
                    finishX = (x + 1) * part + remainder;
                    startY = y * part;
                    finishY = (y + 1) * part;
                }
                else if (x != DIVISIONS - 1 && y == DIVISIONS - 1)
                {
                    startX = x * part;
                    finishX = (x + 1) * part;
                    startY = y * part;
                    finishY = (y + 1) * part + remainder;
                }
                else
                {
                    startX = x * part;
                    finishX = (x + 1) * part;
                    startY = y * part;
                    finishY = (y + 1) * part;
                }

                threadCalculations(startX, finishX, startY, finishY, omp_get_thread_num());
            }
        }
    }
}

void paintUlamPixel(const int id, const int row, const int col)
{
    if (isPrime(getValueFromUlam(row, col, SIZE)))
    {
        image[row][col][0] = 0;
        image[row][col][1] = 0;
        image[row][col][2] = 0;
    }
    else
    {
        image[row][col][0] = COLORS[id % 12][0];
        image[row][col][1] = COLORS[id % 12][1];
        image[row][col][2] = COLORS[id % 12][2];
    }
}

void threadCalculations(int startX, int finishX, int startY, int finishY, int id)
{
    for (int row = startX; row < finishX; row++)
        for (int col = startY; col < finishY; col++)
            paintUlamPixel(id, row, col);
}

int max(const int a, int b)
{
    if (a > b) return a;
    else return b;
}

int getValueFromUlam(int row, int col, int size)
{
    row -= (size - 1) / 2;
    col -= size / 2;
    int mx = abs(row);
    int my = abs(col);
    int l = 2 * max(mx, my);
    int d = col >= row ? l * 3 + row + col : l - row - col;
    return pow(l - 1, 2) + d;
}

bool isPrime(int n)
{
    int p;
    for (p = 2; p*p <= n; p++)
        if (n%p == 0) return 0;
    return n > 2;
}

void saveImageToFile()
{
    FILE * fp = fopen("grafika.ppm", "wb");
    fprintf(fp, "P6\n%i %i\n%i ", SIZE, SIZE, 255);
    fwrite(image, 1, SIZE*SIZE*3, fp);
    fclose(fp);
}