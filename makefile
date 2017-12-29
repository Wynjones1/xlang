BUILD_DIR  := build
BUILD_TYPE := Debug
TEST       ?= test0.x

export CC                   := $(shell which clang)
export CXX                  := $(shell which clang++)
export ASAN_OPTIONS         := vebose=1:symbolize=1
export ASAN_SYMBOLIZER_PATH := $(shell which llvm-symbolizer)

LINKER=ld

#GENERATOR := Ninja

ifeq ($(GENERATOR),Ninja)
	BUILD_TOOL := ninja
else
	BUILD_TOOL := $(MAKE) -j
	GENERATOR  := "Unix Makefiles"
endif

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); cmake -G$(GENERATOR) \
						-DCMAKE_C_COMPILER=$(CC) \
						-DCMAKE_CXX_COMPILER=$(CXX) \
						-DCMAKE_LINKER=$(LINKER) \
						-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ..
	$(BUILD_TOOL) -C $(BUILD_DIR) 2>&1 | tee -a $(BUILD_DIR)/build.log

clean:
	rm -rf $(BUILD_DIR)


run: all
	$(BUILD_DIR)/bin/out data/$(TEST) 2>&1 | tee $(BUILD_DIR)/run.log

test: all
	$(BUILD_DIR)/bin/unittests 2>&1 | tee $(BUILD_DIR)/test.log

debug:
	lldb -- $(BUILD_DIR)/bin/out data/$(TEST) 2>&1 | tee $(BUILD_DIR)/run.log
