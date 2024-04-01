/**


Apache License 2.0

bandlimited~
    Copyright [2015] Paulo Casaes

      This product includes software developed at
      Github (https://github.com/pcasaes/saturator).

 --
 https://github.com/pcasaes/saturator
 mailto:pcasaes@gmail.com

 v 0.3.4
 */

#ifdef PD
#include "m_pd.h"

#include "saturator.h"

void saturator_tilde_addmethods(t_class *sat_class) {
  class_addmethod(sat_class, nullfn, gensym("signal"), A_NULL);
  class_addmethod(sat_class, (t_method)saturator_dsp, gensym("dsp"), A_NULL);
  class_addmethod(sat_class, (t_method)saturator_threshold, gensym("thresh"),
                  A_FLOAT, 0);
  class_addmethod(sat_class, (t_method)saturator_attack, gensym("attack"),
                  A_GIMME, 0);
  class_addmethod(sat_class, (t_method)saturator_release, gensym("release"),
                  A_GIMME, 0);
  class_addmethod(sat_class, (t_method)saturator_order, gensym("order"),
                  A_FLOAT, 0);
  class_addmethod(sat_class, (t_method)saturator_cheap, gensym("cheap"),
                  A_FLOAT, 0);
  class_addmethod(sat_class, (t_method)saturator_autogain, gensym("autogain"),
                  A_FLOAT, 0);
  class_addmethod(sat_class, (t_method)saturator_pushpull, gensym("pushpull"),
                  0);
  class_addmethod(sat_class, (t_method)saturator_push, gensym("push"), 0);
  class_addmethod(sat_class, (t_method)saturator_pull, gensym("pull"), 0);
  class_addmethod(sat_class, (t_method)saturator_debug, gensym("debug"), 0);
}

static void *saturator_new(void) {
  t_saturator *x = (t_saturator *)pd_new(saturator_class);
  return (x);
}

#ifdef NT
__declspec(dllexport)
#endif
    void saturator_setup(void) {
  saturator_class = class_new(gensym("saturator"), (t_newmethod)saturator_new,
                              0, sizeof(t_saturator), 0, 0);
  class_sethelpsymbol(saturator_class, gensym("help-saturator~.pd"));

  saturator_tilde_setup();
  saturator_vu_tilde_setup();
  saturator_thresh_tilde_setup();

  post("saturator: saturator~, saturator_vu~, saturator_thresh~");
}
#endif /* PD */
