#include "adventure.h"

POINT *walkspots[255];
POINT *waypoints[MAX_WAYPOINTS];
unsigned int waypoint_connections[MAX_WAYPOINTS] = {0};
int enabled_paths[256] = {0};

int waypoint_count = 0;

void lua_vector(lua_State *L, int x, int y) {
    lua_getglobal(L, "vec");
    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_call(L, 2, 1);
}

void extract_vector(lua_State *L, int pos, int *x, int *y) {
    lua_pushvalue(L, pos);
    
    lua_pushstring(L, "x");
    lua_gettable(L, -2);
    *x = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    
    lua_pushstring(L, "y");
    lua_gettable(L, -2);
    *y = (int)lua_tonumber(L, -1);
    lua_pop(L, 2);
}

void connect_waypoints(int a, int b) {
    waypoint_connections[a] |= 1 << b;
    waypoint_connections[b] |= 1 << a;
}

int is_pathable(int x1, int y1, int x2, int y2) {
    float x = x1,
          y = y1;
    float dx = x2 - x1, 
          dy = y2 - y1;
    float dist = sqrt(dx * dx + dy * dy);
    int i;
    
    dx /= dist;
    dy /= dist;

    for (i = 0; i < (int)dist; i++) {
        x += dx;
        y += dy;
        
        if (!is_walkable((int)x, (int)y))
            return 0;
    }
    
    return 1;
}

void build_walkspots() {
    int i, color, x, y;
    
    for (i = 0; i < 255; i++)
        free(walkspots[i]);
    
    waypoint_count = 0;
    
    color = 0;
    for (y = 0; y < SCREEN_HEIGHT; y++)
    for (x = 0; x < SCREEN_WIDTH; x++)
        if ((color = getpixel(room_hot, x, y)) && color < 255) {
            walkspots[color] = malloc(sizeof(POINT));
            walkspots[color]->x = x;
            walkspots[color]->y = y;
        }
}

void build_waypoints() {
    int i, x, y, a, b;

    for (i = 0; i < waypoint_count; i++) {
        free(waypoints[i]);
        waypoint_connections[i] = 0;
    }
    
    waypoint_count = 0;
    
    for (y = 0; y < SCREEN_HEIGHT; y++)
    for (x = 0; x < SCREEN_WIDTH; x++)
        if (getpixel(room_hot, x, y) == 255) {
            waypoints[waypoint_count] = malloc(sizeof(POINT));
            waypoints[waypoint_count]->x = x;
            waypoints[waypoint_count++]->y = y;
        }
    
    for (a = 0; a < waypoint_count; a++)
    for (b = a + 1; b < waypoint_count; b++) {
        POINT pa = *waypoints[a], pb = *waypoints[b];
        if (is_pathable(pa.x, pa.y, pb.x, pb.y))
            connect_waypoints(a, b);
    }
}

int script_get_neighbors(lua_State *L) {
    int i, j = 1, connection;
    
    if (lua_gettop(L) != 1 || !lua_isnumber(L, 1)) {
        lua_pushstring(L, "get_neighbors expects (int)");
        lua_error(L);
    }
    
    connection = waypoint_connections[(int)lua_tonumber(L, 1)];
    lua_newtable(L);
    
    for (i = 0; i < MAX_WAYPOINTS; i++)
        if (connection & (1 << i)) {
            lua_pushnumber(L, j++);
            lua_pushnumber(L, i);
            lua_settable(L, -3);
        }
        
    return 1;
}

int script_get_waypoint(lua_State *L) {
    POINT defaultspot, *waypoint;
    
    if (lua_gettop(L) != 1 || !lua_isnumber(L, 1)) {
        lua_pushstring(L, "get_waypoint expects (int)");
        lua_error(L);
    }
    
    defaultspot.x = 0;
    defaultspot.y = 0;
    
    waypoint = waypoints[(int)lua_tonumber(L, 1)];
    if (!waypoint)
        waypoint = &defaultspot;
    
    lua_vector(L, waypoint->x, waypoint->y);
        
    return 1;
}

int script_get_walkspot(lua_State *L) {
    POINT defaultspot, *walkspot;
    
    if (lua_gettop(L) != 1 || !lua_isnumber(L, 1)) {
        lua_pushstring(L, "get_walkspot expects (int)");
        lua_error(L);
    }

    
    defaultspot.x = 0;
    defaultspot.y = 0;
    
    walkspot = walkspots[(int)lua_tonumber(L, 1)];
    if (!walkspot)
        walkspot = &defaultspot;
    
    lua_vector(L, walkspot->x, walkspot->y);
    
    return 1;
}

int script_get_closest_waypoint(lua_State *L) {
    int x, y;
    
    if (lua_gettop(L) != 1 || !lua_istable(L, 1)) {
        lua_pushstring(L, "get_closest_waypoint expects (table)");
        lua_error(L);
    }
    
    extract_vector(L, -1, &x, &y);
    lua_pushnumber(L, closest_waypoint(x, y));
    
    return 1;
}

int script_enable_path(lua_State *L) {
    int n, b;
    
    if ((lua_gettop(L) == 1 && !lua_isnumber(L, 1)) ||
        (lua_gettop(L) >= 2 && (!lua_isnumber(L, 1) || !lua_isboolean(L, 2)))) {
        lua_pushstring(L, "enable_path expects (int, [bool])");
        lua_error(L);
    }
    
    n = lua_tonumber(L, 1);
    b = lua_gettop(L) == 2 ? lua_toboolean(L, 2) : 1;
    
    enabled_paths[n] = b;
    build_waypoints();
    
    return 0;
}

int is_walkable(int x, int y) {    
    return enabled_paths[(getpixel(room_hot, x, y) & (255 << 8)) >> 8] || getpixel(room_hot, x, y) == 255;
}

int script_is_walkable(lua_State *L) {
    BITMAP *bmp;
    int x, y, pixel;
    
    if (lua_gettop(L) != 2 || !lua_isuserdata(L, 1) || !lua_istable(L, 2)) {
        lua_pushstring(L, "is_walkable expects (bitmap, vector)");
        lua_error(L);
    }
    
    bmp = *(BITMAP**)lua_touserdata(L, 1);
    
    extract_vector(L, -1, &x, &y);
    pixel = getpixel(bmp, x, y);
    
    if (pixel == 255)
        lua_pushnumber(L, 255);
    else
        lua_pushnumber(L, (pixel & (255 << 8)) >> 8);
    
    return 1;
}

int script_is_pathable(lua_State *L) {
    int x1, x2, y1, y2;
    
    if (lua_gettop(L) != 2 || !lua_istable(L, 1) || !lua_istable(L, 2)) {
        lua_pushstring(L, "is_pathable expects (vector, vector)");
        lua_error(L);
    }
    
    extract_vector(L, 1, &x1, &y1);
    extract_vector(L, 2, &x2, &y2);
    
    lua_pushboolean(L, is_pathable(x1, y1, x2, y2));
    
    return 1;
}

int closest_waypoint(int x, int y) {
    int dist = 9999999, winner = 0;
    int i, tx, ty, dif;
    
    for (i = 0; i < waypoint_count; i++) {
        tx = waypoints[i]->x - x;
        ty = waypoints[i]->y - y;
        dif = sqrt(tx * tx + ty * ty);
        if (dif < dist && is_pathable(x, y, waypoints[i]->x, waypoints[i]->y)) {
            dist = dif;
            winner = i;
        }
    }
    
    return winner;
}

void register_path() {
    lua_register(script, "get_neighbors", &script_get_neighbors);
    lua_register(script, "get_waypoint", &script_get_waypoint);
    lua_register(script, "get_walkspot", &script_get_walkspot);
    lua_register(script, "get_closest_waypoint", &script_get_closest_waypoint);
    lua_register(script, "enable_path", &script_enable_path);
    lua_register(script, "is_walkable", &script_is_walkable);
    lua_register(script, "is_pathable", &script_is_pathable);
}
