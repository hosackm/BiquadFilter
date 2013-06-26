#include "Biquad.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.141592654)
#endif

biquad* bq_new(int filter_type,
				float frequency,
				float Q,
				float dbGain,
				int sample_rate){

	biquad* tmp = (biquad*)malloc(sizeof(biquad));
	
	if (tmp == NULL){
		printf("Error while allocating memory for biquad");
		return tmp;
	}

// Calculate helper variables for
// generating 'a' and 'b' coefficients
//////////////////////////////////////
	float A = pow(10, dbGain / 40); //convert to db
    float omega = 2 * M_PI * frequency / sample_rate;
    float sn = sin(omega);
    float cs = cos(omega);
    float alpha = sn / (2*Q);
    float beta = sqrt(A + A);
// Load 'a' and 'b' coefficients
// into tmp biquad
/////////////////////////////////
	bq_load_coefficients(tmp, filter_type,
						A, omega,
						sn, cs,
						alpha, beta);
//Scale coeffs to a0
////////////////////
	// tmp->a1 = (tmp->a1) / (tmp->a0);
	// tmp->a2 = (tmp->a2) / (tmp->a0);
	// tmp->b0 = (tmp->b0) / (tmp->a0);
	// tmp->b1 = (tmp->b1) / (tmp->a0);
	// tmp->b2 = (tmp->b2) / (tmp->a0);
	tmp->a1 /= (tmp->a0);
	tmp->a2 /= (tmp->a0);
	tmp->b0 /= (tmp->a0);
	tmp->b1 /= (tmp->a0);
	tmp->b2 /= (tmp->a0);

// Load rest of data
/////////////////////////////////
	tmp->prev_input_1 = 0.0;
	tmp->prev_input_2 = 0.0;
	tmp->prev_output_1 = 0.0;
	tmp->prev_output_2 = 0.0;

	return tmp;
}

float bq_process(biquad* bq, float input){
	float output = 	(bq->b0 * input) +
					(bq->b1 * bq->prev_input_1) +
					(bq->b2* bq->prev_input_2) -
					(bq->a1* bq->prev_output_1) -
					(bq->a2* bq->prev_output_2);
	bq->prev_input_2 = bq->prev_input_1;
	bq->prev_input_1 = input;
	bq->prev_output_2 = bq->prev_output_1;
	bq->prev_output_1 = output;
	//update last samples...
	return output;
}

void bq_destroy(biquad* bq){
	free(bq);
}

void bq_print_info(biquad* bq){
	printf("A0: %.8f\n",bq->a0);
	printf("A1: %.8f\n",bq->a1);
	printf("A2: %.8f\n",bq->a2);
	printf("B0: %.8f\n",bq->b0);
	printf("B1: %.8f\n",bq->b1);
	printf("B2: %.8f\n",bq->b2);
	printf("TYPE: %s\n",bq->type);
	printf("Last input: %.8f\n",bq->prev_input_1);
	printf("Second to last input: %.8f\n",bq->prev_input_2);
	printf("Last output: %.8f\n",bq->prev_output_1);
	printf("Second to last output: %.8f\n",bq->prev_output_2);
}

void bq_load_coefficients(biquad* bq, int filter_type,
						float A, float omega,
						float sn, float cs,
						float alpha, float beta){

	switch (filter_type){
		case LOWPASS:
			bq->b0 = (1.0 - cs) /2.0;
    		bq->b1 = 1.0 - cs;
    		bq->b2 = (1.0 - cs) /2.0;
    		bq->a0 = 1.0 + alpha;
    		bq->a1 = -2.0 * cs;
    		bq->a2 = 1.0 - alpha;
    		bq->type = "LOWPASS";
			break;

		case HIGHPASS:
			bq->b0 = (1 + cs) /2.0;
    		bq->b1 = -(1 + cs);
    		bq->b2 = (1 + cs) /2.0;
    		bq->a0 = 1 + alpha;
    		bq->a1 = -2 * cs;
    		bq->a2 = 1 - alpha;
    		bq->type = "HIGHPASS";
			break;

		case BANDPASS:
			bq->b0 = alpha;
    		bq->b1 = 0;
    		bq->b2 = -alpha;
    		bq->a0 = 1 + alpha;
    		bq->a1 = -2 * cs;
    		bq->a2 = 1 - alpha;
    		bq->type = "BANDPASS";
			break;
		
		case NOTCH:
			bq->b0 = 1;
    		bq->b1 = -2 * cs;
    		bq->b2 = 1;
    		bq->a0 = 1 + alpha;
    		bq->a1 = -2 * cs;
    		bq->a2 = 1 - alpha;
    		bq->type = "NOTCH";
			break;
		
		case PEAK:
			bq->b0 = 1 + (alpha * A);
    		bq->b1 = -2 * cs;
    		bq->b2 = 1 - (alpha * A);
    		bq->a0 = 1 + (alpha /A);
    		bq->a1 = -2 * cs;
    		bq->a2 = 1 - (alpha /A);
    		bq->type = "PEAK";
			break;
		
		case LOWSHELF:
			bq->b0 = A * ((A + 1) - (A - 1) * cs + beta * sn);
    		bq->b1 = 2 * A * ((A - 1) - (A + 1) * cs);
    		bq->b2 = A * ((A + 1) - (A - 1) * cs - beta * sn);
    		bq->a0 = (A + 1) + (A - 1) * cs + beta * sn;
    		bq->a1 = -2 * ((A - 1) + (A + 1) * cs);
    		bq->a2 = (A + 1) + (A - 1) * cs - beta * sn;
    		bq->type = "LOWSHELF";
			break;
		
		case HIGHSHELF:
			bq->b0 = A * ((A + 1) + (A - 1) * cs + beta * sn);
    		bq->b1 = -2 * A * ((A - 1) + (A + 1) * cs);
    		bq->b2 = A * ((A + 1) + (A - 1) * cs - beta * sn);
    		bq->a0 = (A + 1) - (A - 1) * cs + beta * sn;
    		bq->a1 = 2 * ((A - 1) - (A + 1) * cs);
    		bq->a2 = (A + 1) - (A - 1) * cs - beta * sn;
    		bq->type = "HIGHSHELF";
			break;

		default:
			break;
	}
}