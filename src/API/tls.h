
#include "API/CommonObject.h"

__thread psoaCommonObject * zzz = NULL;

/*
tsl struct must be easy to reference object. One way to do this is 
that have every object in psoaCommonObject to have a unique handle 
which is a pointer into an array of local values.

Because of this we need a "singleton" to hold a reference to a
master list? or maybe just a counter to which item is next?
[And a way to increment/resize the array ?]

Items in the array are pointers to psoaCommonObject * ???
(need space for iterators!)

On open
  - verify that the object exists and get ptr
  - mt-lock
  - find a location in array 
  - set the internal handle to this.
  - mt-unlock

What happen on close?
  mt-lock
  set location to -1
  mt-unlock
  
Start tx
  - grab tls for current thread 
       if null: malloc it and init to zeros
  - set bool txon to true (txon is in tls)

object access
  - grab tls for current thread 
  - check txon
  - get tls data for this object

End tx
  - set bool txon to true
  
  
if ( zzz == NULL


*/