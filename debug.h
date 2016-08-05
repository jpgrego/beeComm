#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while(0)
#endif

#endif /* DEBUG_H */
