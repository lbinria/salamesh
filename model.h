#pragma once

#include <ultimaille/all.h>

using namespace UM;

struct Model {

	Model(Triangles &tri, Hexahedra &hex, CellFacetAttribute<float> &cell_highlights) : 
		tri(tri), 
		hex(hex), 
		cell_highlights(cell_highlights) {}

	Triangles &tri;
	Hexahedra &hex;
	CellFacetAttribute<float> &cell_highlights;



};