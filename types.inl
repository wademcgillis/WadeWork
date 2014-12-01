#include <WadeWork/types.h>

namespace ww
{
	template <typename T>
	inline vec2d<T>::vec2d() : x(0), y(0)
	{

	}

	template <typename T>
	inline vec2d<T>::vec2d(T X, T Y) : x(X), y(Y)
	{

	}

	template <typename T>
	inline vec3d<T>::vec3d() : x(0), y(0), z(0)
	{

	}

	template <typename T>
	inline vec3d<T>::vec3d(T X, T Y, T Z) : x(X), y(Y), z(Z)
	{

	}

	template <typename T>
	inline Rectangle<T>::Rectangle() : x(0), y(0), width(0), height(0)
	{

	}

	template <typename T>
	inline Rectangle<T>::Rectangle(T X, T Y, T W, T H) : x(X), y(Y), width(W), height(H)
	{

	}

	template <typename T>
	inline bool Rectangle<T>::intersects(Rectangle<T> R)
	{
		if (y > R.y+R.height)
			return false;
		if (x > R.x+R.width)
			return false;
		if (x+width < R.x)
			return false;
		if (y+height < R.y)
			return false;
		return true;
	}
};