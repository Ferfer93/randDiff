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
  string saveFile ( "corr.out");
  ifstream data(argv[1]);
  ofstream corr(saveFile.c_str());
  int nLinesPe = nLines/100;

  vector<double> positions;
  vector<double> times;
  vector<double> avgsums;
  vector<double> varsums;
  vector<int> avgnum;
  vector<int> varnum;

  //default bin width is 1
  double binWidth = 0.8;
  double maxTimeCorr = 1000;
  int maxDiffChecks = 1000;
  int binTotalNum = (int)(maxTimeCorr/binWidth+0.5);
  int dataSize;

  cout << "Init" << endl;
  for (int i = 0; i < binTotalNum; i++) {
    avgsums.push_back(0);
    varsums.push_back(0);
    avgnum.push_back(0);
    varnum.push_back(0);
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

  dataSize = positions.size();
  int dataSizePe = dataSize/100;

  cout << endl << "Calculating" << endl;
  for (int i = 0; i < dataSize-1; i++) {
    for (int j = i; j < dataSize; j++) {
      double timeDif = times[j] - times[i];
      if (timeDif > maxTimeCorr) break;
      int bin = (int)(timeDif/binWidth + 0.5);
      if (bin == 0) continue;
      avgsums[bin] += abs(positions[j]-positions[i]);
      varsums[bin] += (positions[j]-positions[i])*(positions[j]-positions[i]);
      avgnum[bin]++;
      varnum[bin]++;
    }
    if (i%dataSizePe == 0) {
      cout.flush();
      cout << "\r" << (i*100)/dataSize << "%";
    }
  }

  cout << endl << "Printing to file " << saveFile << endl;
  for (int i = 0; i < binTotalNum; i++) {
    corr << i*binWidth << "\t" << avgsums[i]/avgnum[i] << endl; //"\t" << varsums[i]/varnum[i]-(avgsums[i]/avgnum[i])*(avgsums[i]/avgnum[i]) << endl;
  }

  return 0;
}
