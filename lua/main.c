#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>

int run_lua_script(char *file_path);
int main(int argc, char *argv[]) {
  if (argc == 2) {
    return run_lua_script(argv[1]);
  }

  int i;
  int count = 0;
  for (i = 0; i < 1000000000; i++) {
    count += 1;
  }

  printf("Hello from C: %d\n", count);

  return 0;
}

int run_lua_script(char *file_path) {
  // Create a new Lua state
  lua_State *L = luaL_newstate();

  // Open standard libraries
  luaL_openlibs(L);

  // Load and execute the Lua script
  if (luaL_dofile(L, file_path) != 0) {
    fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1); // Remove error message from the stack
  }

  // Close the Lua state
  lua_close(L);
  return 0;
}
