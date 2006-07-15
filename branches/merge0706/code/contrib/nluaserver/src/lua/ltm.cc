/*
** $Id$
** Tag methods
** See Copyright Notice in lua.h
*/


#include <string.h>

#define ltm_c

#include "lua/lua.h"

#include "lua/lobject.h"
#include "lua/lstate.h"
#include "lua/lstring.h"
#include "lua/ltable.h"
#include "lua/ltm.h"



const char *const luaT_typenames[] = {
  "nil", "boolean", "userdata", "number",
  "string", "table", "function", "userdata", "thread"
};


void luaT_init (lua_State *L) {
  static const char *const luaT_eventname[] = {  /* ORDER TM */
    "__index", "__newindex",
    "__gc", "__mode", "__eq",
    "__add", "__sub", "__mul", "__div",
    "__pow", "__unm", "__lt", "__le",
    "__concat", "__call"
  };
  int i;
  for (i=0; i<TM_N; i++) {
    G(L)->tmname[i] = luaS_new(L, luaT_eventname[i]);
    luaS_fix(G(L)->tmname[i]);  /* never collect these names */
  }
}


/*
** function to be used with macro "fasttm": optimized for absence of
** tag methods
*/
const TObject *luaT_gettm (Table *events, TMS event, TString *ename) {
  const TObject *tm = luaH_getstr(events, ename);
  lua_assert(event <= TM_EQ);
  if (ttisnil(tm)) {  /* no tag method? */
    events->flags |= cast(lu_byte, 1u<<event);  /* cache this fact */
    return NULL;
  }
  else return tm;
}


const TObject *luaT_gettmbyobj (lua_State *L, const TObject *o, TMS event) {
  TString *ename = G(L)->tmname[event];
  switch (ttype(o)) {
    case LUA_TTABLE:
      return luaH_getstr(hvalue(o)->metatable, ename);
    case LUA_TUSERDATA:
      return luaH_getstr(uvalue(o)->uv.metatable, ename);
    default:
      return &luaO_nilobject;
  }
}

