#ifndef FLUID_HPP
#define FLUID_HPP
#include <vector>

class Fluid {
private:
  int size;
  float dt;
  float diff;
  float visc;

  int iter;
  std::vector<float> s;
  std::vector<float> density;

  std::vector<float> Vx;
  std::vector<float> Vy;

  std::vector<float> Vx0;
  std::vector<float> Vy0;

public:
  Fluid();
  Fluid(const int size, float dt, float diff, float visc, int iter);

  int IX(int x, int y) { return x + y * size; };

  void addDensity(int x, int y, int amount);
  void addVelocity(int x, int y, int amountX, int amountY);
  void diffuse(int b, std::vector<float> x, std::vector<float> x0, float diff,
               float dt);
  void project(std::vector<float> velocX, std::vector<float> velocY,
               std::vector<float> p, std::vector<float> div);
  void advect(int b, std::vector<float> d, std::vector<float> d0,
              std::vector<float> velocX, std::vector<float> velocY, float dt);
  void lin_solve(int b, std::vector<float> x, std::vector<float> x0, float a,
                 float c);
  void set_bnd(int b, std::vector<float> x);
  void step();
  float getDensities(int x, int y) { return density[IX(x, y)]; };
};

#endif
