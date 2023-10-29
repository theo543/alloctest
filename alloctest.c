#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


uint32_t fast_rng(void) {
    const uint64_t MUL = 3202034522624059733, INC = 5340424991;
    static uint64_t state = 1;
    state = ((state * MUL) + INC);
    uint32_t ret = state >> 32;
    return ret;
}

int main(void) {
    const uint64_t GB = 1ull<<30;
    int count = 0;
    char **allocs = NULL;

    for(;;) {
        char *ptr = malloc(GB);
        if(ptr == NULL) {
            break;
        }
        count++;

        for(uint64_t i = 0;i < GB;i += 4) {
            // fill 4 bytes with a random value
            uint32_t val = fast_rng();
            memcpy(ptr + i, &val, sizeof(uint32_t));
        }

        allocs = realloc(allocs, sizeof(char*) * count);
        allocs[count - 1] = ptr;

        // print an exclamation mark check if a random byte from a random allocation is 0 so the allocations can't be optimized out
        int which_alloc = fast_rng() % count;
        int which_byte = fast_rng() % GB;
        int last_is_0 = (allocs[which_alloc][which_byte]) == 0;
        char* last_is_0_result = last_is_0 ? "!" : "";

        printf("\x1b[2K\rallocated %d GiB%s", count, last_is_0_result);
        fflush(stdout);
    }

    printf("\nallocation returned NULL after %d GiB\n", count);
}
