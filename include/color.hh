#ifndef COLOR_HH
#define COLOR_HH

#include <unistd.h>
#include <iostream>
#include <cstdio>

struct iftty {
  const char* str;
};

inline FILE* get_standard_stream(const std::ostream& s) {
  if (&s == &std::cout) return stdout;
  else if ((&s == &std::cerr) || (&s == &std::clog)) return stderr;
  return nullptr;
}

std::ostream& operator<<(std::ostream& s, iftty c) {
  FILE* f = get_standard_stream(s);
  if (f && ::isatty(fileno(f))) s << c.str;
  return s;
}

#endif
