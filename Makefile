TARGET = miner
TYPE = debug

DIRS =	src \
        src/crypto \
	src/common \
	src/contrib/epee/include

CPPDEFS = 
CCPARAM = -s WASM=1
CXXFLAGS =  -std=c++11

ifeq ($(TYPE),debug)
CCPARAM += -g
CPPDEFS += DEBUG
endif

ifeq ($(TYPE), release)
CCPARAM += -O2
endif

LDPARAM += $(LDFLAGS) -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' --proxy-to-worker --shell-file template.html --post-js post.js

INCPATH :=  $(DIRS) /opt/local/include

LIBPATH :=  /opt/local/lib/ /usr/local/lib

EXTRA_FILES = Makefile

C++ = emcc

STORE = build/$(TYPE)
SOURCE := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.cpp))
CSOURCE := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.c))
HEADERS := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.h))
OBJECTS := $(addprefix $(STORE)/, $(SOURCE:.cpp=.bc))
COBJECTS := $(addprefix $(STORE)/, $(CSOURCE:.c=.bc))

.PHONY: clean backup dirs

$(TARGET): dirs $(OBJECTS) $(COBJECTS)
	@echo Linking $(OBJECTS)...
	$(C++) $(CCPARAM) $(OBJECTS) $(COBJECTS) $(LDPARAM) -o $(STORE)/$(TARGET).html


$(STORE)/%.bc: %.cpp
	@echo Creating object file for $*...
	$(C++) $(CCPARAM) $(CXXFLAGS) $(foreach INC,$(INCPATH),-I$(INC)) \
		$(foreach CPPDEF,$(CPPDEFS),-D$(CPPDEF)) $< -o $@

$(STORE)/%.bc: %.c
	@echo Creating object file for $*...
	$(C++) $(CCPARAM) $(foreach INC,$(INCPATH),-I$(INC)) \
		$(foreach CPPDEF,$(CPPDEFS),-D$(CPPDEF)) $< -o $@

%.h: ;

clean:
	@echo Making clean.
	@-rm -rf $(STORE)

backup:
	@-if [ ! -e build/backup ]; then mkdir -p build/backup; fi;
	@zip build/backup/backup_`date +%d-%m-%y_%H.%M`.zip $(SOURCE) $(HEADERS) $(EXTRA_FILES)

dirs:
	@-if [ ! -e $(STORE) ]; then mkdir -p $(STORE); fi;
	@-$(foreach DIR,$(DIRS), if [ ! -e $(STORE)/$(DIR) ]; then mkdir -p $(STORE)/$(DIR); fi; )

run: $(TARGET)
	emrun --no_browser --no_emrun_detect --port 8080 $(STORE)/$(TARGET).html

