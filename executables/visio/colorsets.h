#ifndef COLORSETS_H
#define COLORSETS_H	

typedef struct ColorSet
{
	int h_lower;
	int s_lower;
	int v_lower;

	int h_higher;
	int s_higher;
	int v_higher;
}ColorSet;

typedef struct ColorSets
{
	ColorSet blue;
	ColorSet red;
	ColorSet white;
}ColorSets;

#endif

