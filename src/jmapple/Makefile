# JuggleMaster for Apple platforms. `make help` lists the targets.

PROJECT     = JuggleMaster.xcodeproj
DERIVED     = build/dd
# First available simulator of each kind; override with an id
TV_SIM      ?= $(shell xcrun simctl list devices available | grep -m1 'Apple TV 4K' | grep -oE '[0-9A-F-]{36}')
IOS_SIM     ?= $(shell xcrun simctl list devices available | grep -m1 'iPhone' | grep -oE '[0-9A-F-]{36}')
IPAD_SIM    ?= $(shell xcrun simctl list devices available | grep -m1 'iPad Pro' | grep -oE '[0-9A-F-]{36}')
# A paired Apple TV (xcrun devicectl list devices); override with TV_DEVICE=<udid>
TV_DEVICE   ?= 00008110-00143922148A401E
BUNDLE_ID   = net.jugglemaster.JuggleMaster
XCODEBUILD  = xcodebuild -project $(PROJECT) -derivedDataPath $(DERIVED)
FILTER      = | scripts/xcfilter.sh; exit $${PIPESTATUS[0]}
SHELL       = /bin/bash

.PHONY: help project build build-tv build-ios build-mac test test-mac test-mac-ui test-tv test-ios test-ipad test-kit lint format install-tv clean

help:
	@echo "project     generate $(PROJECT) with xcodegen"
	@echo "build       build-tv, build-ios and build-mac"
	@echo "test        JMKit, then JugglerKit/UI tests on macOS and the tvOS, iPhone and iPad simulators"
	@echo "lint        swift format lint --strict"
	@echo "format      swift format in place"
	@echo "install-tv  build for TV_DEVICE, install and launch"

project:
	xcodegen generate --quiet

build: build-tv build-ios build-mac

build-tv: project
	$(XCODEBUILD) -scheme JuggleMasterTV -destination 'generic/platform=tvOS Simulator' build $(FILTER)

build-ios: project
	$(XCODEBUILD) -scheme JuggleMaster -destination 'generic/platform=iOS Simulator' build $(FILTER)

build-mac: project
	$(XCODEBUILD) -scheme JuggleMaster -destination 'platform=macOS,arch=arm64' build $(FILTER)

test: test-kit test-mac test-mac-ui test-tv test-ios test-ipad

test-kit:
	cd JMKit && swift test 2>&1 | grep -E "✘|error|Test run|passed after|failed after"; exit $${PIPESTATUS[0]}

test-mac: project
	$(XCODEBUILD) -scheme JugglerKit -destination 'platform=macOS,arch=arm64' test $(FILTER)

test-mac-ui: project
	$(XCODEBUILD) -scheme JuggleMasterMac -destination 'platform=macOS,arch=arm64' test $(FILTER)

test-tv: project
	$(XCODEBUILD) -scheme JuggleMasterTV -destination 'platform=tvOS Simulator,id=$(TV_SIM)' test $(FILTER)

test-ios: project
	$(XCODEBUILD) -scheme JuggleMaster -destination 'platform=iOS Simulator,id=$(IOS_SIM)' test $(FILTER)

test-ipad: project
	$(XCODEBUILD) -scheme JuggleMaster -destination 'platform=iOS Simulator,id=$(IPAD_SIM)' -only-testing:JuggleMasterUITests test $(FILTER)

lint:
	swift format lint --strict --recursive --parallel App TV JugglerKit JMKit AppUITests TVUITests MacUITests

format:
	swift format --in-place --recursive --parallel App TV JugglerKit JMKit AppUITests TVUITests MacUITests

install-tv: project
	$(XCODEBUILD) -scheme JuggleMasterTV -configuration Release -destination 'id=$(TV_DEVICE)' -allowProvisioningUpdates build $(FILTER)
	xcrun devicectl device install app --device $(TV_DEVICE) $(DERIVED)/Build/Products/Release-appletvos/JuggleMaster.app
	xcrun devicectl device process launch --device $(TV_DEVICE) $(BUNDLE_ID)

clean:
	rm -rf build $(PROJECT)
