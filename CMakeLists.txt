cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
    "Debug"
    CACHE
    STRING
    "Choose build type: Debug Release Distribution"
    FORCE
  )
  set_property(CACHE
    CMAKE_BUILD_TYPE
    PROPERTY
    STRINGS
    "Debug" "Release" "Distribution"
  )
endif()

set(CMAKE_CXX_FLAGS_DISTRIBUTION
  "-O3 -DNDEBUG"
  CACHE
  STRING
  "Distribution CXX flags"
  FORCE
)
set(CMAKE_EXE_LINKER_FLAGS_DISTRIBUTION
  "-s"
  CACHE
  STRING
  "Distribution linker flags"
  FORCE
)
set(CMAKE_SHARED_LINKER_FLAGS_DISTRIBUTION
  "-s"
  CACHE
  STRING
  "Distribution shared linker flags"
  FORCE
)

add_library(nacl STATIC)

target_compile_definitions(nacl PUBLIC
  $<$<CONFIG:Debug>:
    NACL_CONFIG_DEBUG
  >
  $<$<CONFIG:Release>:
    NACL_CONFIG_RELEASE
  >
  $<$<CONFIG:Distribution>:
    NACL_CONFIG_DISTRIBUTION
    NDEBUG
  >
)

target_sources(nacl PUBLIC
  FILE_SET CXX_MODULES FILES
  src/unit.cppm
  src/types.cppm
  src/tags.cppm
  src/meta.cppm
  src/concepts.cppm
  src/panic.cppm
  src/utils.cppm
  src/ref.cppm
  src/option.cppm
  src/downcast.cppm
  src/allocate_aligned.cppm
  src/allocate_bytes.cppm
  src/aligned_heap_allocator.cppm
  src/box.cppm
  src/array_iterators.cppm
  src/slice.cppm
  src/list.cppm
  src/nacl.cppm
)
