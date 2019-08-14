#ifdef  __UTIL_H__
#define __UTIL_H__
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

namespace mysrv{

pid_t GetThreadPid();

}

#endif /*__UTIL_H__ */
