#ifndef HAMILTONIAN_TOTALATTICE_FIXED_END_PARALLEL_HPP
#define HAMILTONIAN_TOTALATTICE_FIXED_END_PARALLEL_HPP

#include <string>
#include <cmath>
#include <vector>

namespace hamiltonian{

class TodaLatticeFixedEndParallel {
public:
  static std::string name() { return "Toda Lattice: fixed End : must use Open Boundary Chain Lattice"; }
  // z[0...n-1]: position, z[n...2n-1]: momentum
  TodaLatticeFixedEndParallel(int num_particles, double J, double alpha, std::vector<std::vector<int > > table, int N_adj) : n_(num_particles), J_(J), table_(table),Nd_(N_adj),alpha_(alpha){}

  // The displacements are defined as r_n = u_n+1 - u_n
  // and the Toda potential is defined as J * exp(a * r_n ) -  a * r_n -1

  double energy(double t, std::vector<double> const& z) const {
    const double *x = &z[0];
    const double *v = &z[n_];
    double ene = 0;

    for (int l = 0; l < n_; ++l){
      ene += 0.5 * v[l]*v[l] * 2.0;
      for(int i = 0; i < Nd_/2; ++i){
        double dx =  x[table_[l][i]] - x[l];
        ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
      }
      for(int i = Nd_/2; i < Nd_ ; ++i){
        double dx = x[l] - x[table_[l][i]];
        ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
      }    
    }
    ene /= 2.0;
    double dx = - x[0];
    ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
    dx = x[n_ - 1];
    ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
    return ene;
  }

  double potential_energy(double  t, std::vector<double> const& z) const {
    const double *x = &z[0];
    double ene = 0;

    for (int l = 0; l < n_; ++l){
      for(int i = 0; i < Nd_/2; ++i){
        double dx =  x[table_[l][i]] - x[l];
        ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
      }
      for(int i = Nd_/2; i < Nd_ ; ++i){
        double dx = x[l] - x[table_[l][i]];
        ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
      }    
    }
    ene /= 2.0;
    double dx = - x[0];
    ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
    dx = x[n_ - 1];
    ene += J_ * (std::exp( alpha_ * dx) - 1 - alpha_ * dx);
    return ene;
  }

  double kinetic_energy(double /* t */, std::vector<double> const& z) const {
    const double *x = &z[0];
    const double *v = &z[n_];
    double ene = 0;

    for (int l = 0; l < n_ ; ++l){
      ene += 0.5 * v[l]*v[l];
    }
    return ene;
  }

  // "force" calculation
  void operator()(double t, std::vector<double> const& z, std::vector<double>& force) const {
    const double *x = &z[0];
    const double *v = &z[n_];
    double *fx = &force[0];
    double *fv = &force[n_];

    #pragma omp for 
    for(int i = 0; i < n_ ; ++i){ 
      fx[i] = v[i];
      fv[i] = 0.0;
      for(int d = 0; d < Nd_/2; ++d){
        double dx = x[table_[i][d]] - x[i];
        fv[i] += J_ * alpha_ * ( std::exp(alpha_ * dx) - 1);
      }
      for(int d = Nd_/2; d < Nd_ ; ++d){
        double dx = x[i] -x[table_[i][d]];
        fv[i] -= J_ * alpha_ * ( std::exp(alpha_ * dx) - 1);
      }
    }
    fv[0] += J_* alpha_ * ( std::exp(- alpha_ * x[0]) - 1 );
    fv[n_-1] -=  J_ * alpha_ * ( std::exp( alpha_ * x[n_ - 1]) - 1 );
  }

  int Nd()const { return Nd_ ;}
  int table(int i, int j)const{ return table_[i][j];} 

private:
  double alpha_;
  int n_;
  double J_;
  std::vector<std::vector<int> > table_;
  int Nd_;

};//end TodaLatticeFixedB

}//end namescape

#endif //HAMILTONIAN_TOTALATTICE_FIXED_END_HPP
