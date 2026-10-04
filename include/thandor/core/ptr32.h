/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/ptr32.h
 */

#ifndef THANDOR_CORE_PTR32_H
#define THANDOR_CORE_PTR32_H

/*
32-bit pointer fields of the original data layouts (step 5f).

The structs of the type headers (<area>/<module>/types.h) are the original's 32-bit layouts: file and
asset images, save-game images (pools written raw), UI templates linked by byte offsets, and runtime records
that code walks with the original strides and offsets. Their pointer fields are declared Ptr32<T> (data
pointers) or Ptr32<R(args)> (function pointers). Ptr32<T> is a 4-byte field holding the pointer as a signed
32-bit value. The exe is linked /LARGEADDRESSAWARE:NO, so every address of the process (image, heap, stacks) is
below 2 GB and fits; sign extension on reading keeps sentinels like (T *)-1 intact. A pointer that does not fit
stops the game (Thandor_Ptr32Overflow). Savegames, assets and the UI templates depend on these layouts.
UPtr32 is the same for a field the code keeps as uintptr_t (a pointer or a small value).
The field converts to T * implicitly, so most code reads and writes it like a pointer. What does not work
(compile errors): taking the field's address as a T **, ?: between the field and a T *, and pointer-typed
references to it.
*/

#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
#include <type_traits>
#endif

#ifdef __cplusplus
/* THANDOR_FN(function) / THANDOR_PTR(pointer) values (core/contracts.h). */
struct ThandorAnyFn {
    void (*fn)();
    template <class F> operator F *() const { return (F *)fn; }
};
struct ThandorAnyPtr {
    void *ptr;
    template <class T> operator T *() const { return (T *)ptr; }
};
/* THANDOR_SLOT(function) values (core/slot.h). */
template <auto Fn> struct ThandorSlot;

/* Stops the game with a log entry: a pointer of 2 GB or more was stored in a 32-bit field. */
[[noreturn]] void Thandor_Ptr32Overflow(uintptr_t value);

static __forceinline int32_t thandor_ptr32_pack(const volatile void *pointer)
{
    intptr_t value = (intptr_t)pointer;
    if ((intptr_t)(int32_t)value != value) {
        Thandor_Ptr32Overflow((uintptr_t)value);
    }
    return (int32_t)value;
}

template <class T> struct Ptr32 {
    int32_t value;

    Ptr32() = default;
    Ptr32(T *pointer) : value(thandor_ptr32_pack((const void *)pointer)) {}
    Ptr32(ThandorAnyPtr pointer) : value(thandor_ptr32_pack(pointer.ptr)) {}
    Ptr32(ThandorAnyFn function) : value(thandor_ptr32_pack((const void *)function.fn)) {}
    /* field = THANDOR_SLOT(function), also as an element of an array initialiser (an exact match, so MSVC does not
       weigh it against Ptr32(T *) over the slot's function-pointer conversion) */
    template <auto Fn> Ptr32(ThandorSlot<Fn>) : Ptr32(ThandorSlot<Fn>::template pick<T>()) {}
    Ptr32 &operator=(T *pointer) { value = thandor_ptr32_pack((const void *)pointer); return *this; }
    Ptr32 &operator=(ThandorAnyPtr pointer) { value = thandor_ptr32_pack(pointer.ptr); return *this; }
    Ptr32 &operator=(ThandorAnyFn function) { value = thandor_ptr32_pack((const void *)function.fn); return *this; }
    /* field = THANDOR_SLOT(function) (the conversions alone would be ambiguous between operator=(T *) and the
       copy assignment) */
    template <auto Fn> Ptr32 &operator=(ThandorSlot<Fn>) { return *this = ThandorSlot<Fn>::template pick<T>(); }

    T *get() const { return (T *)(intptr_t)value; }
    operator T *() const { return get(); }
    T *operator->() const { return get(); }
    /* (U *)field and (integer)field as for a pointer */
    template <class U> explicit operator U *() const { return (U *)(intptr_t)value; }
    explicit operator int32_t() const { return value; }
    explicit operator uint32_t() const { return (uint32_t)value; }
    explicit operator intptr_t() const { return (intptr_t)value; }
    explicit operator uintptr_t() const { return (uintptr_t)(intptr_t)value; }

    Ptr32 &operator++() { *this = get() + 1; return *this; }
    Ptr32 &operator--() { *this = get() - 1; return *this; }
    T *operator++(int) { T *old = get(); *this = old + 1; return old; }
    T *operator--(int) { T *old = get(); *this = old - 1; return old; }
    Ptr32 &operator+=(ptrdiff_t count) { *this = get() + count; return *this; }
    Ptr32 &operator-=(ptrdiff_t count) { *this = get() - count; return *this; }
};

/* A pointer-or-value field kept as uintptr_t by the code (UPtr32): 4 bytes on x64, read sign-extended. */
struct UPtr32 {
    int32_t value;

    UPtr32() = default;
    UPtr32(uintptr_t bits) : value(thandor_ptr32_pack((const void *)bits)) {}
    UPtr32 &operator=(uintptr_t bits) { value = thandor_ptr32_pack((const void *)bits); return *this; }
    operator uintptr_t() const { return (uintptr_t)(intptr_t)value; }
    template <class U> explicit operator U *() const { return (U *)(intptr_t)value; }
    UPtr32 &operator+=(uintptr_t count) { return *this = (uintptr_t)*this + count; }
    UPtr32 &operator-=(uintptr_t count) { return *this = (uintptr_t)*this - count; }
};

/*
Explicit conversions between a pointer and a 32-bit value of the original layouts, for the places that keep a
pointer in a plain 32-bit integer (a field that holds a pointer or an offset/id, a saved offset computed from two
pointers, a pointer dword of a UI template). They replace the plain (uint32_t)pointer / (int)pointer /
(T *)value casts, which truncate silently and are compile errors in g++.

Thandor_PointerToU32(pointer) / Thandor_PointerToI32(pointer): the pointer's address as 32 bits, the same value
the plain cast gave; a pointer of 2 GB or more (which a 32-bit field cannot hold) stops the game
(Thandor_Ptr32Overflow), as for a Ptr32 field. Data and function pointers.

Thandor_U32ToPointer<T>(value): the T * of a 32-bit value (any integer type of up to 4 bytes), sign-extended like
a Ptr32 field: addresses below 2 GB come back unchanged and sentinels like 0xFFFFFFFF become (T *)-1. A wider
integer (uintptr_t) does not compile: it is not a 32-bit value.
*/
template <class P> static __forceinline int32_t Thandor_PointerToI32(P *pointer)
{
    return thandor_ptr32_pack((const volatile void *)pointer);
}
template <class P> static __forceinline uint32_t Thandor_PointerToU32(P *pointer)
{
    return (uint32_t)thandor_ptr32_pack((const volatile void *)pointer);
}
/* A Ptr32 field already holds the 32-bit value. */
template <class P> static __forceinline int32_t Thandor_PointerToI32(const Ptr32<P> &field)
{
    return field.value;
}
template <class P> static __forceinline uint32_t Thandor_PointerToU32(const Ptr32<P> &field)
{
    return (uint32_t)field.value;
}
template <class T = void, class I> static __forceinline T *Thandor_U32ToPointer(I value)
{
    static_assert((std::is_integral_v<I> || std::is_enum_v<I>) && sizeof(I) <= 4,
                  "Thandor_U32ToPointer takes a 32-bit integer");
    return (T *)(intptr_t)(int32_t)value;
}
#endif /* __cplusplus */

#ifdef __cplusplus
/* The 32-bit pointer slot (a Ptr32<T> lvalue) at address p inside an original layout: a pointer in a text
   command stream, a pointer field reached by byte offset. */
#define THANDOR_PTR32_AT(T, p) (*(Ptr32<T> *)(p))
#endif

/* Bytes of a 32-bit pointer field (sizeof(Ptr32<void>)). */
#define THANDOR_PTR32_BYTES 4

#endif
