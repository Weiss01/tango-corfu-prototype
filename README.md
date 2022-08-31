## Distributed Data Structures With Tango

This is a simple proof of concept demonstration of the shared log architecture and distributed data structure implementation introduced in the two research papers referenced below.

**Corfu** is essentially a distributed shared log that is simple, easy to implement and use. It allows for very high horizontal scalabity of storage space and read write throughput over a cluster.

**Tango** is the distributed data structure built on top of Corfu that leverages the shared log to enable fast transactions
across different objects, allowing applications to partition state across machines and scale to the limits of the
underlying log without sacrificing consistency.

### Quick Start
1. Execute `make build` to compile the executables needed for the demo.
2. Execute `./scheduler` to start up the storage servers, the layout and the scheduler.
3. Execute multiple `./lrucache_demo` instances on seperate terminal windows. They should automatically connect to the scheduler and you can start modifying the Tango-LRUCache.
> Variables available to modify in this demo are *a* and *b*.
> 
> Available operations are `update`, `put(key,value)` and `get(key)`.
> 
> Watch the LRUCache change live across all clients as you operate on it! 

### _References_
Balakrishnan, M., Malkhi, D., Davis, J. D., Prabhakaran, V., Wei, M., & Wobber, T. (2013). CORFU: A distributed shared log. ACM Trans. Comput. Syst., 31(4), Article 10. https://doi.org/10.1145/2535930

Balakrishnan, M., Malkhi, D., Wobber, T., Wu, M., Prabhakaran, V., Wei, M., Davis, J., Rao, S., Zou, T., & Zuck, A. (2013). Tango: Distributed data structures over a shared log. https://doi.org/10.1145/2517349.2522732 
