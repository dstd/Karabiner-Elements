#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void krbn_inputsource_observer_objc;
typedef void (*krbn_inputsource_observer_callback)(const char* inputsource_id,
                                                   void* context);
void krbn_inputsource_observer_initialize(krbn_inputsource_observer_objc** observer,
                                          krbn_inputsource_observer_callback callback,
                                          void* context);
void krbn_inputsource_observer_terminate(krbn_inputsource_observer_objc** observer);

#ifdef __cplusplus
}
#endif
