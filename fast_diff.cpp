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

    if (argc != 10) {
		printf("Wrong number of arguments;:\n");
		printf("1. Number of jumps\n2. Output data every N jumps\n"
        "3. Lambda, spin change rate\n4. Temperature\n"
        "5. Temperature of the trap distribution\n"
        "6. G0 Microscopic frequency scale\n"
        "7. Persistence flag (0: no persistence)\n"
        "8. Predefined lattice flag (0: generate on the fly)\n"
        "9. By how much to divide the lattice\n");
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
    int divider = atoi(argv[9]);

    //int initial_spin = 0; Unnecesary since the program assumes same initial
                            //and calculates current one by total number of
                            //spin flips

    double total_time = 0.0;

    vector<double> flip_times;
    flip_times.push_back(0.0);
    vector<double> jump_times;
    jump_times.push_back(0.0);
    vector<int> positions;
    positions.push_back(0);

    int curr_position = 0;
    int past_position = 0;
    double curr_time = 0;
    double past_time = 0;

    //Initial 10 flip times
    /*for (int i = 0; i < 10; i++) {
        flip_times.push_back(flip_times[flip_times.size()-1]+inv_exp(dist(mt),lambda));
    }
    flips = 10;*/
    //That was stupid if flip times are not being saved (which they don't need to)
    int direction = 1;
    double flip_time = inv_exp(dist(mt),lambda);

    //Initialize lattice energies
    vector<double> energies;
    if (PREDEF_LATT) {
        cout << "Generating lattice traps" << endl;
        for (long int i=0; i < totalJumps*2L/divider; i++) {
            if (i%(totalJumpsPe*2L/divider) == 0) {
              cout.flush();
              cout << "\r" << (i*100)/(2L*totalJumps/divider) << "%";
            }
            energies.push_back(inv_exp(dist(mt),1.0/Tg));
        }
        cout << endl;
    }

    //Start calculating jump times and change position
    //If the last flip time is less than the jump_time, add 1 flip times
    //Due to this sequential adding, start counting total number of flips from the end.
    outFile.open("times.out", ios::out);
    cout << "Sim started, (saving to times.out)" << endl;
    for (long int t = 0; t < totalJumps; t++) {

        past_position = curr_position;
        past_time = curr_time;

        if (curr_position > totalJumps/divider || curr_position < -totalJumps/divider) {
            cout << endl << "ERROR: Lattice too small" << endl;
            return -1;
        }

        if (t%totalJumpsPe == 0) {
          cout.flush();
          cout << "\r" << (t*100)/totalJumps << "%";
        }
        double w = 0;
        if (PREDEF_LATT)
            w = G0*exp(-energies[curr_position+totalJumps/divider]/T);
        else
            w = G0*exp(-inv_exp(dist(mt),1.0/Tg)/T); //Set this to a constant for standard diffusion.
        double jTime = inv_exp(dist(mt),w);
        //jump_times.push_back(jump_times[jump_times.size()-1]+jTime);
        curr_time = past_time + jTime;
        total_time += jTime;

        int dir = 0;
        if (!NOPERS_FLAG) {
            while (total_time > flip_time) {
                flip_time += inv_exp(dist(mt),lambda);
                direction *= -1;
            }

            dir = direction;
        }
        else {
            dir = 2*coin(dist(mt))-1;
        }

        //positions.push_back(positions[positions.size()-1]+dir);
        curr_position = past_position + dir;

        if (t%skipOutput == 0)
        outFile << curr_position << "\t" << FIXED_FLOAT(curr_time) << endl;

    }
    cout << endl;
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
    return 1; //Will never be reached but compiler complains.
}
