
#include <iostream>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/geometry/geometries/adapted/boost_tuple.hpp>

BOOST_GEOMETRY_REGISTER_C_ARRAY_CS(cs::cartesian)
BOOST_GEOMETRY_REGISTER_BOOST_TUPLE_CS(cs::cartesian)

using namespace boost::geometry;

int test_main()
{
	model::d2::point_xy<int> p1(1, 1), p2(2, 2);
	std::cout << "Distance p1-p2 is: " << distance(p1, p2) << std::endl;

	int a[2] = { 1,1 };
	int b[2] = { 2,2 };
	double d = distance(a, b);
	std::cout << "Distance a-b is: " << d << std::endl;

	double points[][2] = { { 2.0, 1.3 },{ 4.1, 3.0 },{ 5.3, 2.6 },{ 2.9, 0.7 },{ 2.0, 1.3 } };
	model::polygon<model::d2::point_xy<double> > poly;
	append(poly, points);
	boost::tuple<double, double> p = boost::make_tuple(3.7, 2.0);

	//在多边形内
	std::cout << "Point p is in polygon? " << std::boolalpha << within(p, poly) << std::endl;

	//求多边形面积
	std::cout << "Area: " << area(poly) << std::endl;

	double d2 = distance(a, p);
	std::cout << "Distance a-p is: " << d2 << std::endl;


	//We approximate the Earth as a sphere and calculate the distance between Amsterdam and Paris:
	typedef boost::geometry::model::point
		<
		double, 2, boost::geometry::cs::spherical_equatorial<boost::geometry::degree>
		> spherical_point;
	spherical_point amsterdam(4.90, 52.37);
	spherical_point paris(2.35, 48.86);
	double const earth_radius = 3959; // miles
	std::cout << "Distance in miles: " << distance(amsterdam, paris) * earth_radius << std::endl;



	return 0;
}