#include "loader_core.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void) {
    m14_loader s={0};
    m14_header h={M14_WIRE_MAGIC,1,M14_BEGIN,0,0,0,M14_WEIGHT_BYTES,0x12345678};
    m14_header decoded, reply;
    unsigned char wire[32];
    assert(m14_crc("123456789",9)==0xcbf43926u);
    assert((m14_crc_update(m14_crc_update(0xffffffffu,"1234",4),"56789",5)^0xffffffffu)==0xcbf43926u);
    m14_encode(wire,&h); m14_decode(&decoded,wire);
    assert(decoded.crc==h.crc && decoded.total==h.total && !memcmp(wire,"M14W",4));
    assert(m14_validate(&s,&h)==M14_OK);
    h.size=1; assert(m14_validate(&s,&h)==M14_BAD_HEADER);
    h.size=0; h.op=M14_DATA; assert(m14_validate(&s,&h)==M14_NOT_READY);
    s.active=1; s.seq=1; s.expected_crc=h.crc;
    h.seq=1; h.size=M14_CHUNK_BYTES; assert(m14_validate(&s,&h)==M14_OK);
    h.size=M14_CHUNK_BYTES+1; assert(m14_validate(&s,&h)==M14_BAD_HEADER);
    h.size=0; assert(m14_validate(&s,&h)==M14_BAD_HEADER);
    s.next=0xfffffff0u; h.offset=s.next; h.size=32;
    assert(m14_validate(&s,&h)==M14_BAD_HEADER); /* address overflow */
    s.next=M14_WEIGHT_BYTES-16; h.offset=s.next; h.size=32;
    assert(m14_validate(&s,&h)==M14_BAD_HEADER); /* out of reserved DDR */
    h.size=16; h.offset=0; assert(m14_validate(&s,&h)==M14_BAD_ORDER);
    h.offset=s.next; h.seq=0; assert(m14_validate(&s,&h)==M14_BAD_ORDER);
    h.seq=1; h.op=M14_COMMIT; h.size=0;
    assert(m14_validate(&s,&h)==M14_BAD_ORDER); /* truncated image */
    s.next=M14_WEIGHT_BYTES; h.offset=s.next;
    assert(m14_validate(&s,&h)==M14_OK);
    h.crc^=1; assert(m14_validate(&s,&h)==M14_BAD_ORDER);
    m14_reply(&reply,&s,&h,M14_DDR_ERROR,0x42);
    assert(reply.op==(M14_COMMIT|0x80000000u) && reply.size==M14_DDR_ERROR && reply.crc==0x42);
    puts("M14_TCP_CORE_TEST=PASS hardware_access=0"); return 0;
}
