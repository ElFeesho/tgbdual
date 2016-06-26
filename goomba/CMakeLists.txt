project(goomba C CXX)
set(GOOMBA_SOURCES goombarom.c goombarom.h goombasav.c goombasav.h lzoconf.h lzodefs.h minilzo.c minilzo.h)

add_library(goomba ${GOOMBA_SOURCES})
