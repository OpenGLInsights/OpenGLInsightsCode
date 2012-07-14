#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <NvTriStrip.h>

#include "config.hpp"
#include "vboindexer.hpp"

// Returns true iif v1 can be considered equal to v2
bool is_near(float v1, float v2, float margin){
	return fabs( v1-v2 ) < margin;
}


bool getSimilarVertexIndex( 
	PackedVertex & packed, 
	std::map<PackedVertex,indextype> & VertexToOutIndex,
	indextype & result
){
	std::map<PackedVertex,indextype>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() ){
		return false;
	}else{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<indextype> & out_indices,
	std::vector<glm::vec3> & out_vertices,
	std::vector<glm::vec2> & out_uvs,
	std::vector<glm::vec3> & out_normals
){
	// Reserve some space in the vectors to avoid mallocs
	out_indices.reserve(in_vertices.size());
	out_vertices.reserve(in_vertices.size());
	out_uvs.reserve(in_vertices.size());
	out_normals.reserve(in_vertices.size());

	std::map<PackedVertex,indextype> VertexToOutIndex;

	// For each input vertex
	for ( unsigned int i=0; i<in_vertices.size(); i++ ){

		PackedVertex packed(in_vertices[i], in_uvs[i], in_normals[i]);
		

		// Try to find a similar vertex in out_XXXX
		indextype index;
		bool found = getSimilarVertexIndex( packed, VertexToOutIndex, index);

		if ( found ){ // A similar vertex is already in the VBO, use it instead !
			out_indices.push_back( index );
			// Optional : mix the new and the former normals, tangents & bitangents
			out_normals[index] += in_normals[i];
		}else{ // If not, it needs to be added in the output data.
			out_vertices.push_back( in_vertices[i]);
			out_uvs     .push_back( in_uvs[i]);
			out_normals .push_back( in_normals[i]);
			indextype newindex = (indextype)out_vertices.size() - 1;
			out_indices .push_back( newindex );
			VertexToOutIndex[ packed ] = newindex;
		}
	}

	// Re-normalize the normals, tangents & bitangents 
	for ( unsigned int i=0; i<out_normals.size(); i++ ){
		out_normals[i] = glm::normalize(out_normals[i]);
	}

	// Optional : resize the vectors to just the right size
	std::vector<indextype>(out_indices).swap(out_indices);
	std::vector<glm::vec3>(out_vertices).swap(out_vertices);
	std::vector<glm::vec2>(out_uvs).swap(out_uvs);
	std::vector<glm::vec3>(out_normals).swap(out_normals);

}

bool Stripify(
	std::vector<unsigned short> & in_indices,
	std::vector<unsigned short> & out_indices	
){

	PrimitiveGroup* primGroups = NULL;
	unsigned short numGroups = 0;

	SetStitchStrips(true);
	SetListsOnly(true);

	bool res = GenerateStrips(
		&in_indices[0],
		in_indices.size(),
		&primGroups,
		&numGroups
	);


	for ( unsigned int i = 0 ; i < primGroups[0].numIndices ; ++i ){
		out_indices.push_back( primGroups[0].indices[i] );
	}

	delete [] primGroups;

	return res;

}