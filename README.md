BiquadFilter
============

Biquad Filter implementation in C using Portaudio

# Summary
This is a biquad data structure that can be used to create audio filters such as lowpass, highpass, notch, peak, etc.
There is also an example using Portaudio to test the biquad. The example uses the following diagram:

![I/O Block Diagram](http://i.imgur.com/zLmBSEN.jpg)

# Usage
You must include "Biquad.h" in your source code to be able to use the biquad object.  There are 4 helper functions provided
when including "Biquad.h":

####biquad* bq_new(int filter_type, float frequency, float q, float dbGain, int sample_rate);
This function will allocate and initialize a new biquad object.

####float bq_process(biquad* bq, float input);
This function is used to do the filtering on your input signal.  'input' is expected to be a float value between -1.0 and 1.0
You must include a pointer to your biquad when calling this function.  The example uses this function to apply a
lowpass filtering effect on the microphone (input) signal

####void bq_print_info(biquad* bq);
This function prints out information about the biquad the parameter bq points to ie: A & B coeffectients 
and Filter type ('lowpass' for example)

####void bq_destroy(biquad* bq);
This function is used to free the biquad the parameter bq points to from memory

