#include<array>
#include<arm_neon.h>
#include<vector>
#include<cstdint>
#include<cstdlib>

//https://developer.arm.com/documentation/dui0472/k/Using-NEON-Support/NEON-intrinsics-for-extracting-lanes-from-a-vector-into-a-register
struct toggle_block_t{
    uint16_t start;
    uint16_t mask;
};

//https://stackoverflow.com/a/41406502/73007
//https://lemire.me/blog/2017/07/03/pruning-spaces-from-strings-quickly-on-arm-processors/
static inline bool is_not_zero(uint8x16_t v) {
  uint64x2_t v64 = vreinterpretq_u64_u8(v);
  uint32x2_t v32 = vqmovn_u64(v64);
  uint64x1_t result = vreinterpret_u64_u32(v32);
  return vget_lane_u64(result, 0);
}

static inline uint16_t neonmovemask_addv(uint8x16_t input8) {
  uint16x8_t input = vreinterpretq_u16_u8(input8);
  const uint16x8_t bitmask = { 0x0101 , 0x0202, 0x0404, 0x0808, 0x1010, 0x2020, 0x4040, 0x8080 };
  uint16x8_t minput = vandq_u16(input, bitmask);
  return vaddvq_u16(minput);
}

static inline toggle_block_t* check_toggle_block(
    toggle_block_t* back,
    uint16_t i,
    uint8x16_t& curtog,
    uint8x16_t v,
    const uint8x16_t thresh_v
)
{
    uint8x16_t mask_v=vcgeq_u8(v,thresh_v);
    uint8x16_t is_not_toggle=vmvnq_u8(vceqq_u8(mask_v,curtog));
    if(is_not_zero(is_not_toggle)) //some of them are not toggle
    {
        uint16_t m16=neonmovemask_addv(mask_v);
        *(back++)=toggle_block_t{i,m16};
        curtog=vdupq_lane_u8(vget_high_u8(mask_v),7);
    }
    return back;
}

//row MUST be shorter than 65536*16 and a multiple of 64 
toggle_block_t* find_toggle_blocks_row(
    toggle_block_t* back,
    const uint8_t* data,
    uint16_t len,
    const uint8_t threshold
)
{
    const uint8x16_t thresh_v=vdupq_n_u8(threshold);
    uint8x16_t curtog=vdupq_n_u8(0x0);
    size_t num_blocks=len/16;
    for(size_t i=0;i<len;i++)
    {
        uint8x16_t v=vld1q_u8(data+(i*16));
        back=check_toggle_block(back,i,curtog,v,thresh_v);
    }
    return back;
}



