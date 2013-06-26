// Bi-Quad Module

typedef struct _biquad{
	float a0;
	float a1;
	float a2;
	float b0;
	float b1;
	float b2;
	float prev_input_1;
	float prev_input_2;
	float prev_output_1;
	float prev_output_2;
	char* type;
}biquad;

typedef enum {
	LOWPASS 	= 0,
	HIGHPASS 	= 1,
	BANDPASS 	= 2,
	NOTCH		= 3,
	PEAK		= 4,
	LOWSHELF	= 5,
	HIGHSHELF	= 6,
} FILTER_TYPES;

biquad* bq_new(int filter_type,
				float frequency,
				float q,
				float dbGain,
				int sample_rate);

float bq_process(biquad* bq, float input);

void bq_destroy(biquad* bq);

void bq_print_info(biquad* bq);

void bq_load_coefficients(biquad* bq, int filter_type,
						float A, float omega,
						float sn, float cs,
						float alpha, float beta);