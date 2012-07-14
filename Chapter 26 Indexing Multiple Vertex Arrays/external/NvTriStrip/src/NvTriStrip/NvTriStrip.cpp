
#include "NvTriStripObjects.h"
#include "NvTriStrip.h"

////////////////////////////////////////////////////////////////////////////////////////
//private data
static unsigned int cacheSize    = CACHESIZE_GEFORCE1_2;
static bool bStitchStrips         = true;
static unsigned int minStripSize = 0;
static bool bListsOnly            = false;
static unsigned int restartVal   = 0;
static bool bRestart              = false;

void EnableRestart(const unsigned int _restartVal)
{
	bRestart = true;
	restartVal = _restartVal;
}

void DisableRestart()
{
	bRestart = false;
}

////////////////////////////////////////////////////////////////////////////////////////
// SetListsOnly()
//
// If set to true, will return an optimized list, with no strips at all.
//
// Default value: false
//
void SetListsOnly(const bool _bListsOnly)
{
	bListsOnly = _bListsOnly;
}

////////////////////////////////////////////////////////////////////////////////////////
// SetCacheSize()
//
// Sets the cache size which the stripfier uses to optimize the data.
// Controls the length of the generated individual strips.
// This is the "actual" cache size, so 24 for GeForce3 and 16 for GeForce1/2
// You may want to play around with this number to tweak performance.
//
// Default value: 16
//
void SetCacheSize(const unsigned int _cacheSize)
{
	cacheSize = _cacheSize;
}


////////////////////////////////////////////////////////////////////////////////////////
// SetStitchStrips()
//
// bool to indicate whether to stitch together strips into one huge strip or not.
// If set to true, you'll get back one huge strip stitched together using degenerate
//  triangles.
// If set to false, you'll get back a large number of separate strips.
//
// Default value: true
//
void SetStitchStrips(const bool _bStitchStrips)
{
	bStitchStrips = _bStitchStrips;
}


////////////////////////////////////////////////////////////////////////////////////////
// SetMinStripSize()
//
// Sets the minimum acceptable size for a strip, in triangles.
// All strips generated which are shorter than this will be thrown into one big, separate list.
//
// Default value: 0
//
void SetMinStripSize(const unsigned int _minStripSize)
{
	minStripSize = _minStripSize;
}


////////////////////////////////////////////////////////////////////////////////////////
//Cleanup strips / faces, used by generatestrips
void Cleanup(NvStripInfoVec& tempStrips, NvFaceInfoVec& tempFaces)
{
	//delete strips
	for(int i = 0; i < tempStrips.size(); i++)
	{
		for(int j = 0; j < tempStrips[i]->m_faces.size(); j++)
		{
			delete tempStrips[i]->m_faces[j];
			tempStrips[i]->m_faces[j] = NULL;
		}
		tempStrips[i]->m_faces.resize(0);
		delete tempStrips[i];
		tempStrips[i] = NULL;
	}

	//delete faces
	for(i = 0; i < tempFaces.size(); i++)
	{
		delete tempFaces[i];
		tempFaces[i] = NULL;
	}
}


////////////////////////////////////////////////////////////////////////////////////////
//SameTriangle()
//
//Returns true if the two triangles defined by firstTri and secondTri are the same
// The "same" is defined in this case as having the same indices with the same winding order
//
bool SameTriangle(unsigned short firstTri0, unsigned short firstTri1, unsigned short firstTri2, 
				  unsigned short secondTri0, unsigned short secondTri1, unsigned short secondTri2)
{
	bool isSame = false;

	if (firstTri0 == secondTri0)
	{
		if (firstTri1 == secondTri1)
		{
			if (firstTri2 == secondTri2)
				isSame = true;
		}
	}
	else if (firstTri0 == secondTri1)
	{
		if (firstTri1 == secondTri2)
		{
			if (firstTri2 == secondTri0)
				isSame = true;
		}
	}
	else if (firstTri0 == secondTri2)
	{
		if (firstTri1 == secondTri0)
		{
			if (firstTri2 == secondTri1)
				isSame = true;
		}
	}

	return isSame;
}


bool TestTriangle(const unsigned short v0, const unsigned short v1, const unsigned short v2, const std::vector<NvFaceInfo>* in_bins, const int NUMBINS)
{
	//hash this triangle
	bool isLegit = false;
	int ctr = v0 % NUMBINS;
	for (int k = 0; k < in_bins[ctr].size(); ++k)
	{
		//check triangles in this bin
		if (SameTriangle(in_bins[ctr][k].m_v0, in_bins[ctr][k].m_v1, in_bins[ctr][k].m_v2, 
			v0, v1, v2))
		{
			isLegit = true;
			break;
		}
	}
	if (!isLegit)
	{
		ctr = v1 % NUMBINS;
		for (int k = 0; k < in_bins[ctr].size(); ++k)
		{
			//check triangles in this bin
			if (SameTriangle(in_bins[ctr][k].m_v0, in_bins[ctr][k].m_v1, in_bins[ctr][k].m_v2, 
				v0, v1, v2))
			{
				isLegit = true;
				break;
			}
		}
		
		if (!isLegit)
		{
			ctr = v2 % NUMBINS;
			for (int k = 0; k < in_bins[ctr].size(); ++k)
			{
				//check triangles in this bin
				if (SameTriangle(in_bins[ctr][k].m_v0, in_bins[ctr][k].m_v1, in_bins[ctr][k].m_v2, 
					v0, v1, v2))
				{
					isLegit = true;
					break;
				}
			}
			
		}
	}

	return isLegit;
}
	

////////////////////////////////////////////////////////////////////////////////////////
// GenerateStrips()
//
// in_indices: input index list, the indices you would use to render
// in_numIndices: number of entries in in_indices
// primGroups: array of optimized/stripified PrimitiveGroups
// numGroups: number of groups returned
//
// Be sure to call delete[] on the returned primGroups to avoid leaking mem
//
bool GenerateStrips(const unsigned short* in_indices, const unsigned int in_numIndices,
					PrimitiveGroup** primGroups, unsigned short* numGroups, bool validateEnabled)
{
	//put data in format that the stripifier likes
	WordVec tempIndices;
	tempIndices.resize(in_numIndices);
	unsigned short maxIndex = 0;
	unsigned short minIndex = 0xFFFF;
	for(int i = 0; i < in_numIndices; i++)
	{
		tempIndices[i] = in_indices[i];
		if (in_indices[i] > maxIndex)
			maxIndex = in_indices[i];
		if (in_indices[i] < minIndex)
			minIndex = in_indices[i];
	}
	NvStripInfoVec tempStrips;
	NvFaceInfoVec tempFaces;

	NvStripifier stripifier;
	
	//do actual stripification
	stripifier.Stripify(tempIndices, cacheSize, minStripSize, maxIndex, tempStrips, tempFaces);

	//stitch strips together
	IntVec stripIndices;
	unsigned int numSeparateStrips = 0;

	if(bListsOnly)
	{
		//if we're outputting only lists, we're done
		*numGroups = 1;
		(*primGroups) = new PrimitiveGroup[*numGroups];
		PrimitiveGroup* primGroupArray = *primGroups;

		//count the total number of indices
		unsigned int numIndices = 0;
		for(int i = 0; i < tempStrips.size(); i++)
		{
			numIndices += tempStrips[i]->m_faces.size() * 3;
		}

		//add in the list
		numIndices += tempFaces.size() * 3;

		primGroupArray[0].type       = PT_LIST;
		primGroupArray[0].numIndices = numIndices;
		primGroupArray[0].indices    = new unsigned short[numIndices];

		//do strips
		unsigned int indexCtr = 0;
		for(i = 0; i < tempStrips.size(); i++)
		{
			for(int j = 0; j < tempStrips[i]->m_faces.size(); j++)
			{
				//degenerates are of no use with lists
				if(!NvStripifier::IsDegenerate(tempStrips[i]->m_faces[j]))
				{
					primGroupArray[0].indices[indexCtr++] = tempStrips[i]->m_faces[j]->m_v0;
					primGroupArray[0].indices[indexCtr++] = tempStrips[i]->m_faces[j]->m_v1;
					primGroupArray[0].indices[indexCtr++] = tempStrips[i]->m_faces[j]->m_v2;
				}
				else
				{
					//we've removed a tri, reduce the number of indices
					primGroupArray[0].numIndices -= 3;
				}
			}
		}

		//do lists
		for(i = 0; i < tempFaces.size(); i++)
		{			
			primGroupArray[0].indices[indexCtr++] = tempFaces[i]->m_v0;
			primGroupArray[0].indices[indexCtr++] = tempFaces[i]->m_v1;
			primGroupArray[0].indices[indexCtr++] = tempFaces[i]->m_v2;
		}
	}
	else
	{
		stripifier.CreateStrips(tempStrips, stripIndices, bStitchStrips, numSeparateStrips, bRestart, restartVal);

		//if we're stitching strips together, we better get back only one strip from CreateStrips()
		assert( (bStitchStrips && (numSeparateStrips == 1)) || !bStitchStrips);
		
		//convert to output format
		*numGroups = numSeparateStrips; //for the strips
		if(tempFaces.size() != 0)
			(*numGroups)++;  //we've got a list as well, increment
		(*primGroups) = new PrimitiveGroup[*numGroups];
		
		PrimitiveGroup* primGroupArray = *primGroups;
		
		//first, the strips
		int startingLoc = 0;
		for(int stripCtr = 0; stripCtr < numSeparateStrips; stripCtr++)
		{
			int stripLength = 0;

			if(!bStitchStrips)
			{
				//if we've got multiple strips, we need to figure out the correct length
				for(int i = startingLoc; i < stripIndices.size(); i++)
				{
					if(stripIndices[i] == -1)
						break;
				}
				
				stripLength = i - startingLoc;
			}
			else
				stripLength = stripIndices.size();
			
			primGroupArray[stripCtr].type       = PT_STRIP;
			primGroupArray[stripCtr].indices    = new unsigned short[stripLength];
			primGroupArray[stripCtr].numIndices = stripLength;
			
			int indexCtr = 0;
			for(int i = startingLoc; i < stripLength + startingLoc; i++)
				primGroupArray[stripCtr].indices[indexCtr++] = stripIndices[i];

			//we add 1 to account for the -1 separating strips
			//this doesn't break the stitched case since we'll exit the loop
			startingLoc += stripLength + 1; 
		}
		
		//next, the list
		if(tempFaces.size() != 0)
		{
			int faceGroupLoc = (*numGroups) - 1;    //the face group is the last one
			primGroupArray[faceGroupLoc].type       = PT_LIST;
			primGroupArray[faceGroupLoc].indices    = new unsigned short[tempFaces.size() * 3];
			primGroupArray[faceGroupLoc].numIndices = tempFaces.size() * 3;
			int indexCtr = 0;
			for(int i = 0; i < tempFaces.size(); i++)
			{
				primGroupArray[faceGroupLoc].indices[indexCtr++] = tempFaces[i]->m_v0;
				primGroupArray[faceGroupLoc].indices[indexCtr++] = tempFaces[i]->m_v1;
				primGroupArray[faceGroupLoc].indices[indexCtr++] = tempFaces[i]->m_v2;
			}
		}
	}

	//validate generated data against input
	if (validateEnabled)
	{
		const int NUMBINS = 100;

		std::vector<NvFaceInfo> in_bins[NUMBINS];
		
		//hash input indices on first index
		for (int i = 0; i < in_numIndices; i += 3)
		{
			NvFaceInfo faceInfo(in_indices[i], in_indices[i + 1], in_indices[i + 2]);
			in_bins[in_indices[i] % NUMBINS].push_back(faceInfo);
		}
		
		for (i = 0; i < *numGroups; ++i)
		{
			switch ((*primGroups)[i].type)
			{
				case PT_LIST:
				{
					for (int j = 0; j < (*primGroups)[i].numIndices; j += 3)
					{
						unsigned short v0 = (*primGroups)[i].indices[j];
						unsigned short v1 = (*primGroups)[i].indices[j + 1];
						unsigned short v2 = (*primGroups)[i].indices[j + 2];
						
						//ignore degenerates
						if (NvStripifier::IsDegenerate(v0, v1, v2))
							continue;

						if (!TestTriangle(v0, v1, v2, in_bins, NUMBINS))
						{
							Cleanup(tempStrips, tempFaces);
							return false;
						}
					}
					break;
				}

				case PT_STRIP:
				{
					int brokenCtr = 0;
					bool flip = false;
					for (int j = 2; j < (*primGroups)[i].numIndices; ++j)
					{
						unsigned short v0 = (*primGroups)[i].indices[j - 2];
						unsigned short v1 = (*primGroups)[i].indices[j - 1];
						unsigned short v2 = (*primGroups)[i].indices[j];
						
						if (flip)
						{
							//swap v1 and v2
							unsigned short swap = v1;
							v1 = v2;
							v2 = swap;
						}

						//ignore degenerates
						if (NvStripifier::IsDegenerate(v0, v1, v2))
						{
							flip = !flip;
							continue;
						}

						if (!TestTriangle(v0, v1, v2, in_bins, NUMBINS))
						{
							Cleanup(tempStrips, tempFaces);
							return false;
						}

						flip = !flip;
					}
					break;
				}

				case PT_FAN:
				default:
					break;
			}
		}

	}

	//clean up everything
	Cleanup(tempStrips, tempFaces);

	return true;
}


////////////////////////////////////////////////////////////////////////////////////////
// RemapIndices()
//
// Function to remap your indices to improve spatial locality in your vertex buffer.
//
// in_primGroups: array of PrimitiveGroups you want remapped
// numGroups: number of entries in in_primGroups
// numVerts: number of vertices in your vertex buffer, also can be thought of as the range
//  of acceptable values for indices in your primitive groups.
// remappedGroups: array of remapped PrimitiveGroups
//
// Note that, according to the remapping handed back to you, you must reorder your 
//  vertex buffer.
//
void RemapIndices(const PrimitiveGroup* in_primGroups, const unsigned short numGroups,
				  const unsigned short numVerts, PrimitiveGroup** remappedGroups)
{
	(*remappedGroups) = new PrimitiveGroup[numGroups];

	//caches oldIndex --> newIndex conversion
	int *indexCache;
	indexCache = new int[numVerts];
	memset(indexCache, -1, sizeof(int)*numVerts);
	
	//loop over primitive groups
	unsigned int indexCtr = 0;
	for(int i = 0; i < numGroups; i++)
	{
		unsigned int numIndices = in_primGroups[i].numIndices;

		//init remapped group
		(*remappedGroups)[i].type       = in_primGroups[i].type;
		(*remappedGroups)[i].numIndices = numIndices;
		(*remappedGroups)[i].indices    = new unsigned short[numIndices];

		for(int j = 0; j < numIndices; j++)
		{
			int cachedIndex = indexCache[in_primGroups[i].indices[j]];
			if(cachedIndex == -1) //we haven't seen this index before
			{
				//point to "last" vertex in VB
				(*remappedGroups)[i].indices[j] = indexCtr;

				//add to index cache, increment
				indexCache[in_primGroups[i].indices[j]] = indexCtr++;
			}
			else
			{
				//we've seen this index before
				(*remappedGroups)[i].indices[j] = cachedIndex;
			}
		}
	}

	delete[] indexCache;
}