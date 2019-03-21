#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <iostream>
#include <fstream>


int shortestSeekTimeFirst(std::vector<int> &queue, int curHeadCylinder);
double expoSeekTime(int xDiff);
double relSeekTime(int xDiff);

/* Simple HDD simulation of the Quantum Atlas III
 * xmax = 8057 Cylinders
 * C = 9.1GB        N = 7200 rpm
 * x* = 1686        t = 1.5455 ms
 * c = 0.3187 ms    r = 0.3868
 */

int main()
{
    //Output to csv to make data easier to manage
    std::ofstream csvFile;
    csvFile.open("avgstat.csv");

    //Random number generator with current time as seed
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> distribution(1, 8057);

    for (int i = 1; i <= 20; i++)
    {
        //Initialize current r/w head position
        int curCylinderPosition = distribution(generator);

        //Insert random vals to queue of a certain size
        int queueSize = i;
        std::vector<int> queue;
        for (int x = 0; x < queueSize; x++)
        {
            queue.push_back(distribution(generator));
        }
        std::cout << "Queue Size: " << queueSize << std::endl;

        //Simulation of I/o head
        int nOps = 100000;
        int seekDistance, seekDistanceSum = 0;
        double seekTime, seekTimeSum = 0.0;

        for (int x = 0; x < nOps; x++)
        {
            //Get closet cylinder request from queue
            int queueIndex = shortestSeekTimeFirst(queue, curCylinderPosition);
            int cylnderToMoveTo = queue.at(queueIndex);

            //Get seek time 
            seekDistance = abs(cylnderToMoveTo - curCylinderPosition);
            seekTime = relSeekTime(seekDistance);

            //Move head to new postion
            curCylinderPosition = cylnderToMoveTo;

            //Sum for averages
            seekDistanceSum = seekDistanceSum + seekDistance;
            seekTimeSum = seekTimeSum + seekTime;

            //Erase completed request from queue and add new request
            queue.at(queueIndex) = distribution(generator);
        }

        //Output to screen and file
        std::cout << "Average seek time = " << seekTimeSum / nOps << std::endl;
        std::cout << "Average seek dist = " << seekDistanceSum / nOps << std::endl;
        csvFile << seekTimeSum / nOps << "," << seekDistanceSum / nOps << std::endl;
    }
    csvFile.close();
    return 0;
}

/* Finds the request in the queue closest to the given head position.
 * O(N) algo since it has to go through whole queue.
 */
int shortestSeekTimeFirst(std::vector<int> &queue, int curCylinderPosition)
{
    double curmin = abs(queue.at(0) - curCylinderPosition);
    int cyl = 0;
    int sstfCyl = 0;

    for (cyl = 1; cyl < queue.size(); cyl++)
    {
        int dif = abs(queue.at(cyl) - curCylinderPosition);
        if (dif < curmin)
        {
            curmin = dif;
            sstfCyl = cyl;
        }
    }

    return sstfCyl;
}

/* Returns the seektime based on Expo model described in class.
 * Gets the seektime based on the amount of cylinders the head has to move over.
 */
double expoSeekTime(int xDiff)
{
    if (xDiff == 0)
        return 0.0;

    double t = 1.5455, c = 0.3187, r = 0.3868;
    int xS = 1686;

    if (xDiff >= xS)
    {
        return (c * r * (xDiff - xS)) / pow((xS - 1), (1 - r)) + t + c * pow((xS - 1), r);
    }
    else
    {
        return t + c * pow((xDiff - 1), r);
    }
}

double relSeekTime(int xDiff)
{
    return sqrt(xDiff / 8058.0);
}
