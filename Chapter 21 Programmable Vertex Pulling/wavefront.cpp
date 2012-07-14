/*
 * wavefront.cpp
 *
 *  Created on: Jan 24, 2010
 *      Author: aqnuep
 *
 *  Note: Do not use this module in any product as it is a very rough
 *        wavefront object loader made only for the demo program and
 *        is far from product quality
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "wavefront.h"

using namespace std;

namespace demo {

WaveFrontObj::WaveFrontObj(const char* filename) {

	ifstream file(filename);
	if (!file) {
		cerr << "Unable to open file: " << filename << endl;
		return;
	}

    vector<Vertex> Vertices;
    vector<Normal> Normals;
    Vertex v;
    Normal vn;
	string token, key;
    char slash = '/';
    int id=0;

    while (file) {
    	file >> token;
    	if (token == "v") {
    		file >> v.x >> v.y >> v.z;
    		Vertices.push_back(v);
    	} else
    	if (token == "vn") {
    		file >> vn.x >> vn.y >> vn.z;
    		Normals.push_back(vn);
    	} else
    	if (token == "f") {
    		for (int i=0; i<3; i++) {
    			int iv, ivn;
    			file >> iv >> slash >> slash >> ivn;

    			IndexGroup ig;
    			ig.v = iv; ig.n = ivn; ig.i = id;

    			if (indexGroup.find(ig) == indexGroup.end()) {
    				indexGroup.insert(ig);
					vertex.push_back(Vertices[iv-1]);
					normal.push_back(Normals[ivn-1]);
					index.push_back(id++);
    			} else {
        			set<IndexGroup, IndexGroupLess>::iterator igm = indexGroup.find(ig);
    				index.push_back((*igm).i);
    			}
    		}
    	}
    }

    if (!file.eof()) {
    	cerr << "Error reading the file: " << filename << endl;
    	return;
    }

}

void WaveFrontObj::dump() {
	for (vector<Vertex>::iterator it=vertex.begin(); it!=vertex.end(); it++) {
		cout << "v " << it->x << " " << it->y << " " << it->z << endl;
	}

	for (vector<Normal>::iterator it=normal.begin(); it!=normal.end(); it++) {
		cout << "vn " << it->x << " " << it->y << " " << it->z << endl;
	}

	for (vector<Index>::iterator it=index.begin(); it!=index.end(); it++) {
		cout << "f " << (*it);
		it++; cout << " " << (*it);
		it++; cout << " " << (*it) << endl;
	}
}

} /* namespace demo */
