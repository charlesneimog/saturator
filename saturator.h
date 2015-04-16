#ifndef SATURATOR_H_
#define SATURATOR_H_

static t_class *saturator_class;

 typedef struct saturator {
 	t_object x_obj;
  } t_saturator;


typedef struct _saturator_quant {		//structure used to calculate the
										//the factor used for the attack
										//and release settings.
	t_float duration;					//duration in ms.
	double inc_rmspms; 					//rms/ms, used to calculat new factor.
										//multiplied by time since passing
										// the threshold and +/- to factor. NOTE_4
										//calculated by  (1-thresh)/duration. NOTE_2
	
	t_float param;						//used as a possible parameter for evaluation function.
										//used only in pow function. NOTE_3
	double (*eval)(void*);				//Evaluation function. Used to shape the attack
										// and release.
										//1. Linear: maintains the factor as is. NOTE_5
										//2. Pow: thresh + (1-thresh)[(factor-thresh)/(1-thresh) ] ^ param . NOTE_6
										//3. Cos: thresh + (1-thresh) * [0.5 + 0.5 * cos((factor-thresh)pi/(1-thresh) - pi)] . NOTE_7
} t_saturator_quant;

typedef struct _saturator_tilde
{
	//header
	t_object x_obj;

	
	t_float sthresh;				//stored threshold in rms (0.0-1.0)
	t_float s1_thresh;				//1 - stored threshold
	t_float thresh;					//work threshold in rms (0.0-1.0)
									//at 1.0 compression is disabled. NOTE_1
									//non-linear function is derived by finding
									// the n-polynomial function that is tangent
									// with y=x at thresh and where it has a maximum (peak)
									// at x=1.0
	t_saturator_quant* attack;		//structure with attack data.
	t_saturator_quant* release;		//structure with release data.
	t_saturator_quant* current;		//structure currently being executed.
									//set to attack when signal passes above threshold. NOTE_8
									//set to release when signal drops below threshold. NOTE_9
	
	int cheap;						//if set to 1 then cheap time-dependent algorithm will be used
	int push;						//if set to 1 then compression will be performed on
									//	negative portion of the signal. NOTE_10
	int pull;						//if set to 1 then compression will be performed on
									//	positive portion of the signal. NOTE_11
	int order;						//2 for quadratic function, 3 for cubic function. NOTE_12
	int isvu;						//if set to 1 then x_obj will be assumed to have a 
									// 2nd outlet signal with current thresh data.
	int isthresh;					//if set to 1 then x_obj will be assumed to have a 
									// 2nd outlet signal with attenuation data.
									//only applicable where cheap is false.
	
	int enableAutogain;				//if set to 1 autogain is enabled (default).

	/*even N-polynomial function:
	 * F(x)  = -1r(x-1)^N +(1.0 -c)						NOTE_13
	 * F with autogain(x) = F(x)/autoGain
	 * where
	 * 	r = -1 / [N(thresh - 1)^(N-1)]					NOTE_14
	 * 	c = -1r * (thresh - 1.0)^ N - thresh + 1		NOTE_15
	 * 	autoGain =  1 - c								NOTE_16
	 */
	/*cubic function:
	 * F(x) = r(x-1.0)^N +(1 +c)						NOTE_17
	 * F with autogain(x) = F(x)/autogain
	 * where
	 * 	r = 1 / [N(thresh - 1)^ (N-1)]					NOTE_18
	 *  c = thresh - [r(x-1.0)^N+1]						NOTE_19
	 * 	autoGain =  1 + c								NOTE_20
	 */
	double r;				
	double c;
	double autoGain;

	double factor;	//place holder for factor. starts at 1. NOTE_21
	double key;	//used to hold time since last calculation of factor. NOTE_22 
    
} t_saturator_tilde;


void saturator_tilde_setup(void);
void saturator_vu_tilde_setup(void);
void saturator_thresh_tilde_setup(void);
void *saturator_tilde_new(t_floatarg f);
void saturator_tilde_free(t_saturator_tilde *x);

void saturator_pushpull(t_saturator_tilde *x);
void saturator_push(t_saturator_tilde *x);
void saturator_pull(t_saturator_tilde *x);
void saturator_attack(t_saturator_tilde *x, t_symbol *s, int argc, t_atom *argv);
void saturator_release(t_saturator_tilde *x, t_symbol *s, int argc, t_atom *argv);
void saturator_cheap(t_saturator_tilde *x, t_float f);
void saturator_order(t_saturator_tilde *x, t_float f);
void saturator_autogain(t_saturator_tilde *x, t_float f);
void saturator_threshold(t_saturator_tilde *x, t_float thresh);
void saturator_dsp(t_saturator_tilde *x, t_signal **sp);
void saturator_debug(t_saturator_tilde *x);

double saturator_td_linear(void *x);
double saturator_td_pow(void *x);
double saturator_td_cos(void *x);


void saturator_tilde_addmethods(t_class *sat_class);


#endif /*SATURATOR_H_*/
