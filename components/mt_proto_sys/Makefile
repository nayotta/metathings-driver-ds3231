PROTOC := $(shell which protoc-c)

PROTO_FILES := $(shell ls *.proto)

all:
	$(PROTOC) --c_out=.	$(PROTO_FILES)