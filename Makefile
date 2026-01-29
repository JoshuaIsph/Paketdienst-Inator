
TARGET = src/Paketdienst-Inator.nxc
INCLUDE_PATH = src
OUTPUT = Paketdienst-Inator
OPTIMIZATION_LEVEL = 2 # Level 1 - 2   |   With newest compiler: 1 - 5


# ----- Path for OS -----
ifneq ($(OS),Windows_NT)
	UNAME_S = $(shell uname -s)

	ifeq ($(UNAME_S), Darwin)
		NBC_PLATFORM_FOLDER = Mac
	else
		NBC_PLATFORM_FOLDER = Linux
	endif

endif


HOME_DIR = $(or $(HOME), $(USERPROFILE))


ifeq ($(OS),Windows_NT)
	NBC = $(HOME_DIR)\.vscode\extensions\undefined_publisher.nxt-compiler-0.0.1\files\Windows\nbc.exe
else
	NBC = $(HOME_DIR)/.vscode/extensions/undefined_publisher.nxt-compiler-0.0.1/files/$(NBC_PLATFORM_FOLDER)/nbc
endif



# ----- Commands -----
# Enter "make" to compile
# Enter "make download" to compile and download to controller
# Enter "make clean" to remove compiler cache files and compiled program

all: compile clean


compile:
	$(NBC) $(TARGET) -I=$(INCLUDE_PATH) -O=$(OUTPUT) -Z$(OPTIMIZATION_LEVEL)
	@echo Compilation successful


download:
	$(NBC) $(TARGET) -I=$(INCLUDE_PATH) -d -O=$(OUTPUT) -Z$(OPTIMIZATION_LEVEL)
	@echo Download completed


clean:
ifeq ($(OS),Windows_NT)
	del src\*.bak 2>nul || true
	del src\*.sym 2>nul || true
	del $(OUTPUT) 2>nul || true
else
	rm -f src/*.bak
	rm -f src/*.sym
	rm -f $(OUTPUT)
endif
	@echo Cleaning completed



# ----- Testing -----

os:
ifeq ($(OS), Windows_NT)
	echo "WINDOWS!!!"
else
	echo "NOT WINDOWS!!!"
endif
	
test:
	echo $(HOME_DIR)
