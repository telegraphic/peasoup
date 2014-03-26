#pragma once
#include <data_types/fourierseries.hpp>
#include <kernels/kernels.h>
#include <kernels/defaults.h>
#include <iostream>

class HarmonicFolder {
private:
  unsigned int max_blocks;
  unsigned int max_threads;
  float** h_data_ptrs;
  float** d_data_ptrs;
  HarmonicSums<float>& sums;

public:
  HarmonicFolder(HarmonicSums<float>& sums,
		 unsigned int max_blocks=MAX_BLOCKS,
		 unsigned int max_threads=MAX_THREADS)
    :sums(sums),max_blocks(max_blocks),max_threads(max_threads)
  {
    Utils::device_malloc<float*>(&d_data_ptrs,sums.size());
    Utils::host_malloc<float*>(&h_data_ptrs,sums.size());
  }
  
  void fold(DevicePowerSpectrum<float>& fold0)
  {
    for (int ii=0;ii<sums.size();ii++)
      {
	h_data_ptrs[ii] = sums[ii]->get_data();
      }
    Utils::h2dcpy<float*>(d_data_ptrs,h_data_ptrs,sums.size());
    device_harmonic_sum(fold0.get_data(),d_data_ptrs,
			fold0.get_nbins(),sums.size(),
			max_blocks,max_threads);
  }
  
  ~HarmonicFolder()
  {
    Utils::device_free(d_data_ptrs);
    Utils::host_free(h_data_ptrs);
  }
};
