bin_PROGRAMS=pitft_agent
pitft_agent_SOURCES=src/pitft_agent.cpp   \
    src/framebuffer.cpp src/framebuffer.h \
    src/device_scanner.cpp src/device_scanner.h \
    src/device.h
pitft_agent_CFLAGS = $(LIBEVDEV_CFLAGS)
pitft_agent_LDADD = $(LIBEVDEV_LIBS) -lstdc++fs