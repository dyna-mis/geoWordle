// L1 kernel
#pragma once
#include "colored_point_2.h"
struct MyKernel :public K
{
};

typedef CGAL::Filtered_kernel_adaptor<MyKernel>  MYK_L1;
typedef CGAL::Triangulation_vertex_base_with_info_2<unsigned, MYK_L1>   Vb_L1;
typedef CGAL::Triangulation_data_structure_2<Vb_L1>                    Tds_L1;
typedef CGAL::Delaunay_triangulation_2<MYK_L1, Tds_L1>                      Delaunay_triangulation_2_L1;
typedef CGAL::Sign Oriented_side;

template <>
Oriented_side
inline CGAL::Triangulation_2<MYK_L1, Tds_L1>::side_of_oriented_circle(const Point& p0, const Point& p1, const Point& p2,
	const Point& p, bool perturb) const;

