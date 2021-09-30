#include "fill_segments.hpp"

static inline bool intersect(const fill_segments::segment_t& a,const fill_segments::segment_t& b)
{
    return (a.start < b.finish) && (a.start < b.finish);
}

void fill_segments::find_segments_in_row(const uint8_t* row_ptr,size_t cols,std::vector<segment_t>& vec,size_t ms)
{
    for(size_t i=0;i<cols;i++)
    {
        if(row_ptr[i])
        {
            uint32_t start=i;
            uint32_t finish=i+1;
            while(i<cols && row_ptr[i]){
                finish=++i;
            }
            if(finish-start > ms)
            {
                vec.emplace_back(start,finish);
            }
        }
    }
}

void fill_segments::rows_to_segments(const uint8_t* frame,uint32_t ms)
{		
    const size_t R=ROWS;
    #pragma omp for
    for(size_t r=0;r<R;r++) 
    {
        find_segments_in_row(frame+r*COLS,COLS,row_segments[r],ms);
    }
}

uint32_t fill_segments::set_ids()
{
    uint32_t id=0;
    size_t R=ROWS;
    for(size_t r=0;r<R;r++)
    {
        for(segment_t& seg : row_segments[r])
        {
            seg.id=id++;
        }
    }
    return id;
}
void fill_segments::build_connectivity(uint32_t id)
{
    connectivitySet.reset(id);
    size_t R=ROWS;
    for(size_t r=1;r<R;r++)
    {
        for(const segment_t& seg1 : row_segments[r]) //TODO make this more efficient with a zip/binary search lower bounds on both sides
        {
            for(const segment_t& seg2 : row_segments[r-1])
            {
                if(intersect(seg1,seg2))
                {
                    connectivitySet.Union(seg1.id,seg2.id);
                }
            }
        }
    }
}
void fill_segments::build_inverse_lookup_segments()
{
    ///For array based lookup.
    inverse_segments_buckets.resize(connectivitySet.size());
    
    size_t R=ROWS,C=COLS;
    for(size_t r=0;r<R;r++)
    {
        for(segment_t& seg : row_segments[r])
        {
            seg.start+=r*C;
            seg.finish+=r*C;
            uint32_t par=connectivitySet.Find(seg.id);
            seg.id=par;
            inverse_segments_buckets[par].push_back(seg);
        }
    }	
}
