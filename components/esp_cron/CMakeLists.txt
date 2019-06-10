set(COMPONENT_SRCDIRS library/ccronexpr library/jobs test src .)
set(COMPONENT_ADD_INCLUDEDIRS include ${ESP_IDF}/tools/unit-test-app/components/unity/include/ library/ccronexpr library/jobs .)

set(COMPONENT_REQUIRES log)

register_component()
