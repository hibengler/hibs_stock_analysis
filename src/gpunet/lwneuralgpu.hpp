

struct gpu_layer_ts {
  int no_of_neurons;
  int no_of_neurons_plus_1;
  
  ::brook::stream neuron_learning_rate_plus_bias;
  /* Normally 1.  But could go less for stubborn neurons -- i.e. neurons that
    should not be changed much.
    */
  ::brook::stream neuron_learning_rate;
  /* No bias record - a domain of neuron_learning_rate_plus_bias
    */
  ::brook::stream neuron_output_plus_bias;
  /* used by learn, compute, learn4, compute4
    float[1 or 4] 0..number of nuerons +1 - holds all the neuron outputs plus the bias neuron
    which is either 0 or 1 
    */
  ::brook::stream neuron_output;
  /* used by learn, compute, learn4, compute4
    float[1 or 4] 0..number of nuerons +1 - holds all the neuron outputs.
    Note -- if this is an input lkayer,  then this is actually the INPUTS from 
    the outside system.
    Note that this stream overlapps the neuron_output_plus_bias stream
     */
  
  ::brook::stream upper_lower_weight;
  /* used by learn, compute learn4, compute4 */
  ::brook::stream upper_lower_delta;

  ::brook::stream activation_transformed; /* activation from summing the prior layer
                          this is traversed the wrong way */
  ::brook::stream  output_transformed; /* output from activating activation */
  ::brook::stream  upper_lower_temp;


  ::brook::stream error_temp_plus_bias;
  ::brook::stream error_temp;
  ::brook::stream neuron_error_plus_bias;
  ::brook::stream neuron_error;
  ::brook::stream neuron_error_transformed;
  ::brook::stream derivitave;
  ::brook::stream derivitave_plus_bias;
  
  ::brook::stream neuron_temp_plus_bias_float1;
  ::brook::stream neuron_temp_float1;
  ::brook::stream neuron_temp_plus_bias_float2;
  ::brook::stream neuron_temp_float2;
  ::brook::stream neuron_temp_plus_bias_float3;
  ::brook::stream neuron_temp_float3;
  ::brook::stream neuron_temp_plus_bias_float4;
  ::brook::stream neuron_temp_float4;
  ::brook::stream upper_lower_temp1_float;
  ::brook::stream upper_lower_temp2_float;
  ::brook::stream upper_lower_temp3_float;
  ::brook::stream upper_lower_temp4_float;
  int activation;
};


struct gpu_network_ts {
  struct network_ts *cpu_net; /* the cpu network */
  struct network_ts *cpu_net2; /* the cpu network */
  struct network_ts *cpu_net3; /* the cpu network */
  struct network_ts *cpu_net4; /* the cpu network */
  int no_of_layers;
  int no_of_patterns;
  float momentum;
  float learning_rate;
  struct gpu_layer_ts *layer;
  struct gpu_layer_ts *input_layer;
  struct gpu_layer_ts *output_layer;
  ::brook::stream global_error;
  ::brook::stream target;
};


struct gpu_packed_ts {
int cols; /* usually 2048 */
int rows; /* 1 to whatever,  inclusive */
int last_cols; /* number of columns in the last row */
int total_size; /* in floats */
int chunk_size; /* size of the input chunk */
int chunk_last_floats; /* size of the bit at the end */
int offset; /* offset withing the stream.  This is used to make it so that two packed_ts can share the same stream if they 
are overlapping -- say for instance,  if we are going regressive nnl and the target is the offset + 1 */
::brook::stream all;          /* a big array of cols x rows of float4 - can store up to 8 million floats!*/
::brook::stream chunk_set;    /* an array <cols,1> of float4 used for transferring in and out */
::brook::stream float_sett;   /* an array <1,2048> used for transferring in and out */
::brook::stream decoder; /* stream of 4 elements: 1000 0100 0010 and 0001.  
                            This is like a constant stream and it is 
			    multiplied to the original 4 stream and then added up.  used by kernel_extract_f
			     */
};
