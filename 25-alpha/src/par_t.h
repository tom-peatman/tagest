//$Id: par_t.h 3208 2014-08-30 06:54:21Z jsibert $
#ifndef PAR_T_H
#define PAR_T_H

#ifndef __APPLE__
  #pragma interface
#endif
//#include "coff_t.h"
#include <fvar.hpp>
#include <cifstrem.h>
#if (__GNUC__ >=3)
  #include <iomanip>
#else
  #include <iomanip.h>
#endif
#include "ipar.h"
#include "yr_mon2.h"
#include "modlarea.h"
class indexed_regional_fishery_record;
#ifdef __hppa__
  #define SHORT short
#else
  #define SHORT short
#endif
class recaptype_vector;
class model_parameters;

typedef struct // hold tag release details
{
  year_month  date;    // date of release
  float       tags;    // number of tags released
  int   cohort;  // cohort number, depends in pooling flag
  int   release_cohort;  // tag release cohort number
  int   i;       // array x index in real world for tag release
  int   j;       // array index in real world for tag release
  int cur_rel;
} tagreleasetype;

typedef tagreleasetype tagreleasetype_;
struct tagreleasetype_vrep
{
   tagreleasetype_ *p;
   int en1;
   int en2;
   int nsize;
   int refcnt;
};

class trv_ptr;
class tagreleasetype_vector
{
   tagreleasetype_vrep *v;
   void allocate();
 public: 
   friend class tagreleasetype_matrix;
   tagreleasetype_vector(const int n1, const int n2);
   tagreleasetype_vector();
   tagreleasetype_ *base()
   {
      return v->p;
   }
   tagreleasetype_ & operator[] (const int i);
   tagreleasetype_vector & operator=(const tagreleasetype_vector & x);
   ~tagreleasetype_vector();
   inline tagreleasetype_vector(const tagreleasetype_vector & x)
   {
      x.v->refcnt++;
      v = x.v;
   }
   friend void tagreleasetype_sort(trv_ptr& tr, const int n);
};

class trv_ptr
{
public:
  tagreleasetype_vector* ptr;
  //friend class par_t;
  tagreleasetype_& operator[](const int i) { return (*ptr)[i];}
  tagreleasetype_& operator()(const int i) { return (*ptr)[i];}
};

int tagreleasetype_compare(const void* _a, const void* _b);
int tagreleasetype_compare(tagreleasetype& a, tagreleasetype& b);
void tagreleasetype_sort(trv_ptr& tr, const int n);

ostream& operator << (ostream& s, tagreleasetype_& t);
istream& operator >> (istream& s, tagreleasetype_& t);
cifstream& operator >> (cifstream& s, tagreleasetype_& t);



template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE> class coff_t;

/** Base class for model parameters.
*/
template <typename D3_ARRAY, typename MATRIX, typename VECTOR, typename DOUBLE>
class par_t : public Model_area
{
public:
  friend class model_parameters;

  static par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* instance;
  static par_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>* createInstance(const adstring &filename);
  static void deleteInstance();

public:
  adstring executable;
  
  int nmonth;   ///< number of months of tag return data
  int nrelease; ///< number of tag releases
  int last_cohort;  ///< number of (pooled) tag_releases
  int nfleet;   ///< number of fishing fleets
  int fleet0;   ///< lower index of fleet vector (depends on m_ipar(53))
  trv_ptr tr;              ///< tag releases
  //dmatrix fadmap;          ///< matrix of fadmap
  //dmatrix gx_fadmap;        ///< gradient matrix of fadmap in x
  //dmatrix gy_fadmap;        ///< gradient matrix of fadmap in y
  ivector fadmap_stratum_active_d; ///< switch fad-d parameters on/off

  // "active" parameters:
  //DOUBLE sus_spd;       ///< fish sustainable speed  
  DOUBLE chi;       ///< taxis coefficient for FAD-uv equation  
  double max_gx;       ///< maximum gradient in gx for FAD-uv equation  
  double max_gy;       ///< maximum gradient in gx for FAD-uv equation  
  double max_g;       ///< maximum gradient between max_gx and max_gy for FAD-uv equation  
  //dvector max_gx;       ///< maximum gradient in gx for FAD-uv equation  
  //dvector max_gy;       ///< maximum gradient in gx for FAD-uv equation  
  //double mean_size;     ///< mean size of fish  
  dvector max_fad;       ///< maximum number of fad
  VECTOR logslope;         ///< log(slope) for FAD-D equation
  VECTOR inflection;    ///< inflection for FAD-D equation
  VECTOR min_D;         ///< minimum diffusion rate for FAD-D equation
  DOUBLE loggamma;        ///< a parameter for exponential FAD-D equation;log(gamma)
  //VECTOR loggamma;        ///< a parameter for exponential FAD-D equation;log(gamma)
  DOUBLE mort;		///< instantaneous natural mortality
  DOUBLE special_mort;	///< instantaneous natural mortality for special region
  VECTOR q;		///< catchability coefficient by fleet
  VECTOR report_rates;	///< reporting rates

  VECTOR recapture_dev; ///< recapture (catchability, reporting) devs
  double recapture_dev_weight; ///< weight on norm2(recapture_dev) for likelihood
  DOUBLE recapture_dev_pen; ///< recapture dev penalty for likelihood

  VECTOR tag_surv_prop; ///< proportion of individuals surviving in each cohort
  double tag_surv_prop_weight; ///< weight sum of beta proability distribution
  DOUBLE tag_surv_prop_pen;    ///< tagging survival penalty for likelihood
  DOUBLE mean_tag_surv_prop;   ///< estimated mean tagging survivial proportion
  double tag_surv_prop_prior;  ///< prior for mean tagging survivial proportion

  //Parameter Bounds
  double minafm,     maxafm;
  double minsig,     maxsig;
  double minmort,    maxmort;
  double minq,       maxq;
  double minr_rates, maxr_rates;
  double minvel,     maxvel;
  //double minsus_spd,      maxsus_spd;
  double minchi,      maxchi;
  double minlogslope,   maxlogslope;
  double minmin_D,   maxmin_D;
  double minloggamma, maxloggamma;
  //double mininflection;   
  dvector maxinflection; //maxinflections are different depending on regions 

  DOUBLE fpen;

  double total_uv_penalty;
  double total_sigma_penalty;
  double total_negative_tag_penalty;
  double total_total_tag_penalty;
  double total_bounds_penalty;
  double total_likelihood_contribution;
  double objective_function_value;
  double maximum_gradient;
  VECTOR nb_par; // negative binomial parameter
  int nvar;

public://Variables
  ipar_t  m_ipar;       ///< vector m_ipars ipar[0] holds number of ipars
  adstring_array  fleet_names;  ///< vector of 4 byte fleet identifiers
  ivector DomesticFleetKludgeFlag;  ///<  if do domestic fleet kludge

  year_month  start_date;  ///< starting date
  year_month  global_start_date;  ///< starting date
  year_month  final_date;  ///< date nmonth after last release
  int duration;          ///< months from first tag release to nmonth after last release

  /** Number of "anomalous" fishing mortality events.
  \deprecated Proved to be not useful. Replaced by missing effort
  algrorithms.
  */
  int num_afm;
  ivector afm_yr;
  ivector afm_mo;
  ivector afm_fl;
  ivector afm_i;
  ivector afm_j;
  VECTOR afm;

public://Functions
  par_t();                  //par_t.cpp
  par_t(cifstream& prn);    //ppar.cpp
  par_t(const char* name, const adstring parversion);  //pparead.cpp
  par_t(const par_t<d3_array,dmatrix,dvector,double>& t);
  par_t(const par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>& t);
  virtual ~par_t();         //par_t.cpp

  void  set_fishing_effort_normalization(indexed_regional_fishery_record& irfr);

protected://Functions
  //par_t.cpp
  void allocate();
  void initialize();

public:
  // coh_collapse.cpp
  int collapse_cohorts(recaptype_vector& recaps, const int nrec);
  ///< combines tag cohorts by date of release

/* Compute fishing mortality from catchability and fishing effort.
\tparam f_mort 3D array over model domain by fleet. On return, the layers contain fishing mortality 
for each fleet.
\f$ F_{fij} = q_f*E_{fij}\f$
\param effort d3_array of observed fishing effort, \f$E_{fij}\f$ over model domain by fleet.
\param date year_month object containing the year and month of fishing effort
*/
void fish_mort_comp(D3_ARRAY& f_mort, const d3_array& effort);
//void fish_mort_comp(d3_array& f_mort, const d3_array& effort);
//void fish_mort_comp(dvar3_array& f_mort, const d3_array& effort, year_month& date);
//void fish_mort_comp(D3_ARRAY& f_mort, D3_ARRAY& f_mort0, const d3_array& effort, 
//                    year_month& date, const MATRIX& tag_density, const d3_array& recaps0, 
//                    const dvector& mean_effort);

//DOUBLE baranov_catch_f0(const DOUBLE& f1, const DOUBLE& f0, const DOUBLE& P, const DOUBLE& rr);
//DOUBLE dbaranov_catch_df0(const DOUBLE& f1, const DOUBLE& f0, const DOUBLE& P, const DOUBLE& rr);
//DOUBLE NR_baranov_f0(const double& C, const DOUBLE& f1, const DOUBLE& P, const DOUBLE& rr, const DOUBLE& init_f0);

void df_fish_mort_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, d3_array& dff_mort, d3_array& effort, year_month& date);

void total_mort_comp(D3_ARRAY& f_mort, MATRIX& z);
//void total_mort_comp(D3_ARRAY& f_mort1, D3_ARRAY& f_mort0, MATRIX& z, const int cohort);
//void total_mort_comp(dvar3_array &f_mort, dvar_matrix &z, dvariable &mort, dvariable &special_mort, const int cohort);

  //psavefit.cpp
  void savefit(ofstream& pf, const char* fullname);

void pred_recapture_comp(MATRIX& pred_recaptures, const MATRIX& pred_tags, const MATRIX& z, const D3_ARRAY& fmort, const int fleet, const year_month& date);
void df_pred_recapture_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& pred_recaptures, dmatrix& dfpred_recaptures, dmatrix& pred_tags, dmatrix& dfpred_tags, dmatrix& z, dmatrix& dfz, d3_array& fmort, d3_array& dffmort, int fleet, year_month& date);

private://Functions
  //pparead.cpp
  void read_tagrelease(cifstream& istr);
  void read_bounds(cifstream& istr);
  void read_ipar(cifstream& istr);

public://Overloaded and Virtual functions

  virtual void uvs_comp(dmatrix& u, dmatrix& v, dmatrix& sigma,
                        year_month& ym){};
  virtual int nvarcalc(){return 0;}
  virtual void xinit(dvector& x, adstring_array& xnames){};
  //virtual void reset(const VECTOR& x) = 0;
  virtual void savefit(const double _like, const int _nvar, const double gmax, const char* _ext) = 0;
  void dfparset(par_t<d3_array,dmatrix,dvector,double>& dfparam)
  {
    cout << "Error: Base class dfparset called" << endl;
    exit(1);
  }
  void make_dfpar_t(const par_t<d3_array,dmatrix,dvector,double>& t)
  {
    cout << "Error: Base class make_dfpar_t called" << endl;
    exit(1);
  }
  void dfuvs_comp(par_t<d3_array,dmatrix,dvector,double>& dfparam, dmatrix& dfu,
                  dmatrix& dfv, dmatrix& dfsigma, year_month& ym)
  {
    cout << "Error: Base class dfuvs_comp called" << endl;
    exit(1);
  }
  //virtual void fgcomp(double& plike, dvector& x, dvector& g, int nvar, recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr) = 0;
  //virtual void fgcomp2(DOUBLE& plike, VECTOR& x, dvector& g, int nvar, recaptype_vector& recaps, int nrec, coff_t<D3_ARRAY,MATRIX,VECTOR,DOUBLE>& coff, coff_t<d3_array,dmatrix,dvector,double>& dfcoff, indexed_regional_fishery_record& irfr) = 0;

virtual bool isRegional() const { return false; } 
virtual bool isNeuralNet() const { return false; } 
void set(model_parameters& mp) { }
/*
virtual dvariable computeObjectiveFunctionValue(model_parameters& parameters)
{
	std::cout << __FILE__ << ':' << __LINE__ << std::endl;
	return 0;
}

virtual void reset(const model_parameters& parameters, dvariable& plike)
{
	std::cout << "Error: " << __FILE__ << ':' << __LINE__ << std::endl;
	exit(0);
}
*/
public://Access Functions
  double get_objective_function_value()
  { return objective_function_value; }
  int get_nvar() { return nvar; }
  double get_maximum_gradient() { return maximum_gradient; }
  int get_nfleet() const { return nfleet; }
  int get_nrelease() { return nrelease; }
  int get_nmonth() { return nmonth; }
  int get_month(year_month& _date) { return _date.get_month_value(); }
  int get_no_of_ipars() { return 150; }
  float get_tr_tags(int tt) { return tr[tt].tags; }
  int get_tr_cohort(int tc) { return tr[tc].cohort; }
  int get_tr_release_cohort(int tc) { return tr[tc].release_cohort; }
  int get_tr_i(int ti) { return tr[ti].i; }
  int get_tr_j(int tj) { return tr[tj].j; }
VECTOR& get_q() { return q; }
VECTOR& get_report_rates() { return report_rates; }
DOUBLE get_mort() { return mort; }
DOUBLE get_special_mort() { return special_mort; }
DOUBLE get_q(int qi) { return q(qi); }
DOUBLE get_report_rates(const int gr) { return report_rates[gr]; }
  double get_total_bounds_penalty() { return total_bounds_penalty; }
  double get_total_uv_penalty() { return total_uv_penalty; }
  double get_total_sigma_penalty() { return total_sigma_penalty; }
  double get_total_total_tag_penalty() { return total_total_tag_penalty; }
  double get_total_negative_tag_penalty() { return total_negative_tag_penalty; }
  year_month& get_start_date() { return start_date; }
  year_month& get_tr_date(int td) { return tr[td].date; }
  //DOUBLE get_sus_spd() { return sus_spd; }
  DOUBLE get_chi() { return chi; }
  VECTOR& get_min_D() { return min_D; }
  VECTOR& get_logslope() { return logslope; }
  VECTOR& get_inflection() { return inflection; }
  //VECTOR& get_loggamma() { return loggamma; }
  DOUBLE get_loggamma() { return loggamma; }

  double get_minafm() { return minafm; }
  double get_maxafm() { return maxafm; }
  double get_minsig() { return minsig; }
  double get_maxsig() { return maxsig; }
  double get_minmort() const { return minmort; }
  double get_maxmort() const { return maxmort; }
  double get_minq() const { return minq; }
  double get_maxq() const { return maxq; }
  double get_minr_rates() { return minr_rates; }
  double get_maxr_rates() { return maxr_rates; }
  double get_minvel() { return minvel; }
  double get_maxvel() { return maxvel; }
  //double get_minsus_spd() {return minsus_spd;}
  //double get_maxsus_spd() {return maxsus_spd;}
  double get_minchi() {return minchi;}
  double get_maxchi() {return maxchi;}
  double get_minmin_D() { return minmin_D; }
  double get_maxmin_D() { return maxmin_D; }
  double get_minlogslope() { return minlogslope; }
  double get_maxlogslope() { return maxlogslope; }
  double get_minloggamma() {return minloggamma;}
  double get_maxloggamma() {return maxloggamma;}
  //double get_mininflection() { return mininflection; }
  dvector get_maxinflection() { return maxinflection; }

  //DOUBLE get_fpen() { return fpen; }
  //void set_sus_spd(DOUBLE spd) { sus_spd = spd; }
  void set_chi(DOUBLE chi) { chi = chi; }
  void set_min_D(VECTOR d) { min_D = d; }
  void set_logslope(VECTOR d) { logslope = d; }
  void set_inflection(VECTOR d) { inflection = d; }
  void set_loggamma(DOUBLE d) { loggamma = d; }
  //void set_loggamma(VECTOR d) { loggamma = d; }
  //void set_fpen(DOUBLE d) { fpen = d; }
  void set_mort(DOUBLE mt) { mort = mt; }
  void set_special_mort(DOUBLE mt) { special_mort = mt; }
  void set_nmonth(int nmt) { nmonth = nmt; }
  void set_nfleet(int nft) { nfleet = nft; }
  void set_nrelease(int nrt) { nrelease = nrt; }
void set_report_rates(const int sr, DOUBLE vr) { report_rates[sr] = vr; }
  void set_q(const int qr, DOUBLE qt) { q[qr] = qt; }
  void set_total_uv_penalty(double t) { total_uv_penalty = t; }
  void set_total_sigma_penalty(double t) { total_sigma_penalty = t; }
  void set_total_negative_tag_penalty(double t)
        { total_negative_tag_penalty = t; }
  void set_total_total_tag_penalty(double t) { total_total_tag_penalty = t; }
  void set_total_bounds_penalty(double t) { total_bounds_penalty = t; }
  void set_tr(int cc, tagreleasetype_& rr) { tr[cc] = rr; }
void add_total_bounds_penalty(const double penalty) { total_bounds_penalty += penalty; }
void add_total_bounds_penalty(const dvariable penalty) { total_bounds_penalty += value(penalty); }
void add_total_uv_penalty(const double penalty) { total_uv_penalty += penalty; }
void add_total_uv_penalty(const dvariable penalty) { total_uv_penalty += value(penalty); }
void add_total_sigma_penalty(const double penalty) { total_sigma_penalty += penalty; }
void add_total_sigma_penalty(const dvariable penalty) { total_sigma_penalty += value(penalty); }
void add_total_negative_tag_penalty(const dvariable penalty) { total_negative_tag_penalty += value(penalty); }
void add_total_negative_tag_penalty(const double penalty) { total_negative_tag_penalty += penalty; }
void add_total_total_tag_penalty(const dvariable penalty) { total_total_tag_penalty += value(penalty); }
void add_total_total_tag_penalty(const double penalty) { total_total_tag_penalty += penalty; }
};

#ifndef __APPLE__
template class par_t<d3_array,dmatrix,dvector,double>;
template class par_t<dvar3_array,dvar_matrix,dvar_vector,dvariable>;
#endif

#endif //#ifdef PAR_T_H //
