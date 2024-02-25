#pragma once

#define RELUNIQUE(Class, Method) template<uint64_t ID, uint64_t Offset, typename FunctionType = decltype(&Class::Method)>
#define REL_WriteUniqueCall(ID, Offset, functionHook) REL::WriteUniqueCall<ID, Offset>(functionHook<ID, Offset>)
#define REL_WriteUniqueJump(ID, Offset, functionHook) REL::WriteUniqueJump<ID, Offset>(functionHook<ID, Offset>)
#define REL_CallOriginal REL::CallUniqueOriginal<ID, Offset, FunctionType>

namespace REL
{
	template<uint64_t ID, uint64_t Offset>
	__forceinline std::uintptr_t& GetUniqueOriginalPointer()
	{
		static std::uintptr_t original;
		return original;
	}
	template<uint64_t ID, uint64_t Offset, typename T>
	__forceinline void WriteUniqueCall(T functionHook)
	{
		GetUniqueOriginalPointer<ID, Offset>() = SKSE::GetTrampoline().write_call<5>(REL::ID(ID).address() + Offset, functionHook);
	}
	template<uint64_t ID, uint64_t Offset, typename T>
	__forceinline void WriteUniqueJump(T functionHook)
	{
		GetUniqueOriginalPointer<ID, Offset>() = SKSE::GetTrampoline().write_branch<5>(REL::ID(ID).address() + Offset, functionHook);
	}
	template<uint64_t ID, uint64_t Offset, typename T, typename... Args>
	__forceinline static auto CallUniqueOriginal(Args... args)
	{
		REL::Relocation<T> original{ GetUniqueOriginalPointer<ID, Offset>() };
		return original(args...);
	}
}