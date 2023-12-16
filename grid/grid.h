#ifndef _SDL_H
#include <SDL2/SDL.h>
#define _SDL_H
#endif

#ifndef _POINT_H
#include "../point/point.h"
#define _POINT_H
#endif

#ifndef _ALGORITHM
#include <algorithm>
#define _ALGORITHM
#endif

#ifndef _UNORDERED_MAP
#include <unordered_map>
#define _UNORDERED_MAP
#endif

class GridLayer{
	private:
		std::unordered_map<Point, bool, PointHash> _map;
	
	public:
		bool get(int x, int y);
		void set(int x, int y);
		void unset(int x, int y);

		void render(SDL_Renderer* renderer);
};
