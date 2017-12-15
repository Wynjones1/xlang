BUILD_DIR=build

all:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR); cmake ..
	$(MAKE) -C $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)


run: all
	$(BUILD_DIR)/out
