#ifndef __WadeWork__types_h__
#define __WadeWork__types_h__
namespace ww
{
	template <typename T>
	class vec2d
	{
	public:
		T x;
		T y;

		vec2d();

		vec2d(T x, T y);
	};
	typedef vec2d<int> vec2di;
	typedef vec2d<float> vec2df;

	template <typename T>
	class vec3d
	{
	public:
		T x;
		T y;
		T z;

		vec3d();

		vec3d(T x, T y, T z);
	};
	typedef vec3d<int> vec3di;
	typedef vec3d<float> vec3df;



	template <typename T>
	class Rectangle
	{
	public:
		T x;
		T y;
		T width;
		T height;

		Rectangle();

		Rectangle(T x, T y, T width, T height);

		bool intersects(Rectangle R);
	};
	typedef Rectangle<int> Rectanglei;
	typedef Rectangle<float> Rectanglef;
};

#include "types.inl"
#endif