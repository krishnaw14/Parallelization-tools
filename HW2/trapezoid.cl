void reduce(                                          
   __local  float*,                          
   __global float*);
                        

__kernel void pi(                                                                        
   const float        step_size,                           
   __local  float*    local_sums,                          
   __global float*    partial_sums)                        
{                                                          
   int num_wrk_items  = get_local_size(0);                 
   int local_id       = get_local_id(0);                   
   int group_id       = get_group_id(0);                   
   
   float x, accum = 0.0f;                              
   int i;                                      
   
   i = (group_id * num_wrk_items + local_id) ;      
 
   x = i*step_size;   
   accum += sin(x);  

   local_sums[local_id] = accum;
   barrier(CLK_LOCAL_MEM_FENCE);
   
   reduce(local_sums, partial_sums);                  
}

//------------------------------------------------------------------------------
//
// OpenCL function:  reduction    
//
// Purpose: reduce across all the work-items in a work-group
// 
// input: local float* an array to hold sums from each work item
//
// output: global float* partial_sums   float vector of partial sums
//

void reduce(                                          
   __local  float*    local_sums,                          
   __global float*    partial_sums)                        
{                                                          
   int num_wrk_items  = get_local_size(0);                 
   int local_id       = get_local_id(0);                   
   int group_id       = get_group_id(0);                   
   
   float sum;                              
   int i;                                      
   
   if (local_id == 0) {                      
      sum = 0.0f;                            
   
      for (i=0; i<num_wrk_items; i++) {        
          sum += local_sums[i];             
      }                                     
   
      partial_sums[group_id] = sum;         
   }
}
                                          
