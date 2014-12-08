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
	inline Rectangle<T>::Rectangle() : x(0), y(0), width(0), height(0), xoffset(0), yoffset(0)
	{

	}

	template <typename T>
	inline Rectangle<T>::Rectangle(T X, T Y, T W, T H) : x(X), y(Y), width(W), height(H), xoffset(0), yoffset(0)
	{

	}

	template <typename T>
	inline Rectangle<T>::Rectangle(T X, T Y, T W, T H, T OX, T OY) : x(X), y(Y), width(W), height(H), xoffset(OX), yoffset(OY)
	{

	}

	template <typename T>
	inline bool Rectangle<T>::intersects(Rectangle<T> R)
	{
		if (x+xoffset > R.x+R.xoffset+R.width-1)
			return false;
		if (y+yoffset > R.y+R.yoffset+R.height-1)
			return false;
		if (x+xoffset+width-1 < R.x+R.xoffset)
			return false;
		if (y+yoffset+height-1 < R.y+R.yoffset)
			return false;
		return true;
	}
};