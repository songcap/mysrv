#include "util.h"

namespace mysrv{
  
pid_t GetThreadPid(){
   return syscall(SYS_gettid);
}

}

