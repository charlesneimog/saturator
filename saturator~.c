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
#endif
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include "saturator.h"
#define ISREADY(x) (x->ready != 0)
#define READY(x)	x->ready=1
#define NOTREADY(x)	x->ready=0
#define FOREACH(i, size) for(i = 0; i < size; i++)


#define SETORDER(x, o) (x)->order = o

#define ISORDER(x, o) ((x)->order == o)

/*
 * polynomial tangents with y=x where there's a maximum at x=1
 * 
 * for N even:
 * 	f(x) = -1r(x-1)^n+1-c
 * 	autogain = 1-c
 * 
 * for N odd:
 * 	f(x) = r(x-1)^n+1+c
 * 	autogain = 1+c
 * 
 * set h = thresh
 * f'(h) = 1 and f(h) = h
 * 
 * solve for r and c
 * 
 * for N even:
 * r = -1/ [N (x-1)^(N-1) ]
 * c= -r(h-1)^N+1-h
 * 
 * for N odd:
 * r = 1/ [N (x-1)^(N-1) ]
 * c= h - [ r(h-1)^N+1 ]
 * 
 */




#define ABS(x) ((x)<0.0f?-1.0f*x:x)

#define GR   1.6180339887
//((1.0+sqrt(5.0))/2.0)
#define RES 100
#define FIBON(x) ( (pow(GR, (double)x)-pow(-1.0 * GR, -1.0* x))/sqrt(5.0) )

#ifdef DEBUG
#define IFDEBUG(x) x
#endif
#ifndef DEBUG
#define IFDEBUG(x)
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------- Pure Data ------------------------ */
#ifdef PD

#define ISFLOAT(a) a.a_type==A_FLOAT
#define ISSYMBOL(a) a.a_type==A_SYMBOL

#define GETSTRING(s) (s)->s_name



static t_class *saturator_tilde_class;




static void saturator_up(t_saturator_tilde *x) {
	if(x->attack->duration == 0.0)
		x->attack->inc_rmspms = 1.0-x->sthresh;
	else //irrelevant, with duration=0 factor is manually set
		x->attack->inc_rmspms = (1.0-x->sthresh) / x->attack->duration; //NOTE_2
	//x->quant=x->thresh;
	IFDEBUG(post("up %f",  x->attack->inc_rmspms);)
}
static void saturator_down(t_saturator_tilde *x) {
	if(x->release->duration == 0.0)
		x->release->inc_rmspms = 1.0-x->sthresh;
	else //irrelevant, with duration=0 factor is manually set
		x->release->inc_rmspms = (1.0-x->sthresh) / x->release->duration; //NOTE_2
	//x->quant=x->thresh;
	IFDEBUG(post("down %f",  x->release->inc_rmspms);)
}

void saturator_work_threshold(t_saturator_tilde *x, t_float thresh) {
	if(thresh > 0.0 && thresh <= 1.0)
		x->thresh = thresh;
	if(x->order % 2 == 0) {
		//-r(x-1)^n +1 -c
		x->r = -1.0f / (x->order * powf(x->thresh-1.0f, x->order-1.0f)); //NOTE_14
		x->c = -1.0f * x->r * powf(x->thresh-1.0f, x->order)+1.0f - x->thresh; //NOTE_15
	} else {
		//r(x-1)^n +1 +c
		x->r = 1.0f / (x->order * powf(x->thresh-1.0f, x->order-1.0f)); //NOTE_18
		x->c = x->thresh - ( x->r * powf(x->thresh-1.0f, x->order)+1.0f ); //NOTE_19
	}
	IFDEBUG(post("thresh %f, r: %f, c: %f, ag: %f", x->thresh, x->r, x->c, x->autoGain);)
}


//primary MSP funcs

static t_int *saturator_perform(t_int *w)
{
	t_saturator_tilde *x = (t_saturator_tilde *)(w[1]);
	t_float *in = (t_float *)(w[2]);
	t_float *out = (t_float *)(w[3]);

	t_float *vu;
	t_float *thresh_out;
	long n;
	if(x->isvu == 1) {
		vu = (t_float *)(w[4]);
		n = w[5];
	}  else if(x->isthresh == 1) {
		thresh_out = (t_float *)(w[4]);
		n = w[5];
	} else {
		vu=0;
		n = w[4];
	}

	if(x->sthresh < 1.0)	//NOTE_1 
	{
		t_float j;
		t_float size = 1.0 - x->thresh;
		
		while(n--) {
			double quantPow;
			if(*in > 1.0)
				*out = 1.0;
			else if(*in < -1.0)
				*out = -1.0;
			else 
				*out = *in;
	
	
			if(ABS(*out) >= x->sthresh)  {
				if(x->factor < 1.0 && x->attack->duration > 0.0)
					x->factor += x->attack->inc_rmspms * clock_gettimesince(x->key); //NOTE_4
				if(x->factor > 1.0 || (x->attack->duration == 0.0 && x->factor != 1.0))
					x->factor=1.0;
				x->current = x->attack; //NOTE_8
			} else {
				if(x->factor > x->sthresh && x->release->duration > 0.0)
					x->factor -= x->release->inc_rmspms * clock_gettimesince(x->key); //NOTE_4
				if(x->factor < x->sthresh || (x->release->duration == 0.0 && x->factor != x->sthresh))
					x->factor = x->sthresh;
				x->current = x->release; //NOTE_9
			}
			quantPow = x->current->eval(x); //pow(x->quant, 0.5);
			if(x->cheap == 1)
				*out = *out * quantPow;
			else {
				t_float newthresh  = x->sthresh+(1.0-quantPow);
				if(newthresh != x->thresh)
					saturator_work_threshold(x, x->sthresh+(1.0-quantPow));
			}
			if(ABS(*out) > x->thresh)  {
				t_float y = *out;
				if(y > 0.0) { //NOTE_11
					if(x->pull==1) {
						if(x->order % 2 == 0)  //NOTE_13
							y = -1.0f *x->r * powf(y-1.0f, x->order)+1.0f - x->c;
						else  //NOTE_17
							y = x->r * powf(y-1.0f, x->order)+1.0f + x->c;
					}
				} else if(y < 0.0) { //NOTE_10
					if(x->push==1) {
						if(x->order % 2 == 0)  //NOTE_13
							y = x->r * powf(-1.0f*y-1.0f, x->order)-1.0f + x->c;
						else  //NOTE_17
							y = -1.0f*x->r * powf(-1.0f*y-1, x->order)-1.0f - x->c;
					}
				}
				if(x->isvu == 1) {
					*vu = ABS(*out-y);
					*vu++;
				} else if(x->isthresh == 1) {
					*thresh_out = x->thresh;
					*thresh_out++;
				}
				*out=y;
			} else if(x->isvu == 1) {
				*vu = 0.0f;
				*vu++;
			} else if(x->isthresh == 1) {
				*thresh_out = 1.0f;
				*thresh_out++;
			}
			if(x->cheap == 1)
				*out = (x->enableAutogain==1 ?*out / x->autoGain: *out) / quantPow;
			else if(x->enableAutogain==1)
				*out = *out / x->autoGain;
			
			*out++;
			*in++;
			x->key = clock_getlogicaltime(); //NOTE_22
		}
	} else if(x->isvu == 1) {
		while(n--) {
			*vu=0.0f;
			*vu++;
			*out++;
			*in++;
		}
		x->key = clock_getlogicaltime(); //NOTE_22
	} else if(x->isthresh == 1) {
		while(n--) {
			*thresh_out=1.0f;
			*thresh_out++;
			*out++;
			*in++;
		}
		x->key = clock_getlogicaltime(); //NOTE_22
	} else {
		while(n--) {
			*out++;
			*in++;
		}
		x->key = clock_getlogicaltime(); //NOTE_22
	}
		
	//if(x->quant < 1.0)
	//	IFDEBUG(post("q %f", x->quant);)
	return w + (5+x->isvu+x->isthresh);
}


void saturator_dsp(t_saturator_tilde *x, t_signal **sp)
{
	if(x->isvu==1 || x->isthresh==1)
		dsp_add(saturator_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);	
	else
		dsp_add(saturator_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}


/*static void saturator_table(t_saturator *x) {
	t_float j;
	unsigned long i;
	
	NOTREADY(x);
	FOREACH(i, BINS) {
		j = (i / HALFBINS - 1);
		x->table[i] = pow(x->level, j + x->thresh);
		if(x->table[i] > 1.0)
			x->table[i]=1.0;
		x->table[i] = j;
	}
	READY(x);
}*/


void saturator_threshold(t_saturator_tilde *x, t_float thresh) {
	if(thresh > 0.0 && thresh <= 1.0)
		x->sthresh=thresh;
	x->s1_thresh = 1.0f-x->sthresh;
	saturator_work_threshold(x, thresh);
	if(x->order % 2 == 0) {
		x->autoGain = (x)->sthresh == 1.0? 1.0 : 1.0 - (x)->c; //NOTE_16
	} else {
		x->autoGain = (x)->sthresh == 1.0? 1.0 : 1.0 + (x)->c; //NOTE_20
	}
	saturator_up(x);
	saturator_down(x);
}
void saturator_order(t_saturator_tilde *x, t_float f) {
	if(f  >= 2 && f <=736) {
		SETORDER(x, (int)f);
		saturator_threshold(x, x->sthresh);
	}
	IFDEBUG(post("order %d",  x->order);)
}
void saturator_cheap(t_saturator_tilde *x, t_float f) {
	if(x->isthresh != 1) {
		x->cheap = f == 0.0? 0: 1;
		saturator_order(x, (t_float)x->order);
	}
}

void saturator_autogain(t_saturator_tilde *x, t_float f) {
	x->enableAutogain = f == 0.0? 0: 1;
}


static inline void saturator_attackf(t_saturator_tilde *x, t_float attack) {
	x->attack->duration = attack <= 1.0? 0.0: attack;
	IFDEBUG(post("attack %f", x->attack);)
	saturator_up(x);
}

static inline void saturator_releasef(t_saturator_tilde *x, t_float release) {
	x->release->duration = release <= 1.0? 0.0: release;
	IFDEBUG(post("release %f", x->release->duration);)
	saturator_down(x);
}

static void saturator_quant_set(t_saturator_quant *q, t_symbol * type, t_atom *param1) {
	if(strcmp(GETSTRING(type), "linear") == 0) {
		q->eval = &saturator_td_linear;
	} else if(strcmp(GETSTRING(type), "pow") == 0) {
		if(param1 != NULL) {
			q->eval = &saturator_td_pow;
			q->param = atom_getfloat(param1);
		} else {
			error("saturator~: Mising param 1 for quant type 'pow'");
		}
	} else if(strcmp(GETSTRING(type), "cos") == 0) {
		q->eval = &saturator_td_cos;
	}
}

void saturator_attack(t_saturator_tilde *x, t_symbol *s, int argc, t_atom *argv) {
	if(argc > 0 && ISSYMBOL(argv[0])) {
		saturator_quant_set(x->attack, atom_getsymbol(&argv[0]), argc > 1 ? &argv[1]: NULL);
	} else if(argc > 0 && ISFLOAT(argv[0])) {
		saturator_attackf(x, atom_getfloat(&argv[0]));
	}
}
void saturator_release(t_saturator_tilde *x, t_symbol *s, int argc, t_atom *argv) {
	if(argc > 0 && ISSYMBOL(argv[0])) {
		saturator_quant_set(x->release, atom_getsymbol(&argv[0]), argc > 1 ? &argv[1]: NULL);
	} else if(argc > 0 && ISFLOAT(argv[0])) {
		saturator_releasef(x, atom_getfloat(&argv[0]));
	}
}

void saturator_pushpull(t_saturator_tilde *x) {
	x->push=1;
	x->pull=1;
}
void saturator_push(t_saturator_tilde *x) {
	x->push=1;
	x->pull=0;
}
void saturator_pull(t_saturator_tilde *x) {
	x->push=0;
	x->pull=1;
}


void saturator_debug(t_saturator_tilde *x) {
	post("THRESH: %f, ATTACK: %f, UP: %f, RELEASE: %f, DOWN: %f, QUANT: %f", x->thresh, x->attack->duration, x->attack->inc_rmspms, x->release->duration, x->release->inc_rmspms, x->factor);
}

void saturator_tilde_free(t_saturator_tilde *x) {
	freebytes(x->attack, sizeof(t_saturator_quant));
	freebytes(x->release, sizeof(t_saturator_quant));
}


void *saturator_tilde_new(t_floatarg f)
{
	unsigned int i;
    t_saturator_tilde *x = (t_saturator_tilde *)pd_new(saturator_tilde_class);

    //zero out the struct, to be careful (takk to jkclayton)
    if (x) { 
        for(i=sizeof(t_object);i<sizeof(t_saturator_tilde);i++)  
                ((char *)x)[i]=0; 
	} 
    x->thresh=1;
    x->isvu=0;
    x->isthresh=0;
    x->cheap=1;
    x->enableAutogain = 1;
    x->attack = getbytes(sizeof(t_saturator_quant));
    x->current = x->attack;
    x->release = getbytes(sizeof(t_saturator_quant));
    
    x->attack->eval = &saturator_td_linear;
    x->attack->param=1.0;
    x->release->eval = &saturator_td_linear;
    x->release->param=1.0;
    
	IFDEBUG(
	FOREACH(i, 10)
		post("%f\n", FIBON(i));
	)

	x->order = 2; //default: quadratic
	saturator_threshold(x, 1.0);	
	x->factor=1.0; //NOTE_21
	saturator_attackf(x, 0.0);	
	saturator_releasef(x, 0.0);	
	x->key=clock_getlogicaltime();
	saturator_pushpull(x);


	
	outlet_new(&x->x_obj, gensym("signal"));

    
    return (x);
}

void saturator_tilde_setup(void)
{
	saturator_tilde_class = class_new(gensym("saturator~"), (t_newmethod)saturator_tilde_new, (t_method)saturator_tilde_free,
        sizeof(t_saturator_tilde), 0, A_DEFFLOAT, 0);
	saturator_tilde_addmethods(saturator_tilde_class);

    class_sethelpsymbol(saturator_tilde_class, gensym("help-saturator~.pd"));
}
#endif /* PD */

