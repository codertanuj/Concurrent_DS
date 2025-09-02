#pragma once
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <iostream>

// Node holds a positive 4-byte int and a next pointer.
struct Node {
    int value;
    Node* next;
    explicit Node(int v) : value(v), next(nullptr) {}
};

// A tagged top pointer: (ptr, pop_count). pop_count increments on every successful pop()
// so CAS compares both fields, mitigating ABA.
struct Top {
    Node* ptr;
    std::uint64_t pop_count;
};

// Make sure Top is trivially copyable for std::atomic.
static_assert(std::is_trivially_copyable<Top>::value, "Top must be trivially copyable");

class LockFreeStack {
public:
    LockFreeStack() noexcept {
        Top init{nullptr, 0};
        top_.store(init, std::memory_order_relaxed);
    }

    // Push positive integer v. Ignores Out-of-Memory problems.
    void push(int v) {
        Node* n = new Node(v);
        Top old = top_.load(std::memory_order_acquire);
        for (;;) {
            n->next = old.ptr;
            Top desired{n, old.pop_count}; // pop_count unchanged on push
            if (top_.compare_exchange_weak(
                    old, desired,
                    std::memory_order_release,
                    std::memory_order_acquire)) {
                return;
            }
            // CAS failed: 'old' is updated with current top_, retry
        }
    }

    // Pop returns value if available; returns negative integer when empty (total).
    int pop() {
        Top old = top_.load(std::memory_order_acquire);
        for (;;) {
            Node* head = old.ptr;
            if (head == nullptr) {
                return -1; // empty => total, non-blocking
            }
            Top desired{head->next, old.pop_count + 1}; // bump version on successful pop
            if (top_.compare_exchange_weak(
                    old, desired,
                    std::memory_order_acq_rel,
                    std::memory_order_acquire)) {
                int v = head->value;
                delete head; // assignment ignores detailed memory reclamation schemes
                return v;
            }
            // CAS failed: 'old' updated, retry
        }
    }

    // Debug/verification: prints stack contents from top to bottom.
    // Call after all threads join; concurrent traversal is allowed but yields a transient view.
    void print(std::ostream& os = std::cout) const {
        Top snap = top_.load(std::memory_order_acquire);
        Node* cur = snap.ptr;
        os << "[top] ";
        while (cur) {
            os << cur->value << " ";
            cur = cur->next;
        }
        os << "[bottom]\n";
    }

private:
    std::atomic<Top> top_;
};
