
#define thread_local _Thread_local

#define local_persist static

#define forward_global extern

#define null 0
	
void printf(const char* fmt, ...);
#define ASSERT_STR_HELPER(x) #x
#define ASSERT_STR(x) ASSERT_STR_HELPER(x)
#define assert_line(line, cond, ...) {if(!(cond)) { printf("Assertion failed in file " __FILE__ " on line " ASSERT_STR(line) "\nFailed Condition: " #cond ". Message: " __VA_ARGS__); crash(); }}
#define assert(cond, ...) {assert_line(__LINE__, cond, __VA_ARGS__)}

#define DEFER(start, end) for(int _i_ = ((start), 0); _i_ == 0; _i_ += 1, (end))

#if CONFIGURATION == RELEASE
#undef assert
#define assert(...) (void)0;
#endif

#define panic(...) { print(__VA_ARGS__); crash(); }

#define cast(t) (t)

#define ZERO(t) (t){0}



#define FIRST_ARG(arg1, ...) arg1
#define SECOND_ARG(arg1, arg2, ...) arg2
#define print(...) _Generic((FIRST_ARG(__VA_ARGS__)), \
                           string:  prints, \
                           default: printf \
                          )(__VA_ARGS__)
#define sprint(...) _Generic((SECOND_ARG(__VA_ARGS__)), \
                           string:  sprints, \
                           default: sprintf \
                          )(__VA_ARGS__)
#define tprint(...) _Generic((FIRST_ARG(__VA_ARGS__)), \
                           string:  tprints, \
                           default: tprintf \
                          )(__VA_ARGS__)

typedef struct Nothing {int nothing;} Nothing;

#ifndef CONTEXT_EXTRA
	#define CONTEXT_EXTRA Nothing
#endif

typedef enum Allocator_Message {
	ALLOCATOR_ALLOCATE,
	ALLOCATOR_DEALLOCATE,
	ALLOCATOR_REALLOCATE,
} Allocator_Message;
typedef void*(*Allocator_Proc)(u64, void*, Allocator_Message, void*);

typedef enum Log_Level {
	LOG_ERROR,
	LOG_INFO,
	LOG_WARNING,
	LOG_VERBOSE,
	
	LOG_LEVEL_COUNT,
} Log_Level;



typedef struct Allocator {
	Allocator_Proc proc;
	void *data;	
} Allocator;

Allocator get_heap_allocator();
Allocator get_temporary_allocator();

typedef struct Context {
	void *logger; // void(*Logger_Proc)(Log_Level level, string fmt, ...)
	
	u64 thread_id;
	
	CONTEXT_EXTRA extra;
} Context;


#define CONTEXT_STACK_MAX 512
thread_local Context context;
thread_local Context context_stack[CONTEXT_STACK_MAX];
thread_local u64 num_contexts = 0;

forward_global thread_local Allocator temp;

void* memset(void* dest, int value, size_t amount);
void* alloc(Allocator allocator, u64 size) {
	void *p = allocator.proc(size, 0, ALLOCATOR_ALLOCATE, allocator.data);
#if DO_ZERO_INITIALIZATION
	memset(p, 0, size);
#endif
	return p;
}
void* alloc_uninitialized(Allocator allocator, u64 size) {
	return allocator.proc(size, 0, ALLOCATOR_ALLOCATE, allocator.data);	
}
void dealloc(Allocator allocator, void *p) {
	allocator.proc(0, p, ALLOCATOR_DEALLOCATE, allocator.data);
}

void push_context(Context c) {
	assert(num_contexts < CONTEXT_STACK_MAX, "Context stack overflow");
	
	context_stack[num_contexts] = context;
	context = c;
	num_contexts += 1;
}
void pop_context() {
	assert(num_contexts > 0, "No contexts to pop!");
	num_contexts -= 1;
	context = context_stack[num_contexts];
}



u64 get_next_power_of_two(u64 x) {
    if (x == 0) {
        return 1;
    }

    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x |= x >> 32;

    return x + 1;
}