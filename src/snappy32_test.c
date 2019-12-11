#include "snappy32.h"
#include <stdio.h>

#define MAXLEN (128*1024)

int main()
{
    uint32_t inbuf[MAXLEN];
    uint8_t outbuf[MAXLEN*4];
    size_t count = fread(inbuf, 4, MAXLEN, stdin);
    
    if (!feof(stdin))
    {
        fprintf(stderr, "Message does not fit in buffer\n");
        return 1;
    }
    
    snappy_state_t state;
    
    snappy_init(&state, NULL, outbuf, sizeof(outbuf));
    snappy_compress(&state, inbuf, count);
    size_t len = snappy_finish(&state);

    fwrite(outbuf, 1, len, stdout);
    return 0;
}
