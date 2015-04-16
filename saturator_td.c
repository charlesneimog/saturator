/**
 
 
Apache License 2.0

bandlimited~
    Copyright [2015] Paulo Casaes

      This product includes software developed at
      Github (https://github.com/pcasaes/saturator).
 
 -- 
 https://github.com/pcasaes/saturator
 mailto:pcasaes@gmail.com
 
 v 0.3.3
 */

#ifdef PD
#include "m_pd.h"
#endif

#define PI 3.14159265358979323846

#include "saturator.h"


#include <math.h>

//NOTE_5
double saturator_td_linear(void *x) {
	return ((t_saturator_tilde*)x)->factor;
}
//NOTE_6
double saturator_td_pow(void *x) {
	t_saturator_tilde *xx = ((t_saturator_tilde*)x);
	return pow((xx->factor-xx->sthresh)/xx->s1_thresh, xx->current->param) * xx->s1_thresh+xx->sthresh; //NOTE_3
}
//NOTE_7
double saturator_td_cos(void *x) {
	t_saturator_tilde *xx = ((t_saturator_tilde*)x);
	return xx->sthresh + xx->s1_thresh * (cos((xx->factor-xx->sthresh) * PI / xx->s1_thresh - PI)/2.0 + .5);
}
