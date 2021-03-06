//$Id: rfgcomp.cpp 2930 2012-01-25 20:47:14Z eunjung $
#include "par_t_reg.h"
#include "intersav.h"
#include "fish_rec.h"
#include "trace.h"

#ifdef __GNUC__
  #if (__GNUC__ > 3)
    #include <iostream>
    #include <fstream>
    #include <iomanip>
    using namespace std;
  #else
    #include <iostream>
    #include <fstream.h>
    #include <iomanip.h>
  #endif
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <unistd.h>
  #include <time.h>
#endif

#ifdef __SUN__
#include <unistd.h>
#include <fcntl.h>
#endif


#if !defined(__SUN__) && !defined(__GNUC__)
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#endif

template <typename MATRIX1, typename MATRIX2>
void resid_comp(const MATRIX1& obs, const MATRIX2& pred, const MATRIX1& like, const int report);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void velpen(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& temp, year_month& date);
                                                                                                                                                                                                     
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfvelpen(par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, year_month& date);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void total_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen);
                                                                                                                                                                                                     
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_total_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, double& dfpen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
int negative_tag_penalty(const MATRIX& tags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& pen);
                                                                                                                                                                                                     
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_negative_tag_penalty(dmatrix& tags, dmatrix& dftags, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, double& dfpen);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_poisson_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void poisson_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort) {};
                                                                                                                                                                                                     
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_nb_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort, dvector& a, dvector& dfa, const int naa, const int nb_scale);
                                                                                                                                                                                                      template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void negative_binomial_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort, VECTOR& a, const int naa, const int nb_scale);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void exponential_like(DOUBLE& like, MATRIX& pred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, MATRIX& z, ivector& effort_occured, D3_ARRAY& fmort);
                                                                                                                                                                                                     
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void df_exp_like(double& dflike, dmatrix& pred_tags, dmatrix& dfpred_tags, year_month& date, int cohort, recaptype_vector& recaps, int nrec, par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& z, dmatrix& dfz, ivector& effort_occured, d3_array& fmort, d3_array& dffmort);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void sigpen(par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, DOUBLE& temp, year_month& date);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dfsigpen(par_t_reg<d3_array,dmatrix,dvector,double>& dfparam, par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, year_month& date);

d3_array make_d3_array(int sl,int sh, int nrl, int nrh, const ivector& ncl, const ivector& nch);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void clean(dvector& g, int nvar, par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& pdfpar, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& dfcoff,
       dmatrix& dfrelease,d3_array& dfz, double& plike);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void get_effort_array(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, indexed_regional_fishery_record& irfr, d3_array& effort, year_month& date, ivector& effort_occured);

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void dftotal_mort_comp(par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& param, par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& f_mort, d3_array& dff_mort, dmatrix& dfz);

//void df_fish_mort_comp(par_t& dfparam, d3_array& dff_mort, d3_array& effort);

template <typename DOUBLE>
void reverse(DOUBLE& like, dvector& g, const int nvar);

extern int _global_report_flag;     //Set in main(); 0 while in fmin loop else 1
extern setsaveinterflag interflag;
extern intersavetype * isp;
extern ofstream clogf;

template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
void par_t_reg<D3_ARRAY,MATRIX,VECTOR,DOUBLE>::fgcomp(double& plike, dvector& x, dvector& g, int _nvar,
       recaptype_vector& recaps, int nrec,
       coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff,
       indexed_regional_fishery_record& irfr)
{
  //clogf.setf(ios::fixed);
HERE
cout << max_fad << endl;
exit(1);
HERE
  clock_t time1 = clock();

  par_t_reg<d3_array,dmatrix,dvector,double> dfparam(*this);


  //d3_array effort = make_d3_array(1, nfleet, 1,m, jlb, jub);
  //d3_array fish_mort = make_d3_array(1, nfleet, 1,m, jlb, jub);
  //d3_array dffish_mort = make_d3_array(1, nfleet, 1,m, jlb, jub);
  //dffish_mort.initialize();
  imatrix jlbm(1,nfleet,1,m);
  imatrix jubm(1,nfleet,1,m);
  ivector ilbv(1,nfleet);
  ivector iubv(1,nfleet);
  HERE
  for (int s = 1; s <= nfleet; s++)
  {
    ilbv(s) = 1;
    iubv(s) = m;
    //jlbm(s) = jlb;
    //jubm(s) = jub;
    for (int i = 1; i <= m; i++)
    {
      jlbm(s,i) = jlb(i);
      jubm(s,i) = jub(i);
    } //end_for
  } //end_for
  d3_array effort(1, nfleet, ilbv, iubv, jlbm, jubm);
  d3_array fish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
  d3_array dffish_mort(1, nfleet, ilbv, iubv, jlbm, jubm);
  dffish_mort.initialize();
  HERE

  dmatrix tot_mort(1,m, jlb, jub);     //tot_mort.initialize();
  dmatrix dftot_mort(1,m, jlb, jub); dftot_mort.initialize();
  //Tags
  dmatrix release(1,m, jlb, jub); release.initialize();
  dmatrix dfrelease(1, m, jlb, jub); dfrelease.initialize();


  ivector effort_occured(1, nfleet); effort_occured.initialize();

#if !defined(__SUN__) && !defined(__GNUC__)
  if ( ( isp->savefile = open(isp->savefilename, O_RDWR|O_CREAT|O_BINARY,S_IREAD|S_IWRITE) ) < 0)
#else
#if defined ( __SUN__) ||  defined ( __GNUC__)
  if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0660) ) < 0)
  //if ( ( isp->savefile = open(isp->savefilename, O_RDWR | O_CREAT | O_TRUNC , 0777) ) < 0)
#else
  if ( ( isp->savefile = creat(isp->savefilename, O_RDWR) ) < 0)
#endif
#endif
  {
    cerr <<"Cannot open " << isp->savefilename <<" for output.\n";
    exit(1);
  }

  plike = 0.0;
  fpen = 0.0;
  total_uv_penalty = 0;
  total_sigma_penalty = 0;
  total_negative_tag_penalty = 0;
  total_total_tag_penalty = 0 ;
  total_bounds_penalty = 0;

  //set usergrid, mort, q
  reset(x);
  plike -= fpen;
  total_bounds_penalty += fpen;
  HERE
  int ncohort = nrelease;


  int pool_tags = m_ipar[13];
  int omit_release_month = 0;
  if (m_ipar[14])
  {
    cerr << "Omit release month option (flag 14) not currently supported." << endl;
    exit(1);
  } //end_if
  int last_rn = 0;
  int c1 = 1;
  int c2 = ncohort;

  int cohort = 0;
  if (pool_tags == 1)
  {
    c2 = 1;
  } //end_if
  else if (pool_tags == 2)
  {
    last_rn = 0;
    c1 = 1;
    c2 = ncohort;
  } //end_if

  //Use a macro for cleaner code
  ivector jlb1(0,m+1);
  ivector jub1(0,m+1);
  for (int i = 1; i <= m; i++)
  {
    jlb1(i) = jlb(i) - 1;
    jub1(i) = jub(i) + 1;
  } //end_for
  jlb1(0) = 0;
  jlb1(m+1) = 0;
  jub1(0) = n+1;
  jub1(m+1) = n+1;

  dmatrix u(0, m+1, jlb1, jub1);       //u.initialize();
  dmatrix v(0, m+1, jlb1, jub1);       //v.initialize();
  dmatrix sigma(0, m+1, jlb1, jub1);   //sigma.initialize();

  dmatrix df_u(0, m+1, jlb1, jub1); df_u.initialize();
  dmatrix df_v(0, m+1, jlb1, jub1); df_v.initialize();
  dmatrix df_sigma(0, m+1, jlb1, jub1); df_sigma.initialize();


  year_month date;
  year_month local_start_date = start_date;
  year_month save_start_date = start_date;
  year_month final_date = get_tr_date(nrelease) + nmonth;

  double dfplike = 1.0;
  double dfneg_tag_pen = 0.0;
  double dftot_tag_pen = 0.0;
  dfcoff.clean();
  g.initialize();

  for (int cc = c1; cc <= c2; cc++)
  {
    interflag.isp_reset();
    int current_season = 0;
    if (pool_tags == 2)
    {
      cohort = get_tr_cohort(cc);
      //clogf << "\nReleasing tags for cohort " << cohort << endl;
      local_start_date = get_tr_date(cc);
      final_date = local_start_date + nmonth;
      start_date = local_start_date;
      release.initialize();
      while( (last_rn < ncohort) && 
             (cohort == get_tr_cohort(last_rn+1)) )
      {
        last_rn++;
        int i = get_tr_i(last_rn);
        int j = get_tr_j(last_rn);
        release(i,j) += get_tr_tags(last_rn);
        //clogf << local_start_date <<", released "
        //      << get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      } //end_while
      cc = last_rn;
    } //end_if

    int total_months = final_date - start_date + 1;
    ivector coff_comp_called(0,total_months);
    coff_comp_called.initialize();
    int kmonth = 0;
    //clogf << "---------- starting forward loop ----------" << endl;
    for (date = local_start_date; date <= final_date; date++)
    {
      kmonth ++;
      if (pool_tags == 1)
      {
        while((last_rn < ncohort)
            && (date == get_tr_date(last_rn+1)))
        {
          last_rn++;
          int i = get_tr_i(last_rn);
          int j = get_tr_j(last_rn);
          release(i,j) += get_tr_tags(last_rn);
          //clogf << date <<", released " << get_tr_tags(last_rn) << " at "
          //      << i << "," << j << endl;
        } //end_while
      } //end_if

      if (current_season != get_season(date))
      {
        //clogf << "Season Change Forward "; TTRACE(date,get_season(date))
        current_season = get_season(date);
        uvs_comp(u, v, sigma, date);
        coff.comp(*this, u, v, sigma);
        coff_comp_called(kmonth) = 1;
        //clogf << "               season: " << get_season(date) << endl;
        //added jca
        double penlike = 0.0;
        velpen<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(*this, penlike, date);
        total_uv_penalty += penlike;
        if (penlike>0)
        {
          cerr << " Error in velpen\n";
          char tempchar;
          cin >> tempchar;
        } //end_if
        plike += penlike;

        penlike = 0.0;
        sigpen<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(*this, penlike, date);
        total_sigma_penalty += penlike;
        plike += penlike;
      } //end_if

      get_effort_array(*this, irfr, effort, date, effort_occured);
      fish_mort_comp(fish_mort, effort, date);
      total_mort_comp(fish_mort, tot_mort);
HERE
      coff.b_adjust(*this, tot_mort);

      if (omit_release_month)
      {
        omit_release_month --;
      } //end_if
      else
      {
        double plike1 = 0.0;
        switch (m_ipar[11])
        {
          case 2:
            if ( (m_ipar[27] == 0) || (m_ipar[27] == 1) )
            {
              poisson_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps, nrec, *this, tot_mort, effort_occured, fish_mort);
            } //end_if
            //
            //else if (param.ipar[27] == 2)
            //{
            //    poisson_like(plike1, psi_bar, date, cohort, recaps,
            //                 nrec, param, tot_mort, effort_occured,
            //                 fish_mort);
            //}
            else
            {
              cerr << "Unknown option (" << m_ipar[27]
                   << ") passed to fgcomp in " << __FILE__ << endl;
              exit(1);
            } //end_else
            break; 
       
          case 3:
          //{
          //    least_square_like(plike1, release, date, cohort, recaps,
          //                      nrec, param, effort, fish_mort);
          //}
            cerr << "switch[11] = 3; "
                 << "least squares likelihood not implemented" << endl;
            exit(1);
            break;
          case 4:
          case 5:
            negative_binomial_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps,
                nrec, *this, tot_mort, effort_occured,
                fish_mort,nb_par,1,m_ipar[18]);
            break;
          case 14:
          case 15:
            negative_binomial_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps,
                nrec, *this, tot_mort, effort_occured,
                fish_mort,nb_par,2,m_ipar[18]);
            break;
          case 24:
          case 25:
            negative_binomial_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps,
                nrec, *this, tot_mort, effort_occured,
                fish_mort,nb_par,nfleet,m_ipar[18]);
            break;
          case 6:
            exponential_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(plike1, release, date, cohort, recaps,
                nrec, *this, tot_mort, effort_occured,
                fish_mort);
            break;
          default:
            cerr << "Unknown likelihood function specified"
                 << "; m_ipar[11] = " << m_ipar[11]  << endl;
            exit(1);
        }
        plike += plike1;
      } //end_else
      DOUBLE neg_tag_pen = 0;
      negative_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, *this, neg_tag_pen);
      total_negative_tag_penalty += neg_tag_pen;
      plike -= neg_tag_pen;

      DOUBLE tot_tag_pen = 0;
      total_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, *this, tot_tag_pen);
      total_total_tag_penalty += tot_tag_pen;
      plike -= tot_tag_pen;

      coff.adi(release, isp);
    }                                  //End of forward month loop

    interflag.iclose(isp);

    //clogf << "\n---------- starting reverse loop ----------" << endl;

    for (date = final_date; date >= local_start_date; date--)
    {
      dfcoff.dfadi(coff, release, dfrelease, isp);

      //plike -= tot_tag_pen;
      dftot_tag_pen -= dfplike;
      //total_tag_penalty(release, param, tot_tag_pen);
      df_total_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, dfrelease, *this, dftot_tag_pen);
      //double tot_tag_pen = 0;
      dftot_tag_pen = 0.0;

      //plike -= neg_tag_pen;
      dfneg_tag_pen -= dfplike;
      //negative_tag_penalty(release, *this, neg_tag_pen, negatives);
      df_negative_tag_penalty<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(release, dfrelease, *this, dfneg_tag_pen);
      //double neg_tag_pen = 0;
      dfneg_tag_pen = 0.0;
HERE

      if (omit_release_month)
      {
        omit_release_month --;
      } //end_if
      else
      {
        switch(m_ipar[11])
        {
          case 2:
            if ( (m_ipar[27] == 0) || (m_ipar[27] == 1) )
            {
              df_poisson_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, *this, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort);
            } //end_if
            //
            //else if (param.ipar[27] == 2)
            //{
            //    df_poisson_like(dfplike, psi_bar, dfpsi_bar, date,
            //                    cohort, recaps, nrec, param, dfparam,
            //                    tot_mort, dftot_mort, effort_occured,
            //                    fish_mort, dffish_mort);
            //}
            break;
          //case 3:
            //df_least_square_like(release, date, cohort, recaps, nrec,
            //                     param, dfparam, dfrelease, dfplike,
            //                     effort, fish_mort, dffish_mort);
            //break;
          case 4:
          case 5:
            df_nb_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, *this, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, nb_par, dfparam.nb_par, 1,m_ipar[18]);
            break;
          case 14:
          case 15:
            df_nb_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, *this, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, nb_par, dfparam.nb_par, 2,m_ipar[18]);
            break;
          case 24:
          case 25:
            df_nb_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, *this, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort, nb_par, dfparam.nb_par, nfleet,m_ipar[18]);
            break;
          case 6:
            df_exp_like<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfplike, release, dfrelease, date, cohort, recaps, nrec, *this, dfparam, tot_mort, dftot_mort, effort_occured, fish_mort, dffish_mort);
            break;

          default:
            cerr << "Unknown likelihood function specified"
                 <<  "; m_ipar[11] = " << m_ipar[11]  << endl
                 << "This can't happen!" << endl;
            exit(1);
        }
      } //end_else

      //coff.b_adjust(*this, tot_mort);
      dfcoff.dfb_adjust(coff, *this, dfparam, dftot_mort);

      //total_mort_comp(fish_mort, tot_mort);
      dftotal_mort_comp<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(*this, dfparam, fish_mort, dffish_mort, dftot_mort);

      //fish_mort_comp(fish_mort, effort, date);
      df_fish_mort_comp(dfparam, dffish_mort, effort, date);

      if(coff_comp_called(kmonth))
      {
        //clogf << "Season Change Reverse "; TTRACE(date - 1,get_season(date - 1))
        //current_season = get_season(date -1);
        year_month date1 = date - 1;
        current_season = get_season(date1);

        dfsigpen<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfparam, *this, date);
        dfvelpen<D3_ARRAY,MATRIX,VECTOR,DOUBLE>(dfparam, *this, date);

        uvs_comp(u, v, sigma, date);   // recompute
        dfcoff.dfcomp(coff, *this, dfparam, u, v, sigma, date,
          df_u, df_v, df_sigma, isp);
        //clogf << "               season: " << get_season(date) << endl;

        dfuvs_comp2(*this, dfparam, df_u, df_v, df_sigma, date);

      } //end_if

      if (date > local_start_date)
      {
        year_month date1 = date - 1;
        get_effort_array(*this, irfr, effort, date1, effort_occured);
        fish_mort_comp(fish_mort, effort, date1);
        total_mort_comp(fish_mort, tot_mort);
        coff.b_adjust(*this, tot_mort);
      } //end_if

      kmonth --;
    } //End reverse month loop

    //int current_season = 0;
    if (pool_tags == 2)
    {
      //cohort = get_tr_cohort(cc);
      //local_start_date = get_tr_date(cc);
      //final_date = local_start_date + nmonth;

      //release.initialize();
      dfrelease.initialize();

      //while((last_rn < ncohort)
      //            && (cohort == get_tr_cohort(last_rn+1)))
      //{
      //  last_rn++;
      //  i = get_tr_i(last_rn);
      //  j = get_tr_j(last_rn);
      //  release(i,j) += get_tr_tags(last_rn);
      //  clogf << local_start_date <<", released "
      //        << get_tr_tags(last_rn) << " at " << i << "," << j << endl;
      //}
    } //end_if

  } //for (int cc = c1; cc <= c2; cc++)
  //End of cohort loop
HERE
  start_date = save_start_date;
  dfparam.dfreset(g, x);
  reverse(plike, g, _nvar);

  close(isp->savefile);

  if (_global_report_flag)
    resid_comp(release, release, release, 1);
HERE
  time_t time_sec;
  time(&time_sec);
  clock_t time2 = clock();
  double elapsed_time = (double)(time2-time1)/CLOCKS_PER_SEC;
  clogf << "Objective function value = " << setprecision(8) << plike
        << "; elapsed time = " << setprecision(2) << elapsed_time 
        << " seconds." << endl;
}
template void par_t_reg<d3_array,dmatrix,dvector,double>::fgcomp(double& plike, dvector& x, dvector& g, int _nvar, recaptype_vector& recaps, int nrec, coff_t<d3_array,dmatrix,dvector,double>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr);
template<> void par_t_reg<dvar3_array,dvar_matrix,dvar_vector,dvariable>::fgcomp(double& plike, dvector& x, dvector& g, int _nvar, recaptype_vector& recaps, int nrec, coff_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr) {}
