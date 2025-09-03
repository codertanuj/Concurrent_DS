## Lock-Free Concurrent Data Structures
A collection of high-performance, thread-safe data structures implemented without locks using atomic operations and memory ordering guarantees.
### Features

- Zero-lock design: All data structures use atomic operations and compare-and-swap (CAS) for thread safety
- High performance: Optimized for concurrent access with minimal contention overhead
- Memory safe: Proper memory reclamation and ABA problem prevention
- Scalable: Performance improves with thread count rather than degrading

## Data Structures

- Concurrent Stack: LIFO stack with atomic push/pop operations 
- Concurrent Queue: FIFO queue supporting multiple producers and consumers
- Hash Table: Lock-free hash map with dynamic resizing
- Priority Queue: Thread-safe priority queue implementation

### Requirements

C++17 or later

