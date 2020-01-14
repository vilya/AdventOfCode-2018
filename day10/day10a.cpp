#include <algorithm>
#include <cstdio>
#include <vector>


struct ivec2 {
  int x, y;

  ivec2& operator += (ivec2 v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  ivec2& operator -= (ivec2 v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
};
bool operator == (ivec2 a, ivec2 b) { return a.y == b.y && a.x == b.x; }
bool operator != (ivec2 a, ivec2 b) { return a.y != b.y || a.x != b.x; }
bool operator <  (ivec2 a, ivec2 b) { return (a.y != b.y) ? (a.y < b.y) : (a.x < b.x); }
ivec2 min(ivec2 a, ivec2 b) { return ivec2{ std::min(a.x, b.x), std::min(a.y, b.y) }; }
ivec2 max(ivec2 a, ivec2 b) { return ivec2{ std::max(a.x, b.x), std::max(a.y, b.y) }; }
ivec2 operator * (ivec2 a, int k) { return ivec2{ a.x * k, a.y * k }; }


struct Particle {
  ivec2 pos;
  ivec2 vel;
};
bool order_by_pos(const Particle& a, const Particle& b) { return (a.pos != b.pos) ? (a.pos < b.pos) : (a.vel < b.vel); }


struct Rect {
  ivec2 lo, hi;

  int width()  const { return hi.x - lo.x + 1; }
  int height() const { return hi.y - lo.y + 1; }
  int area()   const { return width() * height(); }
};


Rect bounds(const std::vector<Particle>& particles)
{
  Rect r;
  r.lo = r.hi = particles.front().pos;
  for (const Particle& p : particles) {
    r.lo = min(r.lo, p.pos);
    r.hi = max(r.hi, p.pos);
  }
  return r;
}


void show(const std::vector<Particle>& particles)
{
  // assumes particles have been sorted by y then by x.
  Rect r = bounds(particles);

  size_t i = 0;
  ivec2 pos;
  for (pos.y = r.lo.y; pos.y <= r.hi.y; pos.y++) {
    for (pos.x = r.lo.x; pos.x <= r.hi.x; pos.x++) {
      if (pos == particles[i].pos) {
        fputc('#', stdout);
        while (pos == particles[i].pos)
          ++i;
      }
      else {
        fputc('.', stdout);
      }
    }
    fputc('\n', stdout);
  }
}


void step_forwards(std::vector<Particle>& particles, int steps=1)
{
  if (steps == 1) {
    for (Particle& p : particles) {
      p.pos += p.vel;
    }
  }
  else {
    for (Particle& p : particles) {
      p.pos += p.vel * steps;
    }
  }
}


void step_backwards(std::vector<Particle>& particles, int steps=1)
{
  if (steps == 1) {
    for (Particle& p : particles) {
      p.pos -= p.vel;
    }
  }
  else {
    for (Particle& p : particles) {
      p.pos -= p.vel * steps;
    }
  }
}


int main(int argc, char** argv)
{
  std::vector<Particle> particles;

  FILE* f = fopen(argv[1], "r");
  Particle p;
  while (fscanf(f, "position=< %d, %d> velocity=< %d, %d>\n", &p.pos.x, &p.pos.y, &p.vel.x, &p.vel.y) == 4) {
    particles.push_back(p);
  }
  fclose(f);

  const int n = particles.size() / 2;
  const int maxArea = n * n;

  Rect r = bounds(particles);
  int prevArea = std::numeric_limits<int>::max();
  int area = r.area();
  while (area > maxArea || area < prevArea) {
    step_forwards(particles);
    r = bounds(particles);
    prevArea = area;
    area = r.area();
  }
  step_backwards(particles);
  std::sort(particles.begin(), particles.end(), order_by_pos);
  show(particles);
}
