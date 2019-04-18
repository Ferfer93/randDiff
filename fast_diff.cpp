#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <random>
#include <vector>

using namespace std;

#define FIXED_FLOAT(x) std::fixed <<std::setprecision(2)<<(x)

double inv_exp(double, double);

int coin(double);

int main(int argc, char **argv) {

    /*
    Input:
    tJ,     number of jumps
    wN,     every wN, data is written to file
    Lambda, spin rate temperature
    T,      temperature of the particle
    Tg,     temperature of the distr. of traps
    G0,     bare scaling of the average scape rate
    */

    ofstream outFile;

    if (argc != 9) {
		printf("Wrong number of arguments;:\n");
		printf("1. Number of jumps\n2. Output data every N jumps\n"
        "3. Lambda, spin change rate\n4. Temperature\n"
        "5. Temperature of the trap distribution\n"
        "6. G0 Microscopic frequency scale\n"
        "7. Persistence flag (0: no persistence)\n"
        "8. Predefined lattice flag (0: generate on the fly)\n");
		return -1;
	}

    double lambda = atof(argv[3]);
    double T = atof(argv[4]);
    double Tg = atof(argv[5]);
    double G0 = atof(argv[6]);

    random_device rd;
    mt19937 mt(rd());
    uniform_real_distribution<double> dist(0.0,1.0);

    long int totalJumps = atoi(argv[1]);
    long int skipOutput = atoi(argv[2]);
    long int totalJumpsPe = totalJumps/100;
    int NOPERS_FLAG = 1 - atoi(argv[7]);
    int PREDEF_LATT = atoi(argv[8]);

    int initial_spin = 0;

    int flips = 0;

    double total_time = 0.0;

    vector<double> flip_times;
    flip_times.push_back(0.0);
    vector<double> jump_times;
    jump_times.push_back(0.0);
    vector<int> positions;
    positions.push_back(0);

    //Initial 10 flip times
    for (int i = 0; i < 10; i++) {
        flip_times.push_back(flip_times[flip_times.size()-1]+inv_exp(dist(mt),lambda));
    }
    flips = 10;

    //Initialize lattice energies
    vector<double> energies;
    if (PREDEF_LATT) {
        cout << "Generating lattice traps" << endl;
        for (long int i=0; i < totalJumps*2L; i++) {
            if (i%(totalJumpsPe*2L) == 0) {
              cout.flush();
              cout << "\r" << (i*100)/(2L*totalJumps) << "%";
            }
            energies.push_back(inv_exp(dist(mt),1.0/Tg));
        }
        cout << endl;
    }

    //Start calculating jump times and change position
    //If the last flip time is less than the jump_time, add 1 flip times
    //Due to this sequential adding, start counting total number of flips from the end.
    cout << "Sim started:" << endl;
    for (long int t = 0; t < totalJumps; t++) {
        if (t%totalJumpsPe == 0) {
          cout.flush();
          cout << "\r" << (t*100)/totalJumps << "%";
        }
        double w = 0;
        if (PREDEF_LATT)
            w = G0*exp(-energies[positions[positions.size()-1]+totalJumps/2]/T);
        else
            w = G0*exp(-inv_exp(dist(mt),1.0/Tg)/T); //Set this to a constant for standard diffusion.
        double jTime = inv_exp(dist(mt),w);
        jump_times.push_back(jump_times[jump_times.size()-1]+jTime);
        total_time += jTime;

        int dir = 0;
        if (!NOPERS_FLAG) {
            while (total_time > flip_times[flip_times.size()-1]) {
                flip_times.push_back(flip_times[flip_times.size()-1]+inv_exp(dist(mt),lambda));
                flips++;
            }

            int it = flip_times.size()-2;
            while (!(total_time < flip_times[it+1] && total_time > flip_times[it])) it--; //it now has direction value

            dir = 2*(it % 2)-1;
        }
        else {
            dir = 2*coin(dist(mt))-1;
        }

        positions.push_back(positions[positions.size()-1]+dir);

    }

    outFile.open("times.out", ios::out);

    int pSize = positions.size();
    int pSizePe = pSize/100;
    cout << endl << "Writing data" << endl;
    for (int i = 0; i < pSize; i++) {
        if (i%pSizePe == 0) {
          cout.flush();
          cout << "\r" << i/pSizePe << "%";
        }
        if (i%skipOutput == 0)
        outFile << positions[i] << "\t" << FIXED_FLOAT(jump_times[i]) << endl;
    }
    cout << endl;
    outFile.close();

    return 0;
}

double inv_exp(double r, double s) {
    return -log(r)/s;
}

int coin(double r) {
    if (r<0.5)
        return 0;
    else if (r>0.5)
        return 1;
}
