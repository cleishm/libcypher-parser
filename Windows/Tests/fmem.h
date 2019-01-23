#ifndef FMEM_H_
#define FMEM_H_

#include <stdio.h>

#define FMEM_API

struct fmem_reserved {
    char reserved[32];
};

typedef struct fmem_reserved fmem;

FMEM_API void fmem_init(fmem *file);
FMEM_API void fmem_term(fmem *file);
FMEM_API FILE *fmem_open(fmem *file, const char *mode);
FMEM_API void fmem_mem(fmem *file, void **mem, size_t *size);

#endif /* !FMEM_H_ */
