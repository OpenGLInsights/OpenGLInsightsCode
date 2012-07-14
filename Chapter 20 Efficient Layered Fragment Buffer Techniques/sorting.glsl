/* Copyright 2011 Pyarelal Knowles, under GNU LGPL (see LICENCE.txt) */


//other sorts were attempted but these
//were the most promising
#define SORT_MERGE 0
#define SORT_BUBBLE 0
#define SORT_INSERT 1
#define SORT_SELECT 0
#define SORT_SHELL 0

#if SORT_SHELL
void sortFragments()
{
	int inc = fragCount / 2;
	while (inc > 0)
	{
		for (int i = inc; i < fragCount; ++i)
		{
			vec4 tmp = frags[i];
			int j = i;
			while (j >= inc && frags[j - inc].a > tmp.a)
			{
				frags[j] = frags[j - inc];
				j -= inc;
			}
			frags[j] = tmp;
		}
		inc = int(inc / 2.2 + 0.5);
	}
}
#endif
	
#if SORT_MERGE
vec4 leftArray[MAX_FRAGS/2];
void merge(int step, int a, int b, int c)
{
	int i;
	for (i = 0; i < step; ++i)
		leftArray[i] = frags[a+i];

	i = 0;
	int j = 0;
	for (int k = a; k < c; ++k)
	{
		if (b+j >= c || (i < step && leftArray[i].w < frags[b+j].w))
			frags[k] = leftArray[i++];
		else
			frags[k] = frags[b+j++];
	}
}
void sortFragments()
{
	int n = fragCount;
	int step = 1;
	while (step <= n)
	{
		int i = 0;
		while (i < n - step)
		{
			merge(step, i, i + step, min(i + step + step, n));
			i += 2 * step;
		}
		step *= 2;
	}
}
#endif

#if SORT_BUBBLE
void sortFragments()
{
	vec4 t;
	for (int i = 0; i < fragCount-1; ++i)
	{
		for (int j = i+1; j < fragCount; ++j)
		{
			if (frags[j].a < frags[i].a)
			{
				t = frags[i];
				frags[i] = frags[j];
				frags[j] = t;
			}
		}
	}
}
#endif

#if SORT_INSERT
void sortFragments()
{
	for (int j = 1; j < fragCount; ++j)
	{
		vec4 key = frags[j];
		int i = j - 1;
		while (i >= 0 && frags[i].a > key.a)
		{
			frags[i+1] = frags[i];
			--i;
		}
		frags[i+1] = key;
	}
}
#endif

#if SORT_SELECT
void sortFragments()
{
	vec4 t;
	for (int j = 0; j < fragCount-1; ++j)
	{
		int swap = j;
		for (int i = j+1; i < fragCount; ++i)
		{
			if (frags[swap].a > frags[i].a)
				swap = i;
		}
		t = frags[swap];
		frags[swap] = frags[j];
		frags[j] = t;
	}
}
#endif

