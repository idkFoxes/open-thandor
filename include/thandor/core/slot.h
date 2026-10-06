/*
 * Open Thandor
 * Project: https://github.com/idkFoxes/open-thandor/tree/main
 * File: https://github.com/idkFoxes/open-thandor/blob/main/include/thandor/core/slot.h
 */

#ifndef THANDOR_CORE_SLOT_H
#define THANDOR_CORE_SLOT_H

/*
Typed table entries (step 8): THANDOR_SLOT(function) for a function-pointer slot of a vtable or callback table
(a Ptr32<R(A...)> field or a plain R (*)(A...)), replacing the untyped THANDOR_FN(function).

The value converts to the slot's type it initialises:
- the function has exactly the slot's signature: the function itself (the same address as &function);
- the parameters differ only in well-defined ways: a small thunk with exactly the slot's signature that converts
  each argument and calls the function (usually a single jmp). Allowed per parameter (slot type -> function type):
    * the same type;
    * B * -> D * where D is registered as starting with B (THANDOR_SLOT_PREFIX(D, member): D is standard-layout and
      its first member, at offset 0, is a B), also over several registrations (UiImageControl -> UiSelectableControl
      -> UiNodeBase), and the other way round D * -> B *;
    * B * -> V * where V is registered as a view of the same memory (THANDOR_SLOT_OVERLAY(V, B), or of a type
      reached from B by prefixes);
    * void * -> T * and T * -> void *;
  const may be added to the pointee, never dropped.
- anything else (other arity, other return type, other scalar parameter types such as int vs uint32_t or Bool8 vs
  int) does not compile (static_assert "THANDOR_SLOT: ..."). Those need a hand-written adapter whose conversion is
  visible in the code.

UI_SLOT(function) is the same, for UI vtables and handler tables.

Both GCC and MSVC deduce the slot's signature from the field's type (the Ptr32<R(A...)> constructor and
assignment from a ThandorSlot in core/ptr32.h, the conversion function template to R (*)(A...)), also for the
elements of an array: `.pointerMove = THANDOR_SLOT(UiImageControl_PointerMove)`. Where a context gives no target
type (auto, a template argument, ?:), name it: ThandorSlot<&function>::pick<R(A...)>().

The registrations of the UI node types are in ui/controls/node_views.h, ui/ingame/node_views.h and
ui/frontend/node_views.h.
*/

#include <stddef.h>
#include <thandor/core/ptr32.h>
#ifdef __cplusplus
#include <type_traits>

/* THANDOR_SLOT_PREFIX(T, member): T starts with `member` (offset 0), so a pointer to that member's type may be
   reinterpreted as a T * by a slot thunk. One registration per type, at namespace scope after T is complete;
   `member` may be a path (rootUi.base). */
template <class T> struct ThandorSlotPrefixOf {
    using type = void;
};
#define THANDOR_SLOT_PREFIX(T, member)                                                                          \
    template <> struct ThandorSlotPrefixOf<T> {                                                                 \
        using type = std::remove_cv_t<decltype(static_cast<T *>(nullptr)->member)>;                             \
        static_assert(std::is_class_v<type> && std::is_standard_layout_v<T> && std::is_standard_layout_v<type>, \
                      "THANDOR_SLOT_PREFIX(" #T ", " #member "): both types must be standard-layout structs");  \
        static_assert(offsetof(T, member) == 0,                                                                 \
                      "THANDOR_SLOT_PREFIX(" #T ", " #member "): the member is not at offset 0");               \
    }

/* THANDOR_SLOT_OVERLAY(View, Base): View is a view of the memory of a Base (an overlay that does not start with a
   Base member, e.g. reserved bytes followed by the fields a function reads), so a Base * may be reinterpreted as a
   View * by a slot thunk. One registration per view. */
template <class T> struct ThandorSlotOverlayOf {
    using type = void;
};
#define THANDOR_SLOT_OVERLAY(View, Base)                                                                        \
    template <> struct ThandorSlotOverlayOf<View> {                                                             \
        using type = Base;                                                                                      \
        static_assert(std::is_standard_layout_v<View> && std::is_standard_layout_v<Base>,                       \
                      "THANDOR_SLOT_OVERLAY(" #View ", " #Base "): both types must be standard-layout structs"); \
    }

/* A T object is (starts with, or is registered as a view of) a B object. */
template <class T, class B> constexpr bool thandor_slot_is_view_of()
{
    if constexpr (std::is_same_v<T, B>) {
        return true;
    } else {
        using Prefix = typename ThandorSlotPrefixOf<T>::type;
        using Overlay = typename ThandorSlotOverlayOf<T>::type;
        bool viaPrefix = false;
        bool viaOverlay = false;
        if constexpr (!std::is_void_v<Prefix>) {
            viaPrefix = thandor_slot_is_view_of<Prefix, B>();
        }
        if constexpr (!std::is_void_v<Overlay>) {
            viaOverlay = thandor_slot_is_view_of<Overlay, B>();
        }
        return viaPrefix || viaOverlay;
    }
}

/* Slot parameter type P passed to a function parameter of type F. */
template <class P, class F> constexpr bool thandor_slot_arg_ok()
{
    if constexpr (std::is_same_v<P, F>) {
        return true;
    } else if constexpr (std::is_pointer_v<P> && std::is_pointer_v<F>) {
        using PT = std::remove_pointer_t<P>;
        using FT = std::remove_pointer_t<F>;
        using PU = std::remove_cv_t<PT>;
        using FU = std::remove_cv_t<FT>;
        if constexpr ((std::is_const_v<PT> && !std::is_const_v<FT>) ||
                      (std::is_volatile_v<PT> && !std::is_volatile_v<FT>)) {
            return false;
        } else if constexpr (std::is_function_v<PU> || std::is_function_v<FU>) {
            return false;
        } else if constexpr (std::is_void_v<PU> || std::is_void_v<FU>) {
            return true;
        } else {
            return thandor_slot_is_view_of<FU, PU>() || thandor_slot_is_view_of<PU, FU>();
        }
    } else {
        return false;
    }
}

template <class P, class F> static __forceinline F thandor_slot_arg(P value)
{
    if constexpr (std::is_same_v<P, F>) {
        return value;
    } else if constexpr (std::is_void_v<std::remove_cv_t<std::remove_pointer_t<P>>> ||
                         std::is_void_v<std::remove_cv_t<std::remove_pointer_t<F>>>) {
        return static_cast<F>(value);
    } else {
        return reinterpret_cast<F>(value);
    }
}

template <class Sig> struct ThandorSlotSignature;
template <class R, class... A> struct ThandorSlotSignature<R(A...)> {
    using result = R;
    static constexpr size_t arity = sizeof...(A);
};

template <class SlotSig, class FnSig> struct ThandorSlotArgsOk;
template <class R1, class... A, class R2, class... F> struct ThandorSlotArgsOk<R1(A...), R2(F...)> {
    static constexpr bool value = (thandor_slot_arg_ok<A, F>() && ...);
};

/* The thunk of function Fn (signature FnSig) for a slot of signature SlotSig. */
template <auto Fn, class SlotSig, class FnSig> struct ThandorSlotThunk;
template <auto Fn, class R, class... A, class... F> struct ThandorSlotThunk<Fn, R(A...), R(F...)> {
    static R call(A... args) { return Fn(thandor_slot_arg<A, F>(args)...); }
};

template <class R, class... A> using ThandorSlotFunctionPointer = R (*)(A...);

template <auto Fn> struct ThandorSlot {
    static_assert(std::is_pointer_v<decltype(Fn)> && std::is_function_v<std::remove_pointer_t<decltype(Fn)>>,
                  "THANDOR_SLOT takes a function");
    using FnSig = std::remove_pointer_t<decltype(Fn)>;

    /* The entry for a slot of signature SlotSig: Fn itself or its thunk. */
    template <class SlotSig> static constexpr SlotSig *pick()
    {
        using Slot = ThandorSlotSignature<SlotSig>;
        using Function = ThandorSlotSignature<FnSig>;
        if constexpr (std::is_same_v<SlotSig, FnSig>) {
            return Fn;
        } else if constexpr (Slot::arity != Function::arity) {
            static_assert(Slot::arity == Function::arity,
                          "THANDOR_SLOT: the function has a different number of parameters than the slot");
            return nullptr;
        } else if constexpr (!std::is_same_v<typename Slot::result, typename Function::result>) {
            static_assert(std::is_same_v<typename Slot::result, typename Function::result>,
                          "THANDOR_SLOT: the function returns a different type than the slot (write an adapter)");
            return nullptr;
        } else if constexpr (!ThandorSlotArgsOk<SlotSig, FnSig>::value) {
            static_assert(ThandorSlotArgsOk<SlotSig, FnSig>::value,
                          "THANDOR_SLOT: a parameter of the function is neither the slot's type nor reachable from it "
                          "by a registered prefix/overlay or void * (write an adapter)");
            return nullptr;
        } else {
            return &ThandorSlotThunk<Fn, SlotSig, FnSig>::call;
        }
    }

    template <class R, class... A> constexpr operator ThandorSlotFunctionPointer<R, A...>() const
    {
        return pick<R(A...)>();
    }
};

#define THANDOR_SLOT(fn) (ThandorSlot<&fn>{})
#define UI_SLOT(fn) THANDOR_SLOT(fn)
#endif /* __cplusplus */

#endif
