//$Id: Upwind_coff_t-comp.cpp 2754 2011-01-02 20:57:07Z jsibert $
// 08/28/98 Changed conditionals at edge of model domain ragged arrays to
//          correct tag conservation problem. Also changed moving average
//          smoother (uvntrp4.cpp) so that boundary cell velocities != 0 to
//          correct "sticky" edges in low diffusion situations
// 06/08/95 Changed atype-btype-ctype formulations to use the centred-space
//          variable diffusion approximation
//          Imported CHANNEL cell type from coffcmp4.cpp
// 06/07/95 Altered btype boundary formulations for closed and open boundaries
//

// one point difference for dN/dx*dS/dx
#include "coff_t.h"
#include "intersav.h"
extern ofstream clogf;

#define ELEM elem

prevariable smoother(const dvariable &x, const double scale);
void bad_cell_type(const char* function, int boundary);


/*
void dfCheckCoffComp()
{
	dvar_matrix_position positionA = restore_dvar_matrix_position();
	dvar_matrix_position positionRawB = restore_dvar_matrix_position();
	dvar_matrix_position positionC = restore_dvar_matrix_position();
	dvar_matrix_position positionD = restore_dvar_matrix_position();
	dvar_matrix_position positionE = restore_dvar_matrix_position();
	dvar_matrix_position positionF = restore_dvar_matrix_position();
	dvar_matrix_position positionYBet = restore_dvar_matrix_position();
	dvar_matrix_position positionYGam = restore_dvar_matrix_position();
	dmatrix dfa = restore_dvar_matrix_derivatives(positionA);
	dmatrix dfb = restore_dvar_matrix_derivatives(positionB);
	dmatrix dfc = restore_dvar_matrix_derivatives(positionC);
	dmatrix dfd = restore_dvar_matrix_derivatives(positionD);
	dmatrix dfe = restore_dvar_matrix_derivatives(positionE);
	dmatrix dff = restore_dvar_matrix_derivatives(positionF);
	dmatrix dfybet = restore_dvar_matrix_derivatives(positionYBet);
	dmatrix dfygam = restore_dvar_matrix_derivatives(positionYGam);

	dfa.save_dmatrix_derivatives(positionA);
	dfb.save_dmatrix_derivatives(positionB);
	dfc.save_dmatrix_derivatives(positionC);
	dfd.save_dmatrix_derivatives(positionD);
	dfe.save_dmatrix_derivatives(positionE);
	dff.save_dmatrix_derivatives(positionF);
	dfybet.save_dmatrix_derivatives(positionYBet);
	dfygam.save_dmatrix_derivatives(positionYGam);
}
void checkCoffComp(coff_t &coff, par_t<double> &param, dvar_matrix &u, dvar_matrix &v, dvar_matrix &sigma)
{
	dvar_matrix &a = coff.getVariableA();
	dvar_matrix &raw_b = coff.getVariableRawB();
	dvar_matrix &c = coff.getVariableC();
	dvar_matrix &d = coff.getVariableD();
	dvar_matrix &e = coff.getVariableE();
	dvar_matrix &f = coff.getVariableF();
	dvar_matrix &ygam = coff.getVariableYGam();
	dvar_matrix &ybet = coff.getVariableYBet();

	ygam.save_dvar_matrix_position();
	ybet.save_dvar_matrix_position();
	f.save_dvar_matrix_position();
	e.save_dvar_matrix_position();
	d.save_dvar_matrix_position();
	c.save_dvar_matrix_position();
	raw_b.save_dvar_matrix_position();
	a.save_dvar_matrix_position();

	gradient_structure::GRAD_STACK1->set_gradient_stack(dfCheckCoffComp);
}
*/

extern double sscale;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::comp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, year_month& date, dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma)
{
  //clogf << "Upwind_coff_t::comp  for " << date << endl;

  int i, j;
  dvar_matrix& acoff = this->getVariableA();
  dvar_matrix& bcoff = this->getVariableRawB();
  dvar_matrix& ccoff = this->getVariableC();
  dvar_matrix& dcoff = this->getVariableD();
  dvar_matrix& ecoff = this->getVariableE();
  dvar_matrix& fcoff = this->getVariableF();
  dvar_matrix &ygam = this->getVariableYGam();
  dvar_matrix &ybet = this->getVariableYBet();

  //double dx    = param.get_deltax(1);
  double dx    = param.get_deltax_eq();
  double dy    = param.get_deltay();
  double dxdx  = dx * dx;
  double dydy  = dy * dy;
  double twodxdx = 2.0 * dxdx;
  double twodydy = 2.0 * dydy;
  int _m = param.get_m(); //imax;
  int _n = param.get_n(); //jmax;
  ivector jlb1(0,_m+1);
  ivector jub1(0,_m+1);
  for (i = 1; i <= _m; i++)
  {
   jlb1(i) = this->jlb(i)-1;
   jub1(i) = this->jub(i)+1;
  }
  jlb1(0) = 0;
  jlb1(_m+1) = 0;
  jub1(0) = _n+1;
  jub1(_m+1) = _n+1;

  if (param.m_ipar[34]<=0)
  {
    clogf << "param.m_ipar[34] must be greater than 0" << endl;
    exit(1);
  }
  sscale    = (double)param.m_ipar[34]/100.;

  long nstep;
  if (this->get_k() > 1)
    nstep = this->get_k();  // time steps per month
  else
    nstep = 1;

  double rdt2 = 2*nstep;  // 2/delta_t

  bound boundary;

  for ( i=1; i<=_m; i++)
  {
    int j1 = this->jlb(i);
    int jn = this->jub(i);
    for ( j=j1; j<=jn; j++)
    {
      boundary = param.get_cell_type(i,j);
      //clogf << "*b:" << boundary.b << "*ey:" << boundary.edgey() << endl;
      dvariable bsigy = 0.0;
      dvariable fsigy = 0.0;
      dvariable vm = 0.0;
      dvariable vv = v(i,j);
      dvariable vp = 0.0;
      if (j>=this->jlb(i))
      {
        bsigy =  (sigma[i][j] + sigma[i][j-1]) / twodydy;
        vm = v[i][j-1];
      }
      if (j<=this->jub(i))
      {
        fsigy =  (sigma[i][j] + sigma[i][j+1]) / twodydy;
        vp = v[i][j+1];
      }
      // y direction
      dcoff.ELEM(i,j) = atype(vm, bsigy, boundary.edgey());
      ecoff.ELEM(i,j) = btype(vv, bsigy, fsigy, rdt2, boundary.edgey());
      fcoff.ELEM(i,j) = ctype(vp, fsigy, boundary.edgey());

// this is only true for tags; rtype needs work for growing populations
//    gcoff[i][j] = 0.0; // .initialize()ed in fgcomp

    } // end of j loop
  } // end of i loop


  for ( j=1; j<=_n;j++)
  {
    int i1 = this->ilb(j);
    int im = this->iub(j);
    for (i=i1; i<=im; i++)
    {
      boundary = param.get_cell_type(i,j);

      dvariable bsigx = 0.0;
      dvariable fsigx = 0.0;
      dvariable um = 0.0;
      dvariable uu = u(i,j);
      dvariable up = 0.0;
      if ( (j >= jlb1(i-1)) && (j <= jub1(i-1)) )
      {
        bsigx = (sigma[i][j]+sigma[i-1][j]) / twodxdx;
        um = u[i-1][j];
      }
      if ( (j >= jlb1(i+1)) && (j <= jub1(i+1)) )
      {
        fsigx = (sigma[i][j]+sigma[i+1][j]) / twodxdx;
        up = u[i+1][j];         //
      }
 
      // x direction
      acoff.ELEM(j,i) = atype(um, bsigx, boundary.edgex());
      bcoff.ELEM(j,i) = btype(uu, bsigx, fsigx, rdt2, boundary.edgex());
      ccoff.ELEM(j,i) = ctype(up, fsigx, boundary.edgex());
    } // end of i loop
  } // end of j loop

  for (i=1; i<=_m; i++)
  {
    int j1 = ecoff(i).indexmin();
    int j2 = ecoff(i).indexmax();

    ygam.ELEM(i,j1) = 0.0;
    ybet.ELEM(i,j1) = 1.0/ecoff.ELEM(i,j1);
    j1++;
    for (int j=j1; j<=j2; j++)
    {
      ygam.ELEM(i,j) = fcoff.ELEM(i,j-1) * ybet.ELEM(i,j-1);
      ybet.ELEM(i,j) = 1.0 /( ecoff.ELEM(i,j) - dcoff.ELEM(i,j) 
                                * ygam.ELEM(i,j));
    }
  }

} /* end of coffcomp */
template void Upwind_coff_t<d3_array,dmatrix,dvector,double>::comp(par_t<d3_array,dmatrix,dvector,double>& param, year_month& date, dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma);
template void Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::comp(par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, year_month& date, dvar_matrix& u, dvar_matrix& v, dvar_matrix& sigma);


template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
prevariable Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::atype(const dvariable &um, const dvariable &bsig, const int boundary)
{
	dvariable acoff = 0;
  switch (boundary)
  {
    case NONE: // n o
    case RIGHT_CLOSED:
      if (um > 0)
        acoff =  -smoother(um,sscale) - bsig;
      else
        acoff = -bsig;
      break;
    case RIGHT_OPEN:
      acoff =  -smoother(um,sscale);
      break;
    case LEFT_OPEN:
    case LEFT_CLOSED:
    case ISLAND:
      case CHANNEL:
      acoff = 0;
      break;
    default:
      bad_cell_type("atype", boundary);
  }
	return acoff;
}
template prevariable Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::atype(const dvariable &um, const dvariable &bsig, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
prevariable Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::btype(const dvariable &uu, const dvariable &bsig, const dvariable &fsig, const double dt, const int boundary)
{
	dvariable bcoff = 0;
  switch (boundary)
  {
    case LEFT_OPEN:
      bcoff = dt - smoother(uu,sscale);
      break;
    case RIGHT_OPEN:
      bcoff = dt + smoother(uu,sscale);
      break;
    case NONE:
      if (uu > 0)
        bcoff = dt + (bsig + fsig) + smoother(uu,sscale);
      else
        bcoff = dt + (bsig + fsig) - smoother(uu,sscale);
      break;
    case LEFT_CLOSED:
      if (uu > 0)
        bcoff = dt + fsig + smoother(uu,sscale);
      else
        bcoff = dt + fsig;
      break;
    case RIGHT_CLOSED:
      if (uu > 0)
        bcoff = dt + bsig;
      else
        bcoff = dt + bsig - smoother(uu,sscale);
      break;
    case ISLAND:
    case CHANNEL:
      bcoff = dt;
      break;
    default:
      bad_cell_type("btype", boundary);
  }
	return bcoff;
}
template prevariable Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::btype(const dvariable &uu, const dvariable &bsig, const dvariable &fsig, const double dt, const int boundary);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
prevariable Upwind_coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::ctype(const dvariable &up, const dvariable &fsig, const int boundary)
{
	dvariable ccoff = 0;
  switch (boundary)
  {
    case NONE:
    case LEFT_CLOSED:
      if (up > 0)
        ccoff = -fsig;
      else
        ccoff =  smoother(up,sscale) - fsig;
      break;
    case LEFT_OPEN:
      ccoff =  smoother(up,sscale);
      break;
    case RIGHT_OPEN:
    case RIGHT_CLOSED:
    case ISLAND:
    case CHANNEL:
      ccoff = 0;
      break;
    default:
      bad_cell_type("ctype", boundary);
  }
	return ccoff;
}
template prevariable Upwind_coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>::ctype(const dvariable &up, const dvariable &fsig, const int boundary);
