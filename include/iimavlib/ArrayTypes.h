/**
 * @file 	arrayTypes.h
 *
 * @date 	29.4.2016
 * @author 	Roamn Berka <berka@fel.cvut.cz>
 * @copyright GNU Public License 3.0
 *
 */


#ifndef INCLUDE_IIMAVLIB_ARRAYTYPES_H_
#define INCLUDE_IIMAVLIB_ARRAYTYPES_H_


#include <cmath>
#include <complex>
#include <vector>
#include <string>

#include "Utils.h"

namespace iimavlib {

template <class T>
using complexarray_t = std::vector<std::complex<T> >;

template <class T>
using simplearray_t = std::vector<T>;


template <typename T>
std::vector<T> operator+(const std::vector<T> &A, const std::vector<T> &B);

template <typename T>
std::vector<T> &operator+=(std::vector<T> &A, const std::vector<T> &B);

template <class T>
std::ostream & operator<<(std::ostream &o, const complexarray_t<T>& v);

template <class T>
std::ostream & operator<<(std::ostream &o, const simplearray_t<T>& v);

template <class T>
class matrix {
	std::vector<T> m;
	int rows_;
	int columns_;

public:
	/* *******************************************************************
	 *     Constructors
	 ******************************************************************* */
	matrix():
		rows_(0), columns_(0)
	{}
	matrix(int rows, int columns = 1):
		m(rows * columns), rows_(rows),columns_(columns)
	{}

	matrix(std::initializer_list<T> l):
		m(l), rows_(l.size()), columns_(1)
	{}

	matrix(const std::vector<T> &v, int rows, int columns = 1):
		m(v),rows_(rows),columns_(columns)
	{
		if (m.size() != static_cast<size_t>(rows * columns)) {
			throw std::runtime_error("Wrong array dimmensions");
		}
	}

	/* *******************************************************************
	 *     Matrix manipulation
	 ******************************************************************* */

	void set_matrix(int rows, int columns);
	matrix& sequence();
	matrix<T> operator*(const matrix<T> &v);
	template <class U>
	matrix<T> operator *(const simplearray_t<U> &v);
	matrix<T> transpose();
	matrix<T>& self_transpose();
	T& operator[](int ix);
	simplearray_t<T> data();

	/* *******************************************************************
	 *     Methods for textual output
	 ******************************************************************* */
	template <class U>
	friend std::ostream& operator<<(std::ostream &o, const matrix<U>& mm);
	void set(const int a, const int b, T val);
	void to();
	void info();
};


template <typename T>
std::vector<T> operator+(const std::vector<T> &A, const std::vector<T> &B)
{
    std::vector<T> AB;
    AB.reserve( A.size() + B.size() );
    AB.insert( AB.end(), A.begin(), A.end() );
    AB.insert( AB.end(), B.begin(), B.end() );
    return AB;
}

template <typename T>
std::vector<T> &operator+=(std::vector<T> &A, const std::vector<T> &B)
{
    A.reserve( A.size() + B.size() );
    A.insert( A.end(), B.begin(), B.end() );
    return A;
}

template <class T>
std::ostream & operator<<(std::ostream &o, const complexarray_t<T>& v) {
	o << "[";
	for (typename complexarray_t<T>::const_iterator a=v.begin();a!=v.end();++a)
		o << *a << " ";
	o << "]\n";
	return o;
}

template <class T>
std::ostream & operator<<(std::ostream &o, const simplearray_t<T>& v) {
	o << "[";
	for (typename simplearray_t<T>::const_iterator a=v.begin();a!=v.end();++a)
		o << *a << " ";
	o << "]\n";
	return o;
}

template<class U>
std::ostream& operator<<(std::ostream &o, const matrix<U>& mm) {
	for(int i = 0; i < mm.rows_; ++i) {
		o << mm.m[i * mm.columns_ + 0];
			for(int j = 1;j < mm.columns_; ++j) {
				o << ", " << mm.m[i*mm.columns_+j];
			}
			o << "\n";
	}
	return o;
}

template<class T>
void matrix<T>::set_matrix(int r, int c) {
	m.resize(r*c);
	rows_ = r;
	columns_ = c;
}

template<class T>
matrix<T>& matrix<T>::sequence() {
	T f=0;
	for (auto& s:m)
		s = f++;
	return *this;
}

template<class T>
matrix<T> matrix<T>::operator *(const matrix<T> &v) {
	if (v.rows_ != columns_) {
		throw std::runtime_error("Wrong dimensions of second matrix in matrix::operator*");
	}
	const auto& columns2 = v.columns_;
	matrix<T> w(rows_, columns2);
	for(auto x = 0; x < rows_; ++x) {
		for(auto y = 0; y < columns_; ++y) {
			const auto index = x * columns2 + y;
			w[index] = 0;
			for(auto z = 0; z < columns_; ++z)
				w[index] += m[x * columns_ + z] * v.m[z * columns2 + y];
		}
	}
	return w;
}

template<class T>
template <class U>
matrix<T> matrix<T>::operator *(const simplearray_t<U> &v) {
	if (v.size() != static_cast<size_t>(columns_)) {
		throw std::runtime_error("Wrong dimensions of simple_array in matrix::operator*");
	}

	matrix<T> w(rows_, 1);
	for(auto x = 0; x < rows_; ++x) {
		const auto index = x;
		w[index] = 0;
		for(auto z = 0; z < columns_; ++z) {
			w[index] += m[x * columns_ + z] * v[z];
		}

	}
	return w;
}

template<class T>
simplearray_t<T> matrix<T>::data() {
	return m;
}

template<class T>
matrix<T>& matrix<T>::self_transpose() {
	using std::swap;
	if (rows_ == columns_) {
		for(auto i=0;i<rows_;i++) {
			for(auto j=i+1;j<columns_;j++) {
				swap(m[j * columns_ + i], m[i * columns_ + j]);
			}
		}
	} else {
		auto transposed_matrix = transpose();
		m = transposed_matrix.data();
	}
	swap(columns_, rows_);
	return *this;
}

template<class T>
matrix<T> matrix<T>::transpose() {
	matrix<T> w(columns_, rows_);
	for(auto i = 0; i < rows_; ++i)
		for(auto j = 0; j < columns_; ++j) {
			w[j * rows_ + i] = m[i * columns_ + j];
		}
	return w;
}

template<class T>
T& matrix<T>::operator [](int ix) {
	return m[ix];
}

template<class T>
void matrix<T>::to() {
	logger[log_level::info] << "TO:";
	auto l = logger[log_level::info] << "SIZE: " << m.size() << ": ";
	for(auto s:m) {
		l << s << ",";
	}
}

template<class T>
void matrix<T>::info() {
	logger[log_level::info] << "INFO:";
	logger[log_level::info] << "SIZE:" << m.size();
	logger[log_level::info] << "CAPACITY:" << m.capacity();
	logger[log_level::info] << "MAX_SIZE:" << m.max_size();
}

template<class T>
void matrix<T>::set(const int a, const int b, T val) {
	m[a*columns_+b] = val;
}


}
#endif /* INCLUDE_IIMAVLIB_ARRAYTYPES_H_ */
