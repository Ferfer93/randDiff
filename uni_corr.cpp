#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

using namespace std;

int nLines = 0;
static int isword (unsigned char c) {
    return isalpha (c);
}

//From the gnu/wc src
int getword (FILE *fp) {
    int c;
    int word = 0;
    while ((c = getc (fp)) != EOF)
    {
        if (isword (c))
        {
            break;
        }
        if ((c) == '\n') \
        nLines++;
    }
    for (; c != EOF; c = getc (fp))
    {
        if ((c) == '\n') \
        nLines++;
        if (!isword (c))
        break;
    }
    return c != EOF;
}

int main(int argc, char **argv) {
    //Count number of lines (copied from wc.c)
    FILE *fp = fopen (argv[1], "r");
    while (getword (fp))
    ;
    fclose (fp);

    string readLine;
    string saveFile ("uni_corr.out");
    ifstream data(argv[1]);
    ofstream corr(saveFile.c_str());
    int nLinesPe = nLines/100;

    vector<double> positions;
    vector<double> times;
    vector<double> postimes;
    vector<double> poscounter;


    //default bin width is 1
    double deltaT = 1000;
    double maxTimeCorr = 50000000;
    int maxDiffChecks = 1000;
    int binTotalNum = (int)(maxTimeCorr/deltaT+0.5);
    int dataSize;

    for (int i = 0; i < binTotalNum; i++) {
        postimes.push_back(0);
        poscounter.push_back(0);
    }

    //Load
    cout << "Loading file" << endl;
    int lineCounter = 0;
    while (getline (data,readLine)) {
        stringstream streamLine(readLine);
        double pos,time;
        streamLine >> pos >> time;
        positions.push_back(pos);
        times.push_back(time);
        if (lineCounter%nLinesPe == 0) {
            cout.flush();
            cout << "\r" << (lineCounter*100)/nLines << "%";
        }
        lineCounter++;
        //if (time > maxTimeCorr*2) break;
    }



    double timeReset = 0;
    double positionReference = 0;
    for (int i = 1; i < lineCounter-1; i++) {
        double tim = times[i]-timeReset;
        while (tim > maxTimeCorr) {
            timeReset += maxTimeCorr;
            tim -= maxTimeCorr;
            positionReference = positions[i-1];
        }
        double pos = positions[i]-positionReference;
        double tim2 = times[i+1]-timeReset;
        int timeIndex = (int)(tim/deltaT+0.5);
        int timeIndex2 = (int)(tim2/deltaT+0.5);
        if (timeIndex2 > binTotalNum-1) timeIndex2 = binTotalNum-1;
        for (int j = timeIndex; j < timeIndex2; j++) {
            postimes[j] += pos*pos; //change to squares
            poscounter[j] += 1;
        }
    }

    for (int i=1; i < binTotalNum-1; i++) {
        if (poscounter[i] != 0)
        postimes[i] /= poscounter[i];
        else {
            cout << "Empty position counter, no average" << endl;
            postimes[i] = 0;
        }
    }
    postimes[0] = 0;
    postimes[binTotalNum-1] = postimes[binTotalNum-2];

    cout << endl << "Printing to file " << saveFile << endl;
    for (int i = 0; i < binTotalNum; i++) {
        corr << i*deltaT << "\t" << postimes[i] << endl; //"\t" << varsums[i]/varnum[i]-(avgsums[i]/avgnum[i])*(avgsums[i]/avgnum[i]) << endl;
    }

    return 0;
}
