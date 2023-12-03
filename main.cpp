#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define TILESET_W 


struct Point{
	int x = -1;
	int y = -1;
};

typedef std::unordered_map<int, Point> pointmap;

void printPoint(Point p){
	std::cout << "[" << p.x << ", " << p.y << "]";
}

uint32_t getPixel(SDL_Surface* surf, int x, int y){
	int bpp = surf->format->BytesPerPixel;
	uint8_t *pixel = (uint8_t*)surf->pixels + y*surf->pitch + x * bpp;

	switch(bpp){
		case 1: 
			return *pixel;
			break;
		case 2:
			return *(uint16_t*)pixel;
			break;
		case 3:
		case 4:
			return *(uint32_t*)pixel;
			break;
	}
	return 0;
}

template <size_t map_w, size_t map_h>
int saveMap(const char* filename, int (&map)[map_w][map_h]){
	std::ofstream file;
	file.open(filename);

	if(!file.is_open()){
		std::cout << "Could not open file " << filename << '.' << std::endl;
		return -1;
	}

	for(int i = 0; i < map_h; i++){
		for(int j = 0; j < map_w; j++){
			file << map[j][i] << " ";
		}
		file << "\n";
	}
	
	file.close();
	return 0;
}

template <size_t map_w, size_t map_h>
int loadMap(const char* filename, int (&map)[map_w][map_h]){
	std::ifstream file;
	file.open(filename);

	if(!file.is_open()){
		std::cout << "Could not open file " << filename << '.' << std::endl;
		return -1;
	}

	std::string line;
	
	for(int i = 0; i < map_h && getline(file, line); i++){
		std::string delimiter = " ";
		size_t pos;

		for(int j = 0; j < map_w &&((pos = line.find(delimiter)) != std::string::npos); j++){
			map[j][i] = std::stoi(line.substr(0, pos));
			line.erase(0, pos + delimiter.length());
		}
	}

	file.close();
	return 0;
}
/*
template <size_t map_w, size_t map_h>
void autotile(const char* autotile_file, Point (&map)[map_w][map_h]) {
	SDL_Surface* at_surf = SDL_LoadBMP(autotile_file);
	SDL_Surface* at_mask = SDL_CreateRGBSurface(0, 3, 3, 32, 0, 0, 0, 0);

	pointmap at_map;

	for(int i = 0; i < at_surf->h; i += 3){
		for(int j = 0; j < at_surf->w; j += 3){
			SDL_Rect src_rect = {j, i, 3, 3};							// Select the mask tile
			SDL_SetClipRect(at_surf, &src_rect);						//
			SDL_Rect dst_rect = {0, 0, 3, 3};							// Blit the mask onto
			SDL_BlitSurface(at_surf, &src_rect, at_mask, &dst_rect);	// the mask surface
			
			int bitmask = 0;
			int bit_weight = 0b100000000;
			for(int bi = 0; bi < 3; bi++){
				for(int bj = 0; bj < 3; bj++){
					uint32_t pixel = getPixel(at_mask, bj, bi);
					if(pixel == 0xff000000) bitmask |= bit_weight;
					bit_weight >>= 1;
				}
			}

			if(bitmask > 0){
				pointmap::const_iterator hit = at_map.find(bitmask);
				if(hit == at_map.end()){
					Point p;
					p.x = j / 3;
					p.y = i / 3;
					std::pair<int, Point> value(bitmask, p);
					at_map.insert(value);
				}
			}
		}
	}

	for(auto iter = at_map.begin(); iter != at_map.end(); ++iter){
		printPoint(iter->second);
		std::bitset<9> b9(iter->first);
		std::cout << " => " << b9 << std::endl;
	}

	for(int i = 0; i < map_h; i++){
		for(int j = 0; j < map_w; j++){
			int bitmask = 0;
			int bit_weight = 0b100000000;
			for(int bi = -1; bi <= 1; bi++){
				for(int bj = -1; bj <= 1; bj++){
					int bit_y = i + bi;
					int bit_x = j + bj;

					if(bit_x < 0 || bit_x == map_h || bit_y < 0 || bit_y == map_w) {}
					else if(map[bit_x][bit_y].x != -1)
						bitmask |= bit_weight;

					bit_weight >>= 1;
				}
			}

			pointmap::const_iterator hit = at_map.find(bitmask);
			if(hit != at_map.end()){
				Point tile = hit->second;
				map[j][i].x = tile.x;
				map[j][i].y = tile.y;
			}
			std::bitset<9> b9(bitmask);
			std::cout << b9 << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
*/

template <size_t w, size_t h>
void autotile2x2(const char* tilemask_file, int (&grid)[w][h], Point (&tilegrid)[w-1][h-1]){
	SDL_Surface* tilemask = SDL_LoadBMP(tilemask_file);
	SDL_Surface* mask = SDL_CreateRGBSurface(0, 2, 2, 32, 0, 0, 0, 0);

	pointmap at_map;
	for(int i = 0; i < tilemask->h; i += 2){
		for(int j = 0; j < tilemask->w; j += 2){
			SDL_Rect src_rect = {j, i, 2, 2};
			SDL_BlitSurface(tilemask, &src_rect, mask, NULL);

			uint8_t bitmask = 0b0000;
			uint8_t bit_weight = 0b1000;
			for(int bi = 0; bi < 2; bi++){
				for(int bj = 0; bj < 2; bj++){
					uint32_t pixel = getPixel(mask, bj, bi);
					if(pixel != 0xff000000) bitmask |= bit_weight;
					bit_weight >>= 1;
				}
			}
			
			auto hit = at_map.find(bitmask);
			if(hit == at_map.end()) {
				Point p;
				p.x = j/2;
				p.y = i/2;
				at_map.insert(std::pair<int, Point>(bitmask, p));
			}
		}
	}

	for(int i = 0; i < h-1; i++){
		for(int j = 0; j < w-1; j++){
			uint8_t bitmask = 0b0000;
			bitmask |= grid[j][i]		<< 3;
			bitmask |= grid[j+1][i]		<< 2;
			bitmask |= grid[j][i+1]		<< 1;
			bitmask |= grid[j+1][i+1];

			auto hit = at_map.find(bitmask);
			if(hit != at_map.end()){
				Point tile = hit->second;
				tilegrid[j][i].x = tile.x;
				tilegrid[j][i].y = tile.y;
			}
		}
	}
}

template <size_t map_w, size_t map_h>
void printMap(Point (&map)[map_w][map_h]){
	for(int i = 0; i < map_h; i++){
		for(int j = 0; j < map_w; j++){
			if(map[j][i].x != -1){
				printPoint(map[j][i]);
			}else std::cout << "- ";
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[]){
	SDL_Window* window;
	SDL_Renderer* renderer;

	window = SDL_CreateWindow(
			"SDL Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN
	);

	if(window == NULL)
		std::cout << SDL_GetError() << std::endl;

	renderer = SDL_CreateRenderer(
			window,
			-1, 
			SDL_RENDERER_ACCELERATED
	);

	if(renderer == NULL)
		std::cout << SDL_GetError() << std::endl;

	const char* tileset_file = "Brick2x2.bmp";
	const char* autotile_file = "Grass2x2-mask.bmp";
	const char* mapname = "map";

	SDL_Surface* tileset = SDL_LoadBMP(tileset_file);
	const int w = tileset->w;
	const int h = tileset->h;
	const int tile_size = 16;
	const int tileset_tiles_x = tileset->w / tile_size;
	const int tileset_tiles_y = tileset->h / tile_size;

	const int scale = 2;

	SDL_Rect srcrect = {0, 0, w, h};
	SDL_Rect dstrect = {0, 0, scale*w, scale*h};

	SDL_Surface* tileset_scaled = SDL_CreateRGBSurface(0, scale*w, scale*h, 32, 0, 0, 0, 0);
	SDL_SetSurfaceBlendMode(tileset_scaled, SDL_BLENDMODE_BLEND);
	SDL_BlitScaled(tileset, &srcrect, tileset_scaled, &dstrect);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, tileset_scaled);

	Point** tilemap = (Point**) malloc(tileset_tiles_y * sizeof(Point));
	for(int i = 0; i < tileset_tiles_y; i++){
		tilemap[i] = (Point*) malloc(tileset_tiles_x * sizeof(Point));
		for(int j = 0; j < tileset_tiles_x; j++){
			tilemap[i][j].x = j;
			tilemap[i][j].y = i;
		}
	}

	int mx, my;
	int mx_tile, my_tile;
	
	const int window_w = WINDOW_WIDTH / (scale * tile_size) + 1;
	const int window_h = WINDOW_HEIGHT / (scale * tile_size) + 1;
	const int grid_w = 2 * window_w;
	const int grid_h = 2 * window_h;
	const int grid_size = grid_w * grid_h;
	const int tilegrid_size = (grid_w-1) * (grid_h-1);

	int grid[grid_w][grid_h];
	Point tilegrid[grid_w-1][grid_h-1];

	for(int i = 0; i < grid_h; i++)
		for(int j = 0; j < grid_w; j++)
			grid[j][i] = 0;
	autotile2x2(autotile_file, grid, tilegrid);

	bool lclick = false;
	bool rclick = false;

	int grid_offset_x = 0;
	int grid_offset_y = 0;

	for(int i = 0; i < grid_h; i++){
		for(int j = 0; j < grid_w; j++)
			std::cout << grid[j][i] << " ";
		std::cout << std::endl;
	}
	bool running = true;
	while(running){
		SDL_Event event;
		while(SDL_PollEvent(&event)){
			int mx_prev = mx_tile;
			int my_prev = my_tile;

			switch(event.type){
				case SDL_QUIT:
					running = false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.scancode){
						case SDL_SCANCODE_K:
							std::cout << "Saving map: " << mapname << std::endl;
							saveMap(mapname, grid);
							break;
						case SDL_SCANCODE_L:
							std::cout << "Loading map: " << mapname << std::endl;
							loadMap(mapname, grid);
							break;
						case SDL_SCANCODE_N:
							std::cout << "Created new blank map" << std::endl;
							for(int i = 0; i < grid_h; i++)
								for(int j = 0; j < grid_w; j++)
									grid[j][i] = 0;
							break;
						case SDL_SCANCODE_W:
							grid_offset_y -= (grid_offset_y > 0)? 1 : 0;
							break;
						case SDL_SCANCODE_A:
							grid_offset_x -= (grid_offset_x > 0)? 1 : 0;
							break;
						case SDL_SCANCODE_S:
							grid_offset_y += (grid_offset_y < grid_h - window_h)? 1 : 0;
							break;
						case SDL_SCANCODE_D:
							grid_offset_x += (grid_offset_x < grid_w - window_w)? 1 : 0;
							break;
					}
					autotile2x2(autotile_file, grid, tilegrid);
					break;
				case SDL_MOUSEMOTION:
					SDL_GetMouseState(&mx, &my);
					mx_tile = (mx + float(scale/2) * tile_size) / (scale * tile_size);
					my_tile = (my + float(scale/2) * tile_size) / (scale * tile_size);
					if(mx_tile != mx_prev || my_tile != my_prev){
						if(lclick){
							grid[mx_tile + grid_offset_x][my_tile + grid_offset_y] = 1;
							autotile2x2(autotile_file, grid, tilegrid);
						}else if(rclick){
							grid[mx_tile + grid_offset_x][my_tile + grid_offset_y] = 0;
							autotile2x2(autotile_file, grid, tilegrid);
						}
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					switch(event.button.button){
						case SDL_BUTTON_LEFT:
							grid[mx_tile + grid_offset_x][my_tile + grid_offset_y] = 1;
							autotile2x2(autotile_file, grid, tilegrid);
							lclick = true;
							break;
						case SDL_BUTTON_RIGHT:
							grid[mx_tile + grid_offset_x][my_tile + grid_offset_y] = 0;
							autotile2x2(autotile_file, grid, tilegrid);
							rclick = true;
							break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					switch(event.button.button){
						case SDL_BUTTON_LEFT:
							lclick = false;
							break;
						case SDL_BUTTON_RIGHT:
							rclick = false;
							break;
					}
					break;
			}
		}

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);

		for(int i = 0; i < window_h-1; i++){
			for(int j = 0; j < window_w-1; j++){
				Point tile = tilegrid[j + grid_offset_x][i + grid_offset_y];
				if(tile.x != -1){
					SDL_Rect tile_rect = {
						scale * tile.x * tile_size,
						scale * tile.y * tile_size,
						scale * tile_size,
						scale * tile_size
					};
					SDL_Rect rend_rect = {
						scale * j * tile_size,
						scale * i * tile_size,
						scale * tile_size,
						scale * tile_size
					};

					SDL_RenderCopy(renderer, texture, &tile_rect, &rend_rect);
				}
			}
		}


		const int mx_origin = static_cast<int>((float(mx_tile) - 1.0/2) * scale * tile_size);
		const int my_origin = static_cast<int>((float(my_tile) - 1.0/2) * scale * tile_size);
		SDL_Rect mouse_rect = {
			mx_origin,
			my_origin,
			scale * tile_size,
			scale * tile_size
		};


		SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
		SDL_RenderDrawRect(renderer, &mouse_rect);
		SDL_RenderPresent(renderer);
	}

	SDL_FreeSurface(tileset);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	return 0;
}

