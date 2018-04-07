void reducedSum(__local  float*, __global float*);

//This function is not completely random but gives a number between 0 and 1 and is similar to the rand function defined in stdlib.h
float random_number(unsigned int seed)
{ 
   //unsigned int seed = 10;
   const unsigned int rand_max = 32768;
   unsigned int rv;
   seed = seed * 1103515245 + 12345;
   rv = ((unsigned)(seed/65536) % rand_max);
   return (double)rv/rand_max;
}
                  

__kernel void monte(                                                                        
   const float        step_size,                           
   __local  float*    local_sums,                          
   __global float*    partial_sums)                        
{                                                          
   int num_work_items = get_local_size(0);                 
   int local_id       = get_local_id(0);                   
   int group_id       = get_group_id(0);                   
   
   float x, y, accum = 0.0f;                              
   int i;                                      
   
   i = (group_id * num_work_items + local_id) ;      
 
   x = 3.14159*random_number(i*3.142+1);
   y = random_number(i);      
   
   if(y-sin(x)<=0)
      accum+=1;  

   local_sums[local_id] = accum;
   barrier(CLK_LOCAL_MEM_FENCE);
   
   reducedSum(local_sums, partial_sums);                  
}


void reducedSum(                                          
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
   
