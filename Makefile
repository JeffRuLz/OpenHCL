#.PHONY:

all: cia psp vita wii win32

clean: clean-3ds clean-psp clean-vita clean-wii clean-win32

#---------------------------------
3ds:
	@$(MAKE) -C platform/3ds
	
cia:
	@$(MAKE) -C platform/3ds cia
	
clean-3ds:
	@$(MAKE) -C platform/3ds clean
	
#---------------------------------
psp:
	@$(MAKE) -C platform/psp
	
clean-psp:
	@$(MAKE) -C platform/psp clean

#---------------------------------
vita:
	@$(MAKE) -C platform/vita
	
clean-vita:
	@$(MAKE) -C platform/vita clean
	
#---------------------------------
wii:
	@$(MAKE) -C platform/wii
	
clean-wii:
	@$(MAKE) -C platform/wii clean
	
#---------------------------------
win32:
	@$(MAKE) -C platform/win32
	
clean-win32:
	@$(MAKE) -C platform/win32 clean