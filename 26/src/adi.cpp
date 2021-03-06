//$Id: adi.cpp 3132 2014-02-04 21:54:57Z johnoel $
#include "coff_t.h"
//#include "intersav.h"
#ifdef SIMULATE
   #undef SIMULATE
#endif
#include "trace.h"

//extern setsaveinterflag interflag;
//extern int saveinterflag;

void tridag(const dvector & a, const dvector & b,
	    const dvector & c, const dvector & r, dvector & u,
	    const dvector & bet, const dvector & gam, const int nl,
	    const int nu, dvector & v);



//extern ofstream clogf;
//void grid_report(dmatrix& t, ofstream& c, const char* name);

/*
void coff_t::vSet(prevariable y, const double x)
{
   value(y) = x;
}


void coff_t::vSet(double &y, const double x)
{
   y = x;
}
*/

void coff_t::adi(dvar_matrix & _u)//, intersavetype * savefile)
{
   /*
      cout << "savefile = " << savefile << endl;
      cout << "saveinterflag = " << saveinterflag << endl;
      HERE
      if (saveinterflag == MM_intersave::ADMB_mode)
      ad_exit(1);
    */
   dvar_matrix & u = _u;
   dmatrix a = value(_a);
   dmatrix b = value(_b);
   dmatrix c = value(_c);
   dmatrix d = value(_d);
   dmatrix e = value(_e);
   dmatrix f = value(_f);
   dmatrix g = value(_g);
   dmatrix xgam = value(_xgam);
   dmatrix xbet = value(_xbet);
   dmatrix ygam = value(_ygam);
   dmatrix ybet = value(_ybet);
   dmatrix psi = value(_psi);
   dvector rr = value(_rr);
   dvector uu = value(_uu);
   dvector work = value(_work);

   rr.initialize();
   uu.initialize();
   work.initialize();

   int nits = k;
   double r = 4.0 * (double) k;



   for (int kits = 1; kits <= nits; kits++)
   {
      for (int j = 1; j <= jmax; j++)
      {
	 int il = ilb(j);
	 int iu = iub(j);

	 for (int i = il; i <= iu; i++)
	 {
	    if (j == jlb(i))
	       rr[i] =
		  g[i][j] + (r - e[i][j]) * value(u[i][j]) -
		  f[i][j] * value(u[i][j + 1]);
	    else if (j == jub(i))
	       rr[i] =
		  g[i][j] - d[i][j] * value(u[i][j - 1]) + (r -
							   e[i][j]) *
		  value(u[i][j]);
	    else
	       rr[i] =
		  g[i][j] - d[i][j] * value(u[i][j - 1]) + (r -
							   e[i][j]) *
		  value(u[i][j]) - f[i][j] * value(u[i][j + 1]);
	 }

	 tridag(a[j], b[j], c[j], rr, uu, xbet[j], xgam[j], il, iu, work);

	 for (int i = il; i <= iu; i++)
	 {
	    psi[i][j] = uu[i];
	 }
      }				//j loop

      vSaveValue(u, "u"); //, savefile);

#ifdef SIMULATE
      grid_report(psi, clogf, "psi (first half)");
#endif

      for (int i = 1; i <= imax; i++)
      {
	 int jl = jlb(i);
	 int ju = jub(i);
	 for (int j = jl; j <= ju; j++)
	 {
	    if (i == ilb(j))
	       rr[j] =
		  g[i][j] + (r - b[j][i]) * psi[i][j] - c[j][i] * psi[i +
								      1]
		  [j];
	    else if (i == iub(j))
	       rr[j] =
		  g[i][j] - a[j][i] * psi[i - 1][j] + (r -
						       b[j][i]) *
		  psi[i][j];
	    else
	       rr[j] =
		  g[i][j] - a[j][i] * psi[i - 1][j] + (r -
						       b[j][i]) *
		  psi[i][j] - c[j][i] * psi[i + 1][j];
	 }

	 tridag(d[i], e[i], f[i], rr, uu, ybet[i], ygam[i], jl, ju, work);

	 for (int j = jl; j <= ju; j++)
	 {
	    u[i][j]=uu[j];
	    //vSet(u[i][j], uu[j]);
	 }
      }				//i loop 

#ifdef SIMULATE
      grid_report(u, clogf, "u (second half)");
#endif

      vSaveValue(psi, "psi"); //, savefile);
   } // end of kits loop

   // condition to prevent gradient structure error with constant objects:
   // Error -- you are trying to create a dvar_vector object when there is 
   // no object of type gradient_structure in scope 
   //if (saveinterflag == MM_intersave::ADMB_mode)
      adiADMBIntersave(u, psi);

} // end of adi


void coff_t::adiADMBIntersave(dvar_matrix & u, const dmatrix & psi)
{
   save_identifier_string((char *) "adi1");
   psi.save_dmatrix_position();
   _a.save_dvar_matrix_value();
   _a.save_dvar_matrix_position();
   _b.save_dvar_matrix_value();
   _b.save_dvar_matrix_position();
   _c.save_dvar_matrix_value();
   _c.save_dvar_matrix_position();
   save_identifier_string((char *) "adi2");
   _d.save_dvar_matrix_value();
   _d.save_dvar_matrix_position();
   _e.save_dvar_matrix_value();
   _e.save_dvar_matrix_position();
   _f.save_dvar_matrix_value();
   _f.save_dvar_matrix_position();
   _g.save_dvar_matrix_value();
   _g.save_dvar_matrix_position();
   save_identifier_string((char *) "adi3");
   _xgam.save_dvar_matrix_value();
   _xgam.save_dvar_matrix_position();
   _ygam.save_dvar_matrix_value();
   _ygam.save_dvar_matrix_position();
   save_identifier_string((char *) "xbet");
   _xbet.save_dvar_matrix_value();
   _xbet.save_dvar_matrix_position();
   save_identifier_string((char *) "ybet");
   _ybet.save_dvar_matrix_value();
   _ybet.save_dvar_matrix_position();
   save_identifier_string((char *) "adi4");
   //save_int_value(456);

   u.save_dvar_matrix_position();
   //save_int_value(123);
   save_identifier_string((char *) "adi5");

   void *pthis = (void *) this;
   //TTRACE(pthis,(void*)this)
   save_pointer_value(pthis);
   save_identifier_string((char *) "adi6");

   gradient_structure::GRAD_STACK1->set_gradient_stack(bogus_dfadi);
}				// adiADMBIntersave(...)


//---------- vGet overloads
/*
void coff_t::adiADMBIntersave(dmatrix & u, const dmatrix & psi)
{
}


double coff_t::vGet(dvariable x)
{
   return (value(x));
}


double coff_t::vGet(double x)
{
   return (x);
}

*/

void coff_t::vSaveValue(dmatrix & m, const char *id)
{
   //if (saveinterflag == MM_intersave::ADMB_mode)
   {
      save_identifier_string((char *) id);
      m.save_dmatrix_value();
   }
}


void coff_t::vSaveValue(dvar_matrix & m, const char *id)
{
   //if (saveinterflag == MM_intersave::ADMB_mode)
   {
      save_identifier_string((char *) id);
      m.save_dvar_matrix_value();
   }
}
/*
void coff_t::vSaveValue(dmatrix & m, const char *id, intersavetype * savefile)
{
   if (saveinterflag == MM_intersave::MM_mode)
   {
      save_identifier_string((char *) id);
      interflag.msavep(m, id, savefile);
   }
}
*/

//---------- vGet overloads
/*
 
dmatrix coff_t::
vGet(dmatrix& m) {return(m);}
 dmatrix coff_t::vGet(dmatrix& m);

 
dmatrix coff_t::
vGet(dvar_matrix& m)
{
   dmatrix vm = value(m);
   return(vm);
}
 dmatrix coff_t:: vGet(dvar_matrix& m);

 
dvector coff_t::
vGet(dvector& v) {return(v);}
 dvector coff_t::vGet(dvector& v);

 
dvector coff_t::
vGet(dvar_vector& m)
{
   dvector vm;
   vm = value(m);
   return(vm);
}
 dvector coff_t:: vGet(dvar_vector& m);


::vGet(double x)
{
  return(x);
}

*/
void coff_t::vGetValue(dmatrix& m, const char * id, dmatrix_position * pos)
{
  m = restore_dmatrix_value(*pos);
  verify_identifier_string((char*)id);
}

void coff_t::vGetValue(dvar_matrix& m, const char * id, dvar_matrix_position * pos)
{
  m = restore_dvar_matrix_value(*pos);
  verify_identifier_string((char*)id);
}

/*
 ::
vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
             dmatrix_position * pos)
{
  m = restore_dmatrix_value(*pos);
  verify_identifier_string((char*)id);
}

 ::
vGetValue(dmatrix& m, const char * id, intersavetype * savefile, 
             dmatrix_position * pos)
{
    interflag.mgetp(m, id, savefile);
}
*/
