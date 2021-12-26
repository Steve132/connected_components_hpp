#include<array>
#include<vector>
#include<cstdint>
#include<cstdlib>


#include<experimental/simd>


namespace stdx = std::experimental;

//TODO the mask uint should actually be selected
//using a template based on the size of the simd mask

using uint8v_t = stdx::native_simd<uint8_t>;
using uint8v_mask_t = stdx::native_simd_mask<uint8_t>;
using mask_cpu_t = min_uint_t<stdx::native_simd_mask<uint8_t>::size()>;

struct toggle_block_t{
    uint16_t start;
    mask_cpu_t mask;
};

//if LLVM:


template<class Mask>
static inline 
mask_cpu_t simd_raw_mask(const Mask& m)
{

#if defined(__GNUC__)
    return m.__to_bitset().to_ullong();
#elif defined(__clang__)
    typename class Mask::__native_type z=m;
    return static_cast<mask_cpu_t>(z);
#endif

}


static inline 
toggle_block_t* check_toggle_block(
    toggle_block_t* back,
    uint16_t i,
    bool& curtog,
    uint8v_t v,
    const uint8v_t thresh_v
)
{
    uint8v_mask_t mask_v=v > thresh_v;
    if(!stdx::all_of(mask_v==uint8v_mask_t(curtog)))
    {
        uint16_t msk=simd_raw_mask(mask_v);
        *(back++)=toggle_block_t{i,msk};
        curtog=mask_v[uint8v_mask_t::size()-1];
    }

   /* uint8x16_t is_not_toggle=vmvnq_u8(vceqq_u8(mask_v,curtog));
    if(is_not_zero(is_not_toggle)) //some of them are not toggle
    {
        uint16_t m16=neonmovemask_addv(mask_v);
        *(back++)=toggle_block_t{i,m16};
        curtog=vdupq_lane_u8(vget_high_u8(mask_v),7);
    }*/
    return back;
}

toggle_block_t* find_toggle_blocks_row(
    toggle_block_t* back,
    const uint8_t* data,
    uint16_t len,
    const uint8_t threshold
)
{
    const uint8v_t thresh_v(threshold);
    bool curtog=false;
    size_t num_blocks=len/uint8v_t::size();
    for(size_t i=0;i<len;i++)
    {
        uint8v_t v(data+(i*uint8v_t::size()),stdx::vector_aligned);
        back=check_toggle_block(back,i,curtog,v,thresh_v);
    }
    return back;
}

