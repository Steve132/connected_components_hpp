#ifndef TRUE_ATOMIC_HPP
#define TRUE_ATOMIC_HPP

#ifdef __linux__
#include<linux/cache.h>
#endif

#if defined(__x86_64__) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64)
	#define L1_CACHE_BYTES 64
#endif

#include<new>
#include<atomic>
#include<cstdint>
#include<array>
#include<type_traits>
#include<memory>

#ifdef __cpp_lib_hardware_interference_size
	using std::hardware_constructive_interference_size;
	using std::hardware_destructive_interference_size;
#elifdef L1_CACHE_BYTES
	constexpr std::size_t hardware_constructive_interference_size = L1_CACHE_BYTES;
	constexpr std::size_t hardware_destructive_interference_size = L1_CACHE_BYTES;
#else
	constexpr std::size_t hardware_constructive_interference_size = 256;
	constexpr std::size_t hardware_destructive_interference_size = 256;
#endif


//general case
//atomic weak_ptr and atomic_shared_ptr are covered by the general case.
template<class T,class Enable=void>
struct alignas(hardware_constructive_interference_size) 
true_atomic:
	public std::atomic<T>
{
public:
	using std::atomic<T>::atomic;
	using std::atomic<T>::operator T;
	using std::operator=;
private:
	std::uint8_t pad[hardware_constructive_interference_size-sizeof(std::atomic<T>)];
};

template<class Integral>
struct alignas(hardware_constructive_interference_size) 
true_atomic<Integral,typename std::enable_if<std::is_integral<Integral>::value>::type>:
	public std::atomic<Integral>
{
public:
	using std::atomic<Integral>::atomic;
	using std::atomic<Integral>::operator Integral;
	using std::operator=;

	using std::atomic<Integral>::operator++;
	using std::atomic<Integral>::operator--;
	using std::atomic<Integral>::operator+=;
	using std::atomic<Integral>::operator-=;
	using std::atomic<Integral>::operator&=;
	using std::atomic<Integral>::operator|=;
	using std::atomic<Integral>::operator^=;

private:
	std::uint8_t pad[hardware_constructive_interference_size-sizeof(std::atomic<T>)];
};

template<class T>
struct alignas(hardware_constructive_interference_size) 
true_atomic<T,typename std::enable_if<std::is_floating_point<T>::value>::type>:
	public std::atomic<T>
{
public:
	using std::atomic<T>::atomic;
	using std::atomic<T>::operator T;
	using std::operator=;

	using std::atomic<T>::operator+=;
	using std::atomic<T>::operator-=;
private:
	std::uint8_t pad[hardware_constructive_interference_size-sizeof(std::atomic<T>)];
};

template<class T>
struct alignas(hardware_constructive_interference_size) 
true_atomic<T,typename std::enable_if<std::is_pointer<T>::value>::type>:
	public std::atomic<T>
{
public:
	using std::atomic<T>::atomic;
	using std::atomic<T>::operator T;
	using std::operator=;

	using std::atomic<T>::operator++;
	using std::atomic<T>::operator--;
	using std::atomic<T>::operator+=;
	using std::atomic<T>::operator-=;
private:
	std::uint8_t pad[hardware_constructive_interference_size-sizeof(std::atomic<T>)];
};



#endif
