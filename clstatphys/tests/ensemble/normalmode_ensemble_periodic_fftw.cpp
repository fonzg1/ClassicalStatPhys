#include <gtest/gtest.h>
#include <clstatphys/ensemble/normalmode_ensemble_periodic_boundary_fftw.hpp>
#include <clstatphys/physics/harmonic_oscillator.hpp>
#include <clstatphys/physics/normalmode_energy_periodic_fftw.hpp>
#include <clstatphys/lattice/chain.hpp>
#include <clstatphys/integrator/runge_kutta_4th.hpp>
#include <limits>

namespace {
class NormalModeEnsemblePeriodicTest : public ::testing::Test {
  protected:  
  virtual void SetUp(){
    // set lattice
    int Ns = 1e+2;
    lattice::Chain lattice(Ns);
    num_particles = lattice.set_num_particles(Ns); 
    ASSERT_EQ(num_particles, Ns);
    N_adj = lattice.number_adjacent();
    ASSERT_EQ(N_adj,2);
    pair_table = std::vector<std::vector<int> >(num_particles,std::vector<int>(N_adj));
    lattice.create_table(pair_table);
    ASSERT_EQ(pair_table[0][0],num_particles-1);
    ASSERT_EQ(pair_table[0][1],1);
    ASSERT_EQ(pair_table[num_particles-1][0],num_particles-2);
    ASSERT_EQ(pair_table[num_particles-1][1],0);
    // set constants
    J = 1.0;
    k_initial = 1;
    N_modes = 10;
    total_energy = 10.0;
    precision = std::numeric_limits<double>::epsilon();
    z.resize(2 * num_particles);
    // set random numbers
    std::size_t seed = 1234;
    std::mt19937 mt(seed);
    // create data set
    ensemble::NormalModeEnsemblePeriodicBoundaryFFTW initializer(num_particles,k_initial, N_modes, total_energy);
    initializer.set_initial_state(z, mt);
  }
  std::vector<double> z;
  int num_particles, N_modes, N_adj, k_initial;
  double total_energy,J,precision;
  std::vector<std::vector<int> > pair_table;
};

TEST_F(NormalModeEnsemblePeriodicTest, FundamentalTest) {
  hamiltonian::HarmonicOscillator hamiltonian(num_particles,J,pair_table,N_adj);
  EXPECT_NEAR(total_energy/hamiltonian.energy(0,z),1.0, 10 * num_particles * precision);
  std::vector<double> energy_modes(num_particles);
  NormalModeEnergyPeriodicFFTW(z,energy_modes);
  for(int i = k_initial; i < N_modes/2 + k_initial; ++i) 
    EXPECT_NEAR(energy_modes[i]/total_energy*N_modes,1.0, 100 * num_particles * precision);
  for(int i = k_initial + N_modes/2; i < num_particles - N_modes/2 - (k_initial - 1); ++i) 
    EXPECT_NEAR(energy_modes[i],0.0, 10 * num_particles * precision); 
  for(int i = num_particles - N_modes/2 - (k_initial - 1) ; i < num_particles - (k_initial - 1); ++i)
    EXPECT_NEAR(energy_modes[i]/total_energy*N_modes,1.0, 100 * num_particles * precision);
  for(int i = num_particles - (k_initial - 1) ; i < num_particles ; ++i)
    EXPECT_NEAR(energy_modes[i],0.0, 10 * num_particles * precision); 
}

TEST_F(NormalModeEnsemblePeriodicTest, TimeDevelopTest) {
  hamiltonian::HarmonicOscillator hamiltonian(num_particles,J,pair_table,N_adj);
  // set integrator 
  integrator::RungeKutta4th integrator(2*num_particles);
  double dt = 1.0e-2;
  double pt = 0.0;
  double N_time = 1e+4;
  double error_rungekutta = dt * dt * dt * dt * dt * 50; //50 is assumed constant of error 
  double energy_init = hamiltonian.energy(pt,z);
  for(int i = 0; i < N_time; ++i){
    integrator.step(pt,dt,z,hamiltonian);
    pt += dt;
  }
  std::vector<double> energy_modes(num_particles);
  NormalModeEnergyPeriodicFFTW(z,energy_modes);
  for(int i = k_initial; i < N_modes/2 + k_initial; ++i) 
    EXPECT_NEAR(energy_modes[i]/total_energy*N_modes,1.0,error_rungekutta*N_time);
  for(int i = k_initial + N_modes/2; i < num_particles - N_modes/2 - (k_initial - 1); ++i) 
    EXPECT_NEAR(energy_modes[i],0.0,error_rungekutta*N_time);
  for(int i = num_particles - N_modes/2 - (k_initial - 1) ; i < num_particles - (k_initial - 1); ++i)
    EXPECT_NEAR(energy_modes[i]/total_energy*N_modes,1.0,error_rungekutta*N_time);
  for(int i = num_particles - (k_initial - 1) ; i < num_particles ; ++i)
    EXPECT_NEAR(energy_modes[i],0.0,error_rungekutta*N_time);
  EXPECT_FALSE(energy_init == hamiltonian.energy(pt,z));
  EXPECT_NEAR(energy_init/hamiltonian.energy(pt,z),1.0,error_rungekutta*N_time);
 
}

}//end namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
      return RUN_ALL_TESTS();
}
