#ifndef PACKEDVERTEX_H
#define PACKEDVERTEX_H

#pragma pack(push, 1)
struct PackedVertex{
	PackedVertex(glm::vec3 & p_position, glm::vec2 & p_uv, glm::vec3 & p_normal)
	{
		//memset((void*)this, 0, sizeof(PackedVertex));
		position=p_position;
		uv=p_uv;
		normal=p_normal;
	}
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
};
//inline bool operator<(const PackedVertex & a,const PackedVertex & b){
//	return memcmp((void*)&a, (void*)&b, sizeof(PackedVertex))>0;
//};

bool is_near(float v1, float v2, float margin);

inline bool operator<(const PackedVertex & a,const PackedVertex & b){
	if(!is_near( a.position.x , b.position.x, 0.001f )) return a.position.x < b.position.x;
	if(!is_near( a.position.y , b.position.y, 0.001f )) return a.position.y < b.position.y;
	if(!is_near( a.position.z , b.position.z, 0.001f )) return a.position.z < b.position.z;
	if(!is_near( a.uv.x       , b.uv.x      , 0.1f   )) return a.uv.x       < b.uv.x      ;
	if(!is_near( a.uv.y       , b.uv.y      , 0.1f   )) return a.uv.y       < b.uv.y      ;
	if(!is_near( a.normal.x   , b.normal.x  , 0.3f   )) return a.normal.x   < b.normal.x  ;
	if(!is_near( a.normal.y   , b.normal.y  , 0.3f   )) return a.normal.y   < b.normal.y  ;
	if(!is_near( a.normal.z   , b.normal.z  , 0.3f   )) return a.normal.z   < b.normal.z  ;
	return false; // Vertices are equal
};


#pragma pack(pop)


#endif