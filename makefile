BUILD_DIR=build
CC := clang
CXX := clang++
CC := /usr/local/opt/llvm/bin/clang
CXX:= /usr/local/opt/llvm/bin/clang++

#GENERATOR := Ninja

ifeq ($(GENERATOR),Ninja)
	BUILD_TOOL := ninja
else
	BUILD_TOOL := $(MAKE)
	GENERATOR  := "Unix Makefiles"
endif

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); cmake -G$(GENERATOR) -DCMAKE_C_COMPILER=$(CC) -DCMAKE_CXX_COMPILER=$(CXX) -DCMAKE_BUILD_TYPE=Debug ..
	$(BUILD_TOOL) -C $(BUILD_DIR) 2>&1 | tee $(BUILD_DIR)/build.log

clean:
	rm -rf $(BUILD_DIR)


run: all
	$(BUILD_DIR)/bin/out data/test0.x 2>&1 | tee $(BUILD_DIR)/run.log

