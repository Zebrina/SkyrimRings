#pragma once

#ifndef DefineUniqueFunctionHook
#define DefineUniqueFunctionHook(source, functionType) template<uint64_t ID, uint64_t Offset, class T = source, class S = source, typename F = functionType>
#else
#error DefineUniqueFunctionHook already defined.
#endif
#ifndef WriteUniqueFunctionHookCall
#define WriteUniqueFunctionHookCall(source, id, offset, function) RELX::__WriteUniqueFunctionHookCall<id, offset, source>(function<id, offset, source>)
#else
#error WriteUniqueFunctionHookCall already defined.
#endif
#ifndef WriteUniqueFunctionHookJump
#define WriteUniqueFunctionHookJump(source, id, offset, function) RELX::__WriteUniqueFunctionHookJump<id, offset, source>(function<id, offset, source>)
#else
#error WriteUniqueFunctionHookJump already defined.
#endif
#ifndef CallOriginalFunction
#define CallOriginalFunction RELX::__CallOriginalFunction<ID, Offset, T, S, F>
#else
#error CallOriginalFunction already defined.
#endif

#ifndef DefineVirtualFunctionHook
#define DefineVirtualFunctionHook(source, type, method) template<uint64_t ID = 0, uint64_t Offset = 0, class T = type, class S = source, typename F = decltype(&type::method)>
#else
#error DefineVirtualFunctionHook already defined.
#endif
#ifndef WriteVirtualFunctionHook
#define WriteVirtualFunctionHook(source, type, vtable, offset, function) RELX::__WriteVirtualFunctionHook<source, type>(vtable, offset, function<0, 0, source, type>)
#else
#error WriteVirtualFunctionHook already defined.
#endif

namespace RELX
{
	template<uint64_t ID, uint64_t Offset, class T, class S>
	uintptr_t __OriginalFunctionPointer;

	template<uint64_t ID, uint64_t Offset, class S, typename F>
	__forceinline void __WriteUniqueFunctionHookCall(F function)
	{
		__OriginalFunctionPointer<ID, Offset, S, S> = SKSE::GetTrampoline().write_call<5>(REL::ID(ID).address() + Offset, function);
	}
	template<uint64_t ID, uint64_t Offset, class S, typename F>
	__forceinline void __WriteUniqueFunctionHookJump(F function)
	{
		__OriginalFunctionPointer<ID, Offset, S, S> = SKSE::GetTrampoline().write_branch<5>(REL::ID(ID).address() + Offset, function);
	}
	template<uint64_t ID, uint64_t Offset, class T, class S, typename F, typename... Args>
	__forceinline auto __CallOriginalFunction(Args... args)
	{
		REL::Relocation<F> original{ __OriginalFunctionPointer<ID, Offset, T, S> };
		return original(args...);
	}

	template<class T, class S, typename F>
	__forceinline void __WriteVirtualFunctionHook(const REL::ID& vtable, size_t offset, F function)
	{
		__OriginalFunctionPointer<0, 0, T, S> = REL::Relocation<uintptr_t>(vtable).write_vfunc(offset, function);
	}
}