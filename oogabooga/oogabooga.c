/*

	CONFIGURATION:
	
		#define these before including oogabooga.c to configure
		
		All configuration properties has default values if you do not explicitly #define them.
	
		- ENTRY_PROC
			Define this as whatever the entry procedure of your program should be.
				
			Example:
				
				#define ENTRY_PROC my_entry
				void entry_proc(int, char**) {
					// ...
				}
				
			Note:
				void entry_proc(int, char**);
				
		- DO_ZERO_INITIALIZATION
			
			0: Disable zero initialization
			1: Enable zero initialization
			
			Example:
			
				// Disable zero initialization
				#define DO_ZERO_INITIALIZATION 0
				
			Note:
				Zero initialization only happens to memory allocated with the alloc() procedure.
			
		- ENABLE_SIMD
			0: Disable SIMD
			1: Enable SIMD
			
			Example:
			
				// Disable simd
				#define ENABLE_SIMD 0
			
		- Simd Extensions
			0: Disable
			1: Enable
			
			Possible extensions:	
				SIMD_ENABLE_SSE2
				SIMD_ENABLE_SSE41
				SIMD_ENABLE_AVX
				SIMD_ENABLE_AVX2
				SIMD_ENABLE_AVX512
				
			Example:
				// Enable SSE2 Extension
				#define SIMD_ENABLE_SSE2 1
				
			Note:
				I recommend that you do not touch this unless you know what you're doing.
				These may require you to pass the respective instruction set flag to your 
				compiler.
				For compatilibility reasons, all simd extensions are disabled by default.
				
		- INITIAL_PROGRAM_MEMORY_SIZE
			Defines this as the size in number of bytes you want the initial allocation for 
			your program memory to be.
			This will grow dynamically as needed.
			You can use helper macros KB, MB, GB.
			
			Example:
				#define INITIAL_PROGRAM_MEMORY_SIZE (MB(10))
			
			Note:
				This is not guaranteed to be exactly what you set it to because we have 
				minimum requirements for example to fit the temporary storage in program 
				memory. It's more of a rough guideline.
			
		- RUN_TESTS
			Run ooga booga tests.
		
			0: Disable
			1: Enable
			
			Example:
			
				#define RUN_TESTS 0
				
		- ENABLE_PROFILING
			Enable time profiling which will be dumped to google_trace.json.
		
			0: Disable
			1: Enable
			
			Example:
			
				#define ENABLE_PROFILING 1
				
			Note:
				See timing macros in profile.c
					tm_scope_cycles
					tm_scope_cycles_var
					tm_scope_cycles_accum
		

*/

#define OGB_VERSION_MAJOR 0
#define OGB_VERSION_MINOR 0
#define OGB_VERSION_PATCH 5

#define OGB_VERSION (OGB_VERSION_MAJOR*1000000+OGB_VERSION_MINOR*1000+OGB_VERSION_PATCH)

#include <math.h>
#include <immintrin.h>
#include <intrin.h>
#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef u8 uint8;
typedef s8 int8;
typedef u16 uint16;
typedef s16 int16;
typedef u32 uint32;
typedef s32 int32;
typedef u64 uint64;
typedef s64 int64;

typedef float f32;
typedef double f64;
typedef f32 float32;
typedef f64 float64;

#define F32_MAX 3.402823466e+38F
#define F32_MIN 1.175494351e-38F

typedef u8 bool;
#define false 0
#define true 1


// Determine what compiler we are on
#ifdef __clang__
    #define COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
    #define COMPILER_GCC 1
#elif defined(_MSC_VER)
    #define COMPILER_MSVC 1
#else
    #define COMPILER_UNKNOWN 1
    #warning "Compiler is not explicitly supported, some things will probably not work as expected"
#endif

#define DEBUG 0
#define RELEASE 2

#if defined(NDEBUG)
	#define CONFIGURATION RELEASE
#else
	#define CONFIGURATION DEBUG
#endif


#include "cpu.c"




#ifndef ENTRY_PROC
	#define ENTRY_PROC entry
#endif

#ifndef DO_ZERO_INITIALIZATION
	#define DO_ZERO_INITIALIZATION 1
#endif

#ifndef ENABLE_SIMD
	#define ENABLE_SIMD 1
#endif

#if ENABLE_SIMD && !defined(SIMD_ENABLE_SSE2)
	#if COMPILER_CAN_DO_SSE2
		#define SIMD_ENABLE_SSE2 1
	#else
		#define SIMD_ENABLE_SSE2 0
	#endif
#endif
#if ENABLE_SIMD && !defined(SIMD_ENABLE_SSE41)
	#define SIMD_ENABLE_SSE41 0
#endif
#if ENABLE_SIMD && !defined(SIMD_ENABLE_AVX)
	#define SIMD_ENABLE_AVX 0
#endif
#if ENABLE_SIMD && !defined(SIMD_ENABLE_AVX2)
	#define SIMD_ENABLE_AVX2 0
#endif
#if ENABLE_SIMD && !defined(SIMD_ENABLE_AVX512)
	#define SIMD_ENABLE_AVX512 0
#endif


#define WINDOWS 0
#define LINUX   1
#define MACOS   2

#ifdef _WIN32
	#include <Windows.h>
	#define TARGET_OS WINDOWS
	#define OS_PATHS_HAVE_BACKSLASH 1
#elif defined(__linux__)
	// Include whatever #Incomplete #Portability
	#define TARGET_OS LINUX
	#error "Linux is not supported yet";
	#define OS_PATHS_HAVE_BACKSLASH 0
#elif defined(__APPLE__) && defined(__MACH__)
	// Include whatever #Incomplete #Portability
	#define TARGET_OS MACOS
	#error "Macos is not supported yet";
	#define OS_PATHS_HAVE_BACKSLASH 1
#else
	#error "Current OS not supported!";
#endif


// This needs to be included before dependencies
#include "base.c"

///
///
// Dependencies
///

#include "third_party.c"

/////

#include "simd.c"        

// #Incomplete
// We might want to make this configurable ?
#define GFX_RENDERER_D3D11  0
#define GFX_RENDERER_VULKAN 1
#define GFX_RENDERER_METAL  2
#ifndef GFX_RENDERER
// #Portability
	#if TARGET_OS == WINDOWS
		#define GFX_RENDERER GFX_RENDERER_D3D11
	#elif TARGET_OS == LINUX
		#define GFX_RENDERER GFX_RENDERER_VULKAN
	#elif TARGET_OS == MACOS
		#define GFX_RENDERER GFX_RENDERER_METAL
	#endif
#endif


#include "string.c"
#include "unicode.c"
#include "string_format.c"
#include "hash.c"
#include "path_utils.c"
#include "linmath.c"
#include "range.c"
#include "utility.c"

#include "hash_table.c"

#include "os_interface.c"
#include "concurrency.c"
#include "gfx_interface.c"

#include "font.c"

#include "profiling.c"
#include "random.c"
#include "color.c"
#include "memory.c"
#include "input.c"

#include "drawing.c"

#include "audio.c"

// #Portability
#if GFX_RENDERER == GFX_RENDERER_D3D11
	#include "gfx_impl_d3d11.c"
#elif GFX_RENDERER == GFX_RENDERER_VULKAN
	#error "We only have a D3D11 renderer at the moment"
#elif GFX_RENDERER == GFX_RENDERER_METAL
	#error "We only have a D3D11 renderer at the moment"
#else
	#error "Unknown renderer GFX_RENDERER defined"
#endif

#if TARGET_OS == WINDOWS
	#include "os_impl_windows.c"
#elif TARGET_OS == LINUX
	#error "Linux is not supported yet"
#elif TARGET_OS == MACOS
	#error "Macos is not supported yet"
#else
	#error "Current OS is not supported"
#endif

#include "tests.c"

#define malloc please_use_alloc_for_memory_allocations_instead_of_malloc
#define free please_use_dealloc_for_memory_deallocations_instead_of_free

void oogabooga_init(u64 program_memory_size) {
	context.logger = default_logger;
	temp = get_initialization_allocator();
	Cpu_Capabilities features = query_cpu_capabilities();
	os_init(program_memory_size);
	heap_init();
	temporary_storage_init();
	log_info("Ooga booga version is %d.%02d.%03d", OGB_VERSION_MAJOR, OGB_VERSION_MINOR, OGB_VERSION_PATCH);
	gfx_init();
	log_verbose("CPU has sse1: %cs", features.sse1 ? "true" : "false");
	log_verbose("CPU has sse2: %cs", features.sse2 ? "true" : "false");
	log_verbose("CPU has sse3: %cs", features.sse3 ? "true" : "false");
	log_verbose("CPU has ssse3: %cs", features.ssse3 ? "true" : "false");
	log_verbose("CPU has sse41: %cs", features.sse41 ? "true" : "false");
	log_verbose("CPU has sse42: %cs", features.sse42 ? "true" : "false");
	log_verbose("CPU has avx: %cs", features.avx ? "true" : "false");
	log_verbose("CPU has avx2: %cs", features.avx2 ? "true" : "false");
	log_verbose("CPU has avx512: %cs", features.avx512 ? "true" : "false");
}

int ENTRY_PROC(int argc, char **argv);

int main(int argc, char **argv) {

	print("Ooga booga program started\n");
	oogabooga_init(INITIAL_PROGRAM_MEMORY_SIZE); 
	
	
	assert(main != ENTRY_PROC, "You've ooga'd your last booga");
	
	#if RUN_TESTS
		oogabooga_run_tests();
	#endif
	
	int code = ENTRY_PROC(argc, argv);
	
#if ENABLE_PROFILING
	
	dump_profile_result();
	
#endif
	
	printf("Ooga booga program exit with code %i\n", code);
	
	return code;
}



// I hope whoever caused this @ microsoft is fired.
#ifdef near
#undef near
#endif
#ifdef far
#undef far 
#endif