#include <fvar.hpp>
#include <values.h>

int min(const ivector& v)
{
  int t = MAXINT;
  int i1 = v.indexmin();
  int i2 = v.indexmax();
  for (int i=i1; i <= i2; i++)
  {
    if (v(i) < t)
      t = v(i);
  }
  return t;
}

int min(const imatrix& m)
{
  int t = MAXINT;
  int i1 = m.rowmin();
  int i2 = m.rowmax();
  for (int i = i1; i <= i2; i++)
  {
    int tt = min(m(i));
    if (tt < t)
      t = tt;
  }
  return t;
}
