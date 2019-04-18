# Random background persitent diffusion

Diffusion of particle with persitence in its direction in 1D, in a random energy landscape.

### Compiling
make

### Usage
The program fast_diff simulates the diffusion of the particle with the following parameters
T: Temperature of the particle
Tg: Temperature of the distribution of energy traps
Lambda: Temeprature of the exponential distribution of spin flips
G0: Prefactor of the typical waiting time for a jump

The program uni_corr takes the output of fast_diff and computes the average square displacement. Right now the parameters are put directly in the source code.
