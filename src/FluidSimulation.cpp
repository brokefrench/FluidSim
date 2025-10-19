#include "FluidSimulation.hpp"
#include <algorithm>
#include <cmath>
#include <vector>

Fluid::Fluid(const int size, float dt, float diff, float visc, int iter) {
  this->size = size;
  this->dt = dt;
  this->diff = diff;
  this->visc = visc;
  this->iter = iter;
  this->s = std::vector<float>(size * size);
  this->density = std::vector<float>(size * size);
  this->Vx = std::vector<float>(size * size);
  this->Vy = std::vector<float>(size * size);
  this->Vx0 = std::vector<float>(size * size);
  this->Vy0 = std::vector<float>(size * size);
}

int Fluid::IX(int x, int y) {
  x = std::clamp(x, 0, size - 1);
  y = std::clamp(y, 0, size - 1);
  return x + y * size;
}

void Fluid::addDensity(int x, int y, int amount) {
  int index = IX(x, y);
  density[index] += amount;
}

void Fluid::addVelocity(int x, int y, int amountX, int amountY) {
  int index = IX(x, y);
  Vx[index] += amountX;
  Vy[index] += amountY;
}

void Fluid::diffuse(int b, std::vector<float> &x, std::vector<float> &x0,
                    float diff, float dt) {
  float a = dt * diff * (size - 2) * (size - 2);
  lin_solve(b, x, x0, a, 1 + 6 * a);
}

void Fluid::lin_solve(int b, std::vector<float> &x, std::vector<float> &x0,
                      float a, float c) {
  float cRecip = 1.0 / c;
  for (int k = 0; k < iter; k++) {
    for (int j = 1; j < size - 1; j++) {
      for (int i = 1; i < size - 1; i++) {
        x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i + 1, j)] + x[IX(i - 1, j)] +
                                           x[IX(i, j + 1)] + x[IX(i, j - 1)])) *
                      cRecip;
      }
    }
    set_bnd(b, x);
  }
}

void Fluid::project(std::vector<float> &velocX, std::vector<float> &velocY,
                    std::vector<float> &p, std::vector<float> &div) {
  for (int j = 1; j < size - 1; j++) {
    for (int i = 1; i < size - 1; i++) {
      div[IX(i, j)] = -0.5f *
                      (velocX[IX(i + 1, j)] - velocX[IX(i - 1, j)] +
                       velocY[IX(i, j + 1)] - velocY[IX(i, j - 1)]) /
                      size;
      p[IX(i, j)] = 0;
    }
  }

  set_bnd(0, div);
  set_bnd(0, p);
  lin_solve(0, p, div, 1, 6);

  for (int j = 1; j < size - 1; j++) {
    for (int i = 1; i < size - 1; i++) {
      velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * size;
      velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * size;
    }
  }

  set_bnd(1, velocX);
  set_bnd(2, velocY);
}

void Fluid::advect(int b, std::vector<float> &d, std::vector<float> &d0,
                   std::vector<float> &velocX, std::vector<float> &velocY,
                   float dt) {
  float dtx = dt * (size - 2);
  float dty = dt * (size - 2);
  float Nfloat = size;

  for (int j = 1, jfloat = 1; j < size - 1; j++, jfloat++) {
    for (int i = 1, ifloat = 1; i < size - 1; i++, ifloat++) {
      float tmp1 = dtx * velocX[IX(i, j)];
      float tmp2 = dty * velocY[IX(i, j)];
      float x = ifloat - tmp1;
      float y = jfloat - tmp2;

      x = std::clamp(x, 0.5f, Nfloat + 0.5f);
      y = std::clamp(y, 0.5f, Nfloat + 0.5f);

      int i0 = std::floor(x);
      int i1 = i0 + 1;
      int j0 = std::floor(y);
      int j1 = j0 + 1;

      float s1 = x - i0;
      float s0 = 1.0f - s1;
      float t1 = y - j0;
      float t0 = 1.0f - t1;

      d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                    s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
    }
  }
  set_bnd(b, d);
}

void Fluid::set_bnd(int b, std::vector<float> &x) {
  for (int i = 1; i < size - 1; i++) {
    x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
    x[IX(i, size - 1)] = b == 2 ? -x[IX(i, size - 2)] : x[IX(i, size - 2)];
  }
  for (int j = 1; j < size - 1; j++) {
    x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
    x[IX(size - 1, j)] = b == 1 ? -x[IX(size - 2, j)] : x[IX(size - 2, j)];
  }

  x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
  x[IX(0, size - 1)] = 0.5f * (x[IX(1, size - 1)] + x[IX(0, size - 2)]);
  x[IX(size - 1, 0)] = 0.5f * (x[IX(size - 2, 0)] + x[IX(size - 1, 1)]);
  x[IX(size - 1, size - 1)] =
      0.5f * (x[IX(size - 2, size - 1)] + x[IX(size - 1, size - 2)]);
}

void Fluid::step() {
  diffuse(1, Vx0, Vx, visc, dt);
  diffuse(2, Vy0, Vy, visc, dt);
  project(Vx0, Vy0, Vx, Vy);

  advect(1, Vx, Vx0, Vx0, Vy0, dt);
  advect(2, Vy, Vy0, Vx0, Vy0, dt);

  project(Vx, Vy, Vx0, Vy0);

  diffuse(0, s, density, diff, dt);
  advect(0, density, s, Vx, Vy, dt);
}
