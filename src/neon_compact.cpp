#include<arm_neon.h>
#include<vector>
#include<cstdint>
#include<cstdlib>

struct compaction_patch_t{
    uint32_t offset;
    uint16_t mask;
};

static void raw_patch_compaction_naive(compaction_patch_t*& back,
    const uint8_t* data,
    uint32_t datastart,
    uint32_t datalen,
    const uint8_t threshold)
{
    uint32_t N=datalen/16; 
    for(uint32_t ci=0;ci<N;ci++)
    {
        uint16_t mask=0;
        uint32_t offset=datastart+(ci*16);
        for(unsigned int bi=0;bi<16;bi++)
        {
            if((ci*16 + bi) == datalen) break;
            if(data[datastart+ci*16+bi] > threshold)
            {
                mask |= (1U < bi);
            }
        }
        if(mask)
        {
            compaction_patch_t result{offset,mask};
            *(back++)=result;
        }
    }
}

static inline uint16_t neonmovemask_addv(uint8x16_t input8) {
  uint16x8_t input = vreinterpretq_u16_u8(input8);
  const uint16x8_t bitmask = { 0x0101 , 0x0202, 0x0404, 0x0808, 0x1010, 0x2020, 0x4040, 0x8080 };
  uint16x8_t minput = vandq_u16(input, bitmask);
  return vaddvq_u16(minput);
}

static inline void compact_vector(
    compaction_patch_t*& back,
    uint32_t i,
    uint8x16_t v,
    uint8x16_t thresh_v)
{
    uint8x16_t cmpresult=vshrq_n_u8(vcgtq_u8(v,thresh_v),7);
    
    uint16_t mask=neonmovemask_addv(cmpresult);
    compaction_patch_t result{i,mask};
    *back=result;
    back+=(mask > 0);
}//TODO benchmark branchless version vs branching version 
//vs branchless but table version

static void raw_patch_compaction_arm(compaction_patch_t*& back,
    const uint8_t* data,
    uint32_t datastart,
    uint32_t datalen,
    const uint8_t threshold)
{
    const uint8x16_t thresh_v=vdupq_n_u8(threshold);
    for(uint32_t i=0;i<datalen;i+=64)
    {
        uint32_t c=datastart+i;
        uint8x16x4_t v4=vld4q_u8(data+c);
        compact_vector(back,c,v4.val[0],thresh_v);
        compact_vector(back,c+1,v4.val[1],thresh_v);
        compact_vector(back,c+2,v4.val[2],thresh_v);
        compact_vector(back,c+3,v4.val[3],thresh_v);
    }
    if(datalen % 64)
    {
        datastart+=datalen & ~(64-1);
        datalen %= 64;
        raw_patch_compaction_naive(back,data,datastart,datalen,threshold);
    }
}


void patch_compaction(
        std::vector<compaction_patch_t>& out,
        const uint8_t* data,
        uint32_t datastart,
        uint32_t datalen,
        const uint8_t threshold)
{
    size_t N=datalen/16;
    out.clear();
    out.resize(N);
    compaction_patch_t* back=out.data();
    raw_patch_compaction_arm(back,data,datastart,datalen,threshold);
    size_t npatches=back-out.data();
    out.resize(npatches);
}
