SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,%.o,$(SRCS))
LIB_NAME := libapp_onvif

TARGET := $(LIB_NAME).so

ONVIF_INCLUDE:=$(shell pwd)/include
CFLAGS += $(addprefix -I,$(ONVIF_INCLUDE))

.PHONY: $(TARGET) install

$(TARGET): $(OBJS)
	$(LOCAL_GEN) $(LOCAL_COMMON_FLAG) $@ $^
	$(CP) $(TARGET) $(APP_LIB_DIR)
	@echo ""

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo ""

clean:
	$(RM) *.o
	$(RM) $(TARGET)
	$(RM) $(APP_LIB_DIR)/$(TARGET)

install:
	@echo ""
