#include "deflate.h"
#include <stdio.h>

#define MAXLEN (128*1024)

int main()
{
    uint32_t inbuf[MAXLEN];
    uint32_t outbuf[MAXLEN];
    size_t count = fread(inbuf, 4, MAXLEN, stdin);
    
    if (!feof(stdin))
    {
        fprintf(stderr, "Message does not fit in buffer\n");
        return 1;
    }
    
    deflate_state_t state;
    
    deflate_init(&state, inbuf, count);
    deflate_start_block(&state, outbuf);
    deflate_compress(&state, inbuf, count);
    deflate_end_block(&state);
    
    fwrite(outbuf, 4, state.stream.wr_pos - outbuf, stdout);
    return 0;
}
