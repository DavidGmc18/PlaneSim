#pragma once

// Signed
typedef __INT8_TYPE__ i8;
typedef __INT16_TYPE__ i16;
typedef __INT32_TYPE__ i32;
typedef __INT64_TYPE__ i64;

// Unsigned
typedef __UINT8_TYPE__ u8;
typedef __UINT16_TYPE__ u16;
typedef __UINT32_TYPE__ u32;
typedef __UINT64_TYPE__ u64;

#define U8_MAX UINT8_MAX
#define U16_MAX UINT16_MAX
#define U32_MAX UINT32_MAX
#define U64_MAX UINT64_MAX

// Size
typedef __PTRDIFF_TYPE__ isize;
typedef __SIZE_TYPE__ usize;

#define USIZE_MAX SIZE_MAX

// Pointers
typedef __INTPTR_TYPE__ iptr;
typedef __UINTPTR_TYPE__ uptr;

#define UPTR_MAX UINTPTR_MAX