#.PHONY:

all: cia psp wii win32

clean: clean-3ds clean-psp clean-wii clean-win32

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
wii:
	@$(MAKE) -C platform/wii
	
clean-wii:
	@$(MAKE) -C platform/wii clean
	
#---------------------------------
win32:
	@$(MAKE) -C platform/win32
	
clean-win32:
	@$(MAKE) -C platform/win32 clean