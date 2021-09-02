#include "kernel_L1.h"
template <>
Oriented_side
inline CGAL::Triangulation_2<MYK_L1, Tds_L1>::side_of_oriented_circle(const Point& p0, const Point& p1, const Point& p2,
	const Point& p, bool perturb) const
{
	std::cout << "using Euclidean Distance" << std::endl;
	//CGAL_triangulation_precondition( orientation(p0, p1, p2) == POSITIVE );
// no reason for such precondition and it invalidates fast removal in Delaunay

	typename MYK_L1::Side_of_oriented_circle_2 pred = geom_traits().side_of_oriented_circle_2_object();
	Oriented_side os = pred(construct_point(p0), construct_point(p1),
		construct_point(p2), construct_point(p));

	if ((os != ON_ORIENTED_BOUNDARY) || (!perturb))
		return os;

	// We are now in a degenerate case => we do a symbolic perturbation.

	// We sort the points lexicographically.
	const Point* points[4] = { &p0, &p1, &p2, &p };
	std::sort(points, points + 4, Perturbation_order(this));

	// We successively look whether the leading monomial, then 2nd monomial
	// of the determinant has non null coefficient.
	// 2 iterations are enough if p0p1p2 is positive (cf paper)
	for (int i = 3; i > 0; --i) {
		if (points[i] == &p)
			return ON_NEGATIVE_SIDE; // since p0 p1 p2 are non collinear
		  // and "conceptually" positively oriented
		Orientation o;
		if (points[i] == &p2 && (o = orientation(p0, p1, p)) != COLLINEAR)
			return Oriented_side(o);
		if (points[i] == &p1 && (o = orientation(p0, p, p2)) != COLLINEAR)
			return Oriented_side(o);
		if (points[i] == &p0 && (o = orientation(p, p1, p2)) != COLLINEAR)
			return Oriented_side(o);
	}
	// CGAL_triangulation_assertion(false);
	//no reason for such precondition and it invalidates fast removal in Delaunay
	return ON_NEGATIVE_SIDE;
}