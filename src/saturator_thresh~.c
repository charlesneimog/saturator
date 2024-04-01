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
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include "saturator.h"

#define FOREACH(i, size) for(i = 0; i < size; i++)



/* ---------------------------- Pure Data ------------------------ */
#ifdef PD

static t_class *saturator_thresh_tilde_class;




static void *saturator_thresh_tilde_new(t_floatarg f)
{
    t_saturator_tilde *x = (t_saturator_tilde *)saturator_tilde_new(f);
    x->cheap=0;
    x->isthresh=1;
	outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

void saturator_thresh_tilde_setup(void)
{
	saturator_thresh_tilde_class = class_new(gensym("saturator_thresh~"), (t_newmethod)saturator_thresh_tilde_new, (t_method)saturator_tilde_free,
        sizeof(t_saturator_tilde), 0, A_DEFFLOAT, 0);
	saturator_tilde_addmethods(saturator_thresh_tilde_class);

    class_sethelpsymbol(saturator_thresh_tilde_class, gensym("help-saturator~.pd"));
}
#endif /* PD */

