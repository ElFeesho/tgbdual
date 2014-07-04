all: libretro sdl

core:
	$(MAKE) -C goomba
	$(MAKE) -C gb_core

libretro: core
	$(MAKE) -C libretro_ui

sdl: core
	$(MAKE) -C sdl_ui

clean:
	$(MAKE) -C goomba clean
	$(MAKE) -C gb_core clean
	$(MAKE) -C libretro_ui clean
	$(MAKE) -C sdl_ui clean

