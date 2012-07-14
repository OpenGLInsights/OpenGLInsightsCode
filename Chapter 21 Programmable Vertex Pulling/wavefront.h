/*
 * wavefront.h
 *
 *  Created on: Jan 24, 2010
 *      Author: aqnuep
 *
 *  Note: Do not use this module in any product as it is a very rough
 *        wavefront object loader made only for the demo program and
 *        is far from product quality
 */

#ifndef WAVEFRONT_H_
#define WAVEFRONT_H_

#include <string>
#include <vector>
#include <set>
#include <GL/gl.h>

using namespace std;

namespace demo {

struct Vertex { float x, y, z; };
struct Normal { float x, y, z; };
typedef GLint Index;

class IndexGroup {
public:
	int i, v, n;
	bool operator ==(const IndexGroup& ig) {
		return (this->v == ig.v) and (this->n == ig.n);
	}
};

struct IndexGroupLess : public binary_function<IndexGroup, IndexGroup, bool>
{
  bool
  operator()(const IndexGroup& __x, const IndexGroup& __y) const
  { return __x.v < __y.v; }
};

class WaveFrontObj {
public:
	WaveFrontObj(const char* filename);
	void dump();

    vector<Vertex> vertex;
    vector<Normal> normal;
    vector<Index> index;
    set<IndexGroup, IndexGroupLess> indexGroup;
};

} /* namespace demo */

#endif /* WAVEFRONT_H_ */
