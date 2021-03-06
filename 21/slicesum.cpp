#include <fvar.hpp>

dmatrix slice_sum(const d3_array& t)
{
  int nrl = t.rowmin();
  int nrh = t.rowmax();
  //int ncl = t.colmin();
  //int nch = t.colmax();
  ivector ncl(nrl, nrh);
  ivector nch(nrl, nrh);
  for (int i = nrl; i <= nrh; i++)
  {
    ncl(i) = t(1,i).indexmin();
    nch(i) = t(1,i).indexmax();
  }

  dmatrix s(nrl,nrh,ncl,nch);
  s.initialize();

  int sl = t.slicemin();
  int sh = t.slicemax();
  for (int slice = sl; slice <= sh; slice++)
  {
    int i1 = t(slice).rowmin();
    int i2 = t(slice).rowmax();
    for (int i = i1; i <= i2; i++)
    {
      int j1 = t(slice,i).indexmin();
      int j2 = t(slice,i).indexmax();
      for (int j = j1; j <= j2; j++)
      {
	s(i,j) += t(slice,i,j);
      }
    }
  }
  return(s);
}
