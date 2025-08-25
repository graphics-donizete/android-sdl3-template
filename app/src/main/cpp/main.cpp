#include <cassert>

#include <android_native_app_glue.h>
#include <android/log.h>

static void android_main_cmd(struct android_app *state, int32_t cmd);

static int32_t android_main_event(struct android_app *state, AInputEvent *event);

void android_main(struct android_app *state) {
    __android_log_print(ANDROID_LOG_DEBUG, __FUNCTION__, "It lives!!!");

    state->onAppCmd = android_main_cmd;
    state->onInputEvent = android_main_event;

    int ident;
    int events;
    struct android_poll_source *source;

    while (state->running) {
        while ((ident = ALooper_pollOnce(0, nullptr, &events, (void **) &source)) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }

            if (ident == LOOPER_ID_USER) {

            }
        }
    }
}

static void android_main_cmd(struct android_app *state, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            assert(state->window);
            __android_log_print(ANDROID_LOG_DEBUG, __FUNCTION__, "Window created");
            break;
        }
    }
}

int32_t android_main_event(struct android_app *state, AInputEvent *event) {

    return 1;
}