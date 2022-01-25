#ifndef FILL_SEGMENTS_HPP
#define FILL_SEGMENTS_HPP

#include<cstdint>
#include "disjoint_set.hpp"


class fill_segments
{
public:
	struct segment_t{
		uint32_t start,finish,id,row;
		segment_t(uint32_t s=0,uint32_t f=0):start(s),finish(f),id(0),row(0)
		{}
	}; // (start1 <= finish2) && (start2 <= end1)) 
	
	//should be possible to binary search to find which ones need to be unioned with.
	
private:
	size_t ROWS,COLS;
	size_t idcount;
    
    disjoint_set<uint32_t> connectivitySet;
	std::vector<std::vector<segment_t>> row_segments;
	//std::unordered_map<uint32_t,std::vector<segment_t>> inverse_segments_buckets;
	std::vector<std::vector<segment_t>> inverse_segments_buckets;
    
    
	static void find_segments_in_row(const uint8_t* row_ptr,size_t cols,std::vector<segment_t>& vec,size_t ms);
    static void find_segments_in_row_vec(const uint8_t* row_ptr,size_t cols,std::vector<segment_t>& vec,size_t ms);
	void rows_to_segments(const uint8_t* frame,uint32_t ms);
	uint32_t set_ids();
	void build_connectivity(uint32_t id);
	void build_inverse_lookup_segments();
public:
	fill_segments(size_t r,size_t c):
	ROWS(r),COLS(c),row_segments(r)
	{}
	void reset()
	{
		for(std::vector<segment_t>& rowsegs : row_segments) rowsegs.clear();
		for(std::vector<segment_t>& allsegs : inverse_segments_buckets) allsegs.clear();
	}
	void frame(const uint8_t* bwimg,uint32_t ms=1)
	{
		#pragma omp single
		{
			reset();
		}
		rows_to_segments(bwimg,ms);
		#pragma omp single
		{
			uint32_t id=set_ids();
			build_connectivity(id);
			build_inverse_lookup_segments();
		}
	}
	const std::vector<segment_t>& segs_matching(uint32_t id) { 
		return inverse_segments_buckets[connectivitySet.find(id)]; 
	}
	uint32_t parent(uint32_t id){
		return connectivitySet.find(id);
	}
	const std::vector<segment_t>& row_segs(size_t r) const {
		return row_segments[r];
	}
	size_t num_ids() const { return inverse_segments_buckets.size(); }
};




#endif
