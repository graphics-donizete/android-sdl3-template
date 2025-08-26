#include <cassert>
#include <random>

#include <android_native_app_glue.h>
#include <android/log.h>

static uint16_t static_pixels[2400 * 1080];

static void android_main_cmd(struct android_app *state, int32_t cmd);

static int32_t android_main_input_event(struct android_app *state, AInputEvent *event);

void android_main(struct android_app *state) {
    __android_log_print(ANDROID_LOG_DEBUG, __FUNCTION__, "It lives!!!");

    state->onAppCmd = android_main_cmd;
    state->onInputEvent = android_main_input_event;

    int ident;
    int events;
    struct android_poll_source *source;

    while (!state->destroyRequested) {
        while ((ident = ALooper_pollOnce(0, nullptr, &events, (void **) &source)) >= 0) {
            if (source != nullptr) {
                source->process(state, source);
            }

            switch (ident) {
                case LOOPER_ID_INPUT: {
                    break;
                }
                case LOOPER_ID_USER: {
                    break;
                }
            }
        }
    }
}

static void android_main_cmd(struct android_app *state, int32_t cmd) {
    switch (cmd) {
        case APP_CMD_INIT_WINDOW: {
            assert(state->window);
            __android_log_print(ANDROID_LOG_DEBUG, __FUNCTION__, "Window created");

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
                memset(static_pixels, 0xF800, width * height * sizeof(uint16_t));
                memcpy(buf.bits, static_pixels, buf.width * buf.height * sizeof(uint16_t));
            }

            ANativeWindow_unlockAndPost(window);
            ANativeWindow_release(window);
            break;
        }
    }
}

int32_t android_main_input_event(struct android_app *state, AInputEvent *event) {
    int32_t type = AInputEvent_getType(event);
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        int32_t index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        float x = AMotionEvent_getX(event, index);
        float y = AMotionEvent_getY(event, index);
        __android_log_print(ANDROID_LOG_DEBUG, __FUNCTION__, "Motion event: x=%f, y=%f", x, y);

        draw:
        {
            ANativeWindow *window = state->window;
            ANativeWindow_acquire(window);

            ANativeWindow_Buffer buf;
            if (ANativeWindow_lock(state->window, &buf, nullptr) >= 0) {
                static_pixels[(int) y * buf.width + (int) x] = 0xFFFF;
                memcpy(buf.bits, static_pixels, buf.width * buf.height * sizeof(uint16_t));
                ANativeWindow_unlockAndPost(window);
            }
            ANativeWindow_release(window);
        }
    }
    return 1;
}