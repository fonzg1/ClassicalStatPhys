#ifndef HAMILTONIAN_TOTA_LATTICEHPP
#define HAMILTONIAN_TOTA_LATTICEHPP

#include <string>
#include <cmath>
#include <vector>

namespace hamiltonian{

class TodaLattice {
public:
  static std::string name() { return "Toda Lattice Hamiltonian"; }
  // z[0...n-1]: position, z[n...2n-1]: momentum
  TodaLattice(int num_particles, double J, double alpha, std::vector<std::vector<int > > table, int N_adj) : n_(num_particles), J_(J), table_(table),Nd_(N_adj),alpha_(alpha){}
  TodaLattice() = default;

  double energy(double t, std::vector<double> const& z) const {
    return potential_energy(t, z) + kinetic_energy(t, z);
  }
  double potential_energy(double  t, std::vector<double> const& z) const {
    double ene = 0;
    for(int i = 0; i < n_ ; ++i) ene += target_potential_energy(i,z,t);
    return ene/2.0;
  }
  double kinetic_energy(double /* t */, std::vector<double> const& z) const {
    double ene = 0;
    for(int i = 0; i < n_; ++i) ene += target_kinetic_energy(i,z);
    return ene;
  }

  // The displacements are defined as r_n = u_n+1 - u_n
  // and the Toda potential is defined as J * exp(a * r_n ) -  a * r_n -1

  double target_potential_energy(int l, std::vector<double> const& z, double t) const {
    const double *x = &z[0];
    double ene = 0;
    for(int i = 0; i < Nd_/2; ++i){
      double dx = x[l] - x[table_[l][i]];
      ene += J_ * ( std::exp( alpha_ * dx) - 1.0 -   alpha_ * dx);
    }
    for(int i = Nd_/2; i < Nd_ ; ++i){
      double dx = x[table_[l][i]] - x[l];
      ene += J_ * ( std::exp( alpha_ * dx) - 1.0 -   alpha_ * dx);
    }

    return ene;
  }

  double energy_field(int l, std::vector<double> const&z, double t) const {
    const double *x = &z[0];
    const double *v = &z[n_];
    double ene = 0;
    double dx = x[l] - x[table_[l][0]];
    ene += J_ * ( std::exp( alpha_ * dx) - 1.0 -   alpha_ * dx);
    ene += 0.5 * v[l]*v[l];
    return ene;
  }

  double target_kinetic_energy(int l, std::vector<double> const& z) const {
    const double *v = &z[n_];
    double ene = 0;
    ene += 0.5 * v[l]*v[l];
    return ene;
  }

  // "force" calculation
  void operator()(double t, std::vector<double> const& z, std::vector<double>& force) const {
    const double *x = &z[0];
    const double *v = &z[n_];
    double *fx = &force[0];
    double *fv = &force[n_];
    for(int i = 0; i < n_ ; ++i) fx[i] = v[i];
    for(int i = 0; i < n_ ; ++i){
      fv[i] = 0.0;
      for(int d = 0; d < Nd_/2; ++d){
        double dx = x[i] -x[table_[i][d]];
        fv[i] -= J_ * ( alpha_ * std::exp(alpha_ * dx) - alpha_ );
      }
      for(int d = Nd_/2; d < Nd_; ++d){
        double dx = x[table_[i][d]] - x[i];
        fv[i] += J_ * ( alpha_ * std::exp(alpha_ * dx) - alpha_ );
      }
    }
  }

  int Nd()const { return Nd_ ;}
  int table(int i, int j)const{ return table_[i][j];} 

private:
  int Nd_;
  int n_;
  double J_;
  double alpha_;
  std::vector<std::vector<int> > table_;
};//end TodaLattice

}// end namespace

#endif //HAMILTONIAN_TOTA_LATTICEHPP

