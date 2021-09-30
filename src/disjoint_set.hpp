#ifndef DISJOINT_SET_HPP
#define DISJOINT_SET_HPP

#include<vector>
#include<algorithm>
#include<numeric>

template<class label_uint_t>
class disjoint_set
{
private:
	std::vector<label_uint_t> sizes;
	std::vector<label_uint_t> parents;
public:
	disjoint_set(label_uint_t N=0) {
		reset(N);
	}
	void reset(label_uint_t N){
		parents.resize(N);
		sizes.resize(N);
		std::iota(parents.begin(),parents.end(),0);
		std::fill(sizes.begin(),sizes.end(),1);
	}

	label_uint_t Find(label_uint_t x) {
		while(parents[x] != x)
		{
			label_uint_t px=parents[x];
			parents[x]=parents[px];
			x=px;
		}
		return x;
	}
	void Union(label_uint_t x,label_uint_t y) {
		label_uint_t xroot=Find(x);
		label_uint_t yroot=Find(y);
		
		if(xroot==yroot) return;
		label_uint_t xsz=sizes[xroot];
		label_uint_t ysz=sizes[yroot];
		if(xsz < ysz){
			std::swap(xroot,yroot);
		}
		sizes[xroot]=xsz+ysz;
		parents[yroot]=xroot;
	}
	label_uint_t size() const {
		if(parents.size()==0) return 0;
		return (*std::max_element(parents.cbegin(),parents.cend()))+1;
	}
};

#endif
