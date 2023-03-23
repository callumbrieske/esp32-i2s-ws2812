idf_component_register(SRCS "ws2812.c" INCLUDE_DIRS include PRIV_REQUIRES driver esp_timer)

# set(COMPONENT_ADD_INCLUDEDIRS include)
# set(COMPONENT_SRCS "ws2812.c")
# register_component()