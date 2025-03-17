SHELL := /bin/bash

SRC_DIR := src
WWW_DIR := $(SRC_DIR)/www
DIST_DIR := dist

VERSION := 1.0.0

CXXFLAGS := -O2 -std=c++17

LINUX_CXX   := g++
WINDOWS_CXX := x86_64-w64-mingw32-g++

SRCS := $(wildcard $(SRC_DIR)/*.cpp)

.PHONY: all build linux windows test clean fullclean

all: build

build: linux windows

linux: $(SRCS)
	@mkdir -p $(DIST_DIR)
	$(LINUX_CXX) $(CXXFLAGS) $(SRCS) -o $(DIST_DIR)/Europa_Launcher-$(VERSION)
	@echo "Linux build complete: $(DIST_DIR)/Europa_Launcher-$(VERSION)"

windows: $(SRCS)
	@mkdir -p $(DIST_DIR)
	$(WINDOWS_CXX) $(CXXFLAGS) $(SRCS) -o $(DIST_DIR)/Europa_Launcher-$(VERSION).exe
	@echo "Windows build complete: $(DIST_DIR)/Europa_Launcher-$(VERSION).exe"

test: $(SRCS)
	$(LINUX_CXX) $(CXXFLAGS) $(SRCS) -o Europa_Launcher-$(VERSION)_test
	@echo "Test build complete: ./Europa_Launcher-$(VERSION)_test"

clean:
	rm -f Europa_Launcher-$(VERSION)_test *.o

fullclean: clean
	rm -rf $(DIST_DIR) cache
