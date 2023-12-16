#ifndef _FUNCTIONAL
#include <functional>
#define _FUNCTIONAL
#endif

#ifndef _ALGORITHM
#include <algorithm>
#define _ALGORITHM
#endif

class Point{
	public:
		int x{0};
		int y{0};
	
		Point(int x, int y);
		Point(Point &p);

		bool operator==(const Point& other){
			return x == other.x && y == other.y;
		}

		void move(int x_offset, int y_offset);
		void moveTo(int x, int y);
};

struct PointHash{
	std::size_t operator()(const Point& p) const{
		std::size_t h1 = std::hash<int>()(p.x);
		std::size_t h2 = std::hash<int>()(p.y);

		return h1 ^ h2;
	}
};

