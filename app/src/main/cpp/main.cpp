#include <cassert>
#include <random>

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

    while (!state->destroyRequested) {
        while ((ident = ALooper_pollOnce(0, nullptr, &events, (void **) &source)) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }

            if (ident == LOOPER_ID_USER) {

            }
        }

        if (state->window != nullptr) {
            ANativeWindow *window = state->window;
            ANativeWindow_acquire(window);

            ANativeWindow_Buffer buf;
            if (ANativeWindow_lock(window, &buf, nullptr) < 0) {
                return;
            }

            uint32_t format = buf.format;
            if (format == WINDOW_FORMAT_RGB_565) {
                int width = buf.width;
                int height = buf.height;
                auto *pixels = (uint16_t *) buf.bits;
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        pixels[y * width + x] = std::rand();
                    }
                }
            }

            ANativeWindow_unlockAndPost(window);
            ANativeWindow_release(window);
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