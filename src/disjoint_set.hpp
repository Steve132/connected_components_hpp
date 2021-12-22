#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#include<vector>
#include<algorithm>
#include<numeric>
#include<cstdint>

//https://arxiv.org/pdf/1911.06347.pdf
template<class label_uint_t,class storage_uint_t=label_uint_t>
class disjoint_set
{
protected:
	mutable std::vector<storage_uint_t> parents;
	storage_uint_t cur_groups;

	static bool prand_cmp(uint32_t ux,uint32_t uy) {
		ux ^= uy;
		uint64_t v=ux;
		v*=3499239749UL;

		return (v >> 32) & 0x1;
	}

public:
	disjoint_set(size_t N=0) {
		reset(N);
	}
	void reset(size_t N){
		parents.resize(N);
		cur_groups=(storage_uint_t)N;
		std::iota(parents.begin(),parents.end(),0);
	}

	label_uint_t Find(label_uint_t x) const {
		label_uint_t px;

		while((px=parents[x]) != x)
		{
			label_uint_t ppx=parents[px];
			if(ppx == px) return px;
			parents[x]=ppx;
			x=px;
		}
		return x;
	}

	label_uint_t Union(label_uint_t x,label_uint_t y) {
		label_uint_t xroot=Find(x);
		label_uint_t yroot=Find(y);

		if(xroot==yroot) return;

		if(prand_cmp(xroot,yroot)){
			std::swap(xroot,yroot);
		}
		parents[yroot]=xroot;
		cur_groups--;
		return xroot;
	}
	size_t num_groups() const {
		return (size_t)cur_groups;
	}
};


#if __cplusplus >= 202002L
// C++20 (and later) code

#include<atomic>

template<class label_uint_t>
class parallel_disjoint_set: private disjoint_set<label_uint_t>
{
public:
	using dstype=disjoint_set<label_uint_t>;
	parallel_disjoint_set(size_t N=0) {
		reset(N);
	}
	void reset(size_t N){
		dstype::reset(N);
		(std::atomic_ref<size_t>(dstype::cur_groups))=N;
	}
	size_t num_groups() const {
		return std::atomic_ref<size_t>(dstype::cur_groups);
	}
	label_uint_t Find(label_uint_t x) const {
		label_uint_t px;

		while((px=std::atomic_ref<label_uint_t>(dstype::parents[x])) != x)
		{
			label_uint_t ppx=std::atomic_ref<label_uint_t>(disjoint_set<label_uint_t>::parents[px]);
			if(ppx == px) return px;
			dstype::parents[x]=ppx; //ON PURPOSE.  This writeback isn't needed for correctness so it can be non-atomic
			x=px;
		}
		return x;
	}

	label_uint_t Union(label_uint_t x,label_uint_t y) {
		label_uint_t xroot=Find(x);
		label_uint_t yroot=Find(y);

		if(xroot==yroot) return;

		if(prand_cmp(xroot,yroot)){
			std::swap(xroot,yroot);
		}
		(std::atomic_ref<label_uint_t>(dstype::parents[yroot]))=xroot;
		(std::atomic_ref<label_uint_t>(dstype::cur_groups))--;
		return xroot;
	}
};

#else

#include<atomic>

template<class label_uint_t>
using parallel_disjoint_set=disjoint_set<label_uint_t,std::atomic<label_uint_t>>;

#endif
#endif


	void Union(label_uint_t x,label_uint_t y) {
		label_uint_t xroot=Find(x);
		label_uint_t yroot=Find(y);

		if(xroot==yroot) return;

		if(prand_cmp(xroot,yroot)){
			std::swap(xroot,yroot);
		}
		parents[yroot]=xroot;
		cur_groups--;
	}
	size_t num_groups() const {
		return (size_t)cur_groups;
	}
};


#if __cplusplus >= 202002L
// C++20 (and later) code

#include<atomic>

template<class label_uint_t>
class parallel_disjoint_set: private disjoint_set<label_uint_t>
{
public:
    using dstype=disjoint_set<label_uint_t>;
	parallel_disjoint_set(size_t N=0) {
		reset(N);
	}
	void reset(size_t N){
		dstype::reset(N);
		(std::atomic_ref<size_t>(dstype::cur_groups))=N;
	}
	size_t num_groups() const {
		return std::atomic_ref<size_t>(dstype::cur_groups);
	}
	label_uint_t Find(label_uint_t x) const {
		label_uint_t px;

		while((px=std::atomic_ref<label_uint_t>(dstype::parents[x])) != x)
		{
			label_uint_t ppx=std::atomic_ref<label_uint_t>(disjoint_set<label_uint_t>::parents[px]);
			if(ppx == px) return px;
			dstype::parents[x]=ppx; //ON PURPOSE.  This writeback isn't needed for correctness so it can be non-atomic
			x=px;
		}
		return x;
	}

	void Union(label_uint_t x,label_uint_t y) {
		label_uint_t xroot=Find(x);
		label_uint_t yroot=Find(y);

		if(xroot==yroot) return;

		if(prand_cmp(xroot,yroot)){
			std::swap(xroot,yroot);
		}
		(std::atomic_ref<label_uint_t>(dstype::parents[yroot]))=xroot;
		(std::atomic_ref<label_uint_t>(dstype::cur_groups))--;
	}
};

#else

#include<atomic>

template<class label_uint_t>
using parallel_disjoint_set=disjoint_set<label_uint_t,std::atomic<label_uint_t>>;

#endif
#endif
