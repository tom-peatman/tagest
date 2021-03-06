//$Id: sigpen.cpp 2754 2011-01-02 20:57:07Z jsibert $
#include "par_t_reg.h"
#undef TRACE
#define TRACE(object) clogf << "line " << __LINE__ << ", file "\
      << __FILE__ << ", " << #object " = " << object << endl;
extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void sigpen(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& temp, year_month& date)
{

  int       i,j, k1, k2, k3, k4;

  //TRACE(param.ipar[41])
  if (param.m_ipar[41] > 0)
  {
    int m = param.get_m();
    int n = param.get_n();
    MATRIX dxsigma(1,m, 1,n);

    VECTOR& ug = param.get_usergridSigma(param.get_season(date));
    //clogf << "sigpen for " << date << "; season = " << param.get_season(date) << endl;
    int ngrid = param.get_ngrid();
    VECTOR sigma(1,ngrid);
    // convert sigma to a radius
    for (int k = 1; k <= ngrid; k++)
    {
      sigma[k] = sqrt(ug[k]/M_PI);
    }
    double sigpen = double(param.m_ipar[42])*1.0e-9;
    ////TRACE(sigpen)

    /* calculate differences in x direction */

    for (j=1; j <= n; j++)
    {
      for (i=1; i <= m-1; i++)
      {
	k1 = param.get_gridmap(i, j);
	k2 = param.get_gridmap(i+1, j);
	if ( (k1>0) && (k2>0) )
	{
	  dxsigma(i,j) = sigma[k2] -  sigma[k1];
	}
	else
	{
	  dxsigma(i,j) = 0.0;
	}
      }//i loop
    }//j loop

    DOUBLE d2xsigma = 0;

    for (j=1; j <= n; j++)
    {
      for (i=1; i <= m-2; i++)
      {
	d2xsigma = dxsigma[i+1][j] - dxsigma[i][j];
	temp -= sigpen*d2xsigma*d2xsigma;
      }//i loop
    }//j loop
    ////TRACE(temp)

  /* rename pointers to keep code understanable? */

    MATRIX& dysigma = dxsigma;
    DOUBLE d2ysigma = 0;

  /* calculate differences in y direction */

    for (j=1; j <= n-1; j++)
    {
      for (i=1; i <= m; i++)
      {
	k3 = param.get_gridmap(i, j);
	k4 = param.get_gridmap(i, j+1);  /* = k2 */
	if ( (k3>0) && (k4>0) )
	{
	  dysigma(i,j) = sigma[k4] -  sigma[k3];
	}
	else
	{
	  dysigma(i,j) = 0.0;
	}
      }//i loop
    }//j loop

    for (j=1; j <= n-2; j++)
    {
      for (i=1; i <= m; i++)
      {
	d2ysigma = dysigma[i][j+1] - dysigma[i][j];
	temp -= sigpen*d2ysigma*d2ysigma;
      }//i loop
    }//j loop
    ////TRACE(temp)
    if (fabs(temp) > 1.0)
    {
      clogf << "* * warning: sigma curvature penalty is getting large, " 
            << temp << endl;
    }
  }//if m_ipar[41] > 0
}
template void sigpen(par_t_reg<d3_array,dmatrix,dvector,double>& param, double& temp, year_month& date);
template void sigpen(par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>& param, dvariable& temp, year_month& date);
