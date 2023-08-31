#pragma once

#pragma region INCLUDES
#include <stdarg.h>     // valist
#include <stddef.h>     // ptrdiff_t
#include <stdlib.h>     // malloc, realloc, free
#include <limits.h>     // INT32_MAX, UINT32_MAX
#include <string.h>     // memset
#include <float.h>      // FLT_MAX
#include <stdio.h>      // FILE
#include <time.h>       // time
#include <ctype.h>      // tolower
#include <assert.h>     // assert
#include <malloc.h>     // alloca/_alloca

#include "oslo_math.h"

#pragma endregion

#pragma region PLATFORM_DEFINES

/*===================
// PLATFORM DEFINES
===================*/ 

#if (defined __ANDROID__)

    #define OSLO_PLATFORM_ANDROID

/* Platform Apple */
#elif (defined __APPLE__ || defined _APPLE)

    #define OSLO_PLATFORM_APPLE

/* Platform Windows */
#elif (defined _WIN32 || defined _WIN64)

    #define __USE_MINGW_ANSI_STDIO  1

    // Necessary windows defines before including windows.h, because it's retarded.
    #define OEMRESOURCE

    #define OSLO_PLATFORM_WIN
    #define OSLO_PLATFORM_WINDOWS
    #include <windows.h>

    #define WIN32_LEAN_AND_MEAN

/* Platform Linux */
#elif (defined linux || defined _linux || defined __linux__)

    #define OSLO_PLATFORM_LINUX

/* Platform Emscripten */
#elif (defined __EMSCRIPTEN__)

    #define OSLO_PLATFORM_WEB

/* Else - Platform Undefined and Unsupported or custom */

#endif
#pragma endregion

#pragma region COLOR_DEFINES

// Color defines
#define OSLO_COLOR_RED v4(1, 0, 0, 1)
#define OSLO_COLOR_GREEN v4(0, 1, 0, 1)
#define OSLO_COLOR_BLUE v4(0, 0, 1, 1)
#define OSLO_COLOR_WHITE v4(1, 1, 1, 1)

#pragma endregion

#pragma region DYN_ARRAY
/*===================================
// Dynamic Array
===================================*/

typedef struct oslo_dyn_array
{
    int32_t size;
    int32_t capacity;
} oslo_dyn_array;

#define oslo_dyn_array_head(__ARR)\
    ((oslo_dyn_array*)((uint8_t*)(__ARR) - sizeof(oslo_dyn_array)))

#define oslo_dyn_array_size(__ARR)\
    (__ARR == NULL ? 0 : oslo_dyn_array_head((__ARR))->size)

#define oslo_dyn_array_capacity(__ARR)\
    (__ARR == NULL ? 0 : oslo_dyn_array_head((__ARR))->capacity)

#define oslo_dyn_array_full(__ARR)\
    ((oslo_dyn_array_size((__ARR)) == oslo_dyn_array_capacity((__ARR))))    

void* oslo_dyn_array_resize_impl(void* arr, size_t sz, size_t amount);

#define oslo_dyn_array_need_grow(__ARR, __N)\
    ((__ARR) == 0 || oslo_dyn_array_size(__ARR) + (__N) >= oslo_dyn_array_capacity(__ARR))

#define oslo_dyn_array_grow(__ARR)\
    oslo_dyn_array_resize_impl((__ARR), sizeof(*(__ARR)), oslo_dyn_array_capacity(__ARR) ? oslo_dyn_array_capacity(__ARR) * 2 : 1)

#define oslo_dyn_array_grow_size(__ARR, __SZ  )\
    oslo_dyn_array_resize_impl((__ARR), (__SZ ), oslo_dyn_array_capacity(__ARR) ? oslo_dyn_array_capacity(__ARR) * 2 : 1)

void** oslo_dyn_array_init(void** arr, size_t val_len);

void oslo_dyn_array_push_data(void** arr, void* val, size_t val_len);

oslo_inline void oslo_dyn_array_set_data_i(void** arr, void* val, size_t val_len, uint32_t offset)
{
    memcpy(((char*)(*arr)) + offset * val_len, val, val_len);
}

#define oslo_dyn_array_push(__ARR, __ARRVAL)\
    do {\
        oslo_dyn_array_init((void**)&(__ARR), sizeof(*(__ARR)));\
        if (!(__ARR) || ((__ARR) && oslo_dyn_array_need_grow(__ARR, 1))) {\
            *((void **)&(__ARR)) = oslo_dyn_array_grow(__ARR); \
        }\
        (__ARR)[oslo_dyn_array_size(__ARR)] = (__ARRVAL);\
        oslo_dyn_array_head(__ARR)->size++;\
    } while(0)

#define oslo_dyn_array_reserve(__ARR, __AMOUNT)\
    do {\
        if ((!__ARR)) oslo_dyn_array_init((void**)&(__ARR), sizeof(*(__ARR)));\
        if ((!__ARR) || (size_t)__AMOUNT > oslo_dyn_array_capacity(__ARR)) {\
            *((void **)&(__ARR)) = oslo_dyn_array_resize_impl(__ARR, sizeof(*__ARR), __AMOUNT);\
        }\
    } while(0)

#define oslo_dyn_array_empty(__ARR)\
    (oslo_dyn_array_init((void**)&(__ARR), sizeof(*(__ARR))), (oslo_dyn_array_size(__ARR) == 0))

#define oslo_dyn_array_pop(__ARR)\
    do {\
        if (__ARR && !oslo_dyn_array_empty(__ARR)) {\
            oslo_dyn_array_head(__ARR)->size -= 1;\
        }\
    } while (0)

#define oslo_dyn_array_back(__ARR)\
    *(__ARR + (oslo_dyn_array_size(__ARR) ? oslo_dyn_array_size(__ARR) - 1 : 0))

#define oslo_dyn_array_for(__ARR, __T, __IT_NAME)\
    for (__T* __IT_NAME = __ARR; __IT_NAME != oslo_dyn_array_back(__ARR); ++__IT_NAME)

#define oslo_dyn_array_new(__T)\
    ((__T*)oslo_dyn_array_resize_impl(NULL, sizeof(__T), 0))

#define oslo_dyn_array_clear(__ARR)\
    do {\
        if (__ARR) {\
            oslo_dyn_array_head(__ARR)->size = 0;\
        }\
    } while (0)

#define oslo_dyn_array(__T)   __T*

#define oslo_dyn_array_free(__ARR)\
    do {\
        if (__ARR) {\
            free(oslo_dyn_array_head(__ARR));\
            (__ARR) = NULL;\
        }\
    } while (0)

#pragma endregion

#pragma region SLOT_ARRAY
/*===================================
// Slot Array
===================================*/

#define oslo_slot_array_INVALID_HANDLE    UINT32_MAX

#define oslo_slot_array_handle_valid(__SA, __ID)\
    (__ID < oslo_dyn_array_size((__SA)->indices) && (__SA)->indices[__ID] != oslo_slot_array_INVALID_HANDLE)

typedef struct __oslo_slot_array_dummy_header {
    oslo_dyn_array(uint32_t) indices;
    oslo_dyn_array(uint32_t) data;
} __oslo_slot_array_dummy_header;

#define oslo_slot_array(__T)\
    struct\
    {\
        oslo_dyn_array(uint32_t) indices;\
        oslo_dyn_array(__T) data;\
        __T tmp;\
    }*

#define oslo_slot_array_new(__T)\
    NULL

oslo_inline
uint32_t __oslo_slot_array_find_next_available_index(oslo_dyn_array(uint32_t) indices)
{
    uint32_t idx = oslo_slot_array_INVALID_HANDLE;
    for (uint32_t i = 0; i < (uint32_t)oslo_dyn_array_size(indices); ++i)
    {
        uint32_t handle = indices[i];
        if (handle == oslo_slot_array_INVALID_HANDLE)
        {
            idx = i;
            break;
        }
    }
    if (idx == oslo_slot_array_INVALID_HANDLE)
    {
        idx = oslo_dyn_array_size(indices);
    }

    return idx;
}

 void** 
oslo_slot_array_init(void** sa, size_t sz);

#define oslo_slot_array_init_all(__SA)\
    (oslo_slot_array_init((void**)&(__SA), sizeof(*(__SA))), oslo_dyn_array_init((void**)&((__SA)->indices), sizeof(uint32_t)),\
        oslo_dyn_array_init((void**)&((__SA)->data), sizeof((__SA)->tmp)))

oslo_inline
uint32_t oslo_slot_array_insert_func(void** indices, void** data, void* val, size_t val_len, uint32_t* ip)
{
    // Find next available index
    u32 idx = __oslo_slot_array_find_next_available_index((uint32_t*)*indices);

    if (idx == oslo_dyn_array_size(*indices)) {
        uint32_t v = 0;
        oslo_dyn_array_push_data(indices, &v, sizeof(uint32_t));  
        idx = oslo_dyn_array_size(*indices) - 1;
    }

    // Push data to array
    oslo_dyn_array_push_data(data, val, val_len);

    // Set data in indices
    uint32_t bi = oslo_dyn_array_size(*data) - 1;
    oslo_dyn_array_set_data_i(indices, &bi, sizeof(uint32_t), idx);

    if (ip){
        *ip = idx;
    }

    return idx;
}

#define oslo_slot_array_reserve(__SA, __NUM)\
    do {\
        oslo_slot_array_init_all(__SA);\
        oslo_dyn_array_reserve((__SA)->data, __NUM);\
        oslo_dyn_array_reserve((__SA)->indices, __NUM);\
    } while (0)

#define oslo_slot_array_insert(__SA, __VAL)\
    (oslo_slot_array_init_all(__SA), (__SA)->tmp = (__VAL),\
        oslo_slot_array_insert_func((void**)&((__SA)->indices), (void**)&((__SA)->data), (void*)&((__SA)->tmp), sizeof(((__SA)->tmp)), NULL))

#define oslo_slot_array_insert_hp(__SA, __VAL, __hp)\
    (oslo_slot_array_init_all(__SA), (__SA)->tmp = (__VAL),\
        oslo_slot_array_insert_func((void**)&((__SA)->indices), (void**)&((__SA)->data), &((__SA)->tmp), sizeof(((__SA)->tmp)), (__hp)))

#define oslo_slot_array_insert_no_init(__SA, __VAL)\
    ((__SA)->tmp = (__VAL), oslo_slot_array_insert_func((void**)&((__SA)->indices), (void**)&((__SA)->data), &((__SA)->tmp), sizeof(((__SA)->tmp)), NULL))

#define oslo_slot_array_size(__SA)\
    ((__SA) == NULL ? 0 : oslo_dyn_array_size((__SA)->data))

 #define oslo_slot_array_empty(__SA)\
    (oslo_slot_array_size(__SA) == 0)

#define oslo_slot_array_clear(__SA)\
    do {\
        if ((__SA) != NULL) {\
            oslo_dyn_array_clear((__SA)->data);\
            oslo_dyn_array_clear((__SA)->indices);\
        }\
    } while (0)

#define oslo_slot_array_exists(__SA, __SID)\
    ((__SA) && (__SID) < (uint32_t)oslo_dyn_array_size((__SA)->indices) && (__SA)->indices[__SID] != oslo_slot_array_INVALID_HANDLE)

 #define oslo_slot_array_get(__SA, __SID)\
    ((__SA)->data[(__SA)->indices[(__SID) % oslo_dyn_array_size(((__SA)->indices))]])

 #define oslo_slot_array_getp(__SA, __SID)\
    (&(oslo_slot_array_get(__SA, (__SID))))

 #define oslo_slot_array_free(__SA)\
    do {\
        if ((__SA) != NULL) {\
            oslo_dyn_array_free((__SA)->data);\
            oslo_dyn_array_free((__SA)->indices);\
            (__SA)->indices = NULL;\
            (__SA)->data = NULL;\
            free((__SA));\
            (__SA) = NULL;\
        }\
    } while (0)

 #define oslo_slot_array_erase(__SA, __id)\
    do {\
        uint32_t __H0 = (__id) /*% oslo_dyn_array_size((__SA)->indices)*/;\
        if (oslo_slot_array_size(__SA) == 1) {\
            oslo_slot_array_clear(__SA);\
        }\
        else if (!oslo_slot_array_handle_valid(__SA, __H0)) {\
            notify_error(instance, SLOT_ARRAY_ERROR, "Attempting to erase invalid slot array handle");\
        }\
        else {\
            uint32_t __OG_DATA_IDX = (__SA)->indices[__H0];\
            /* Iterate through handles until last index of data found */\
            uint32_t __H = 0;\
            for (uint32_t __I = 0; __I < oslo_dyn_array_size((__SA)->indices); ++__I)\
            {\
                if ((__SA)->indices[__I] == oslo_dyn_array_size((__SA)->data) - 1)\
                {\
                    __H = __I;\
                    break;\
                }\
            }\
        \
            /* Swap and pop data */\
            (__SA)->data[__OG_DATA_IDX] = oslo_dyn_array_back((__SA)->data);\
            oslo_dyn_array_pop((__SA)->data);\
        \
            /* Point new handle, Set og handle to invalid */\
            (__SA)->indices[__H] = __OG_DATA_IDX;\
            (__SA)->indices[__H0] = oslo_slot_array_INVALID_HANDLE;\
        }\
    } while (0)

/*=== Slot Array Iterator ===*/

// Slot array iterator new
typedef uint32_t oslo_slot_array_iter;

#define oslo_slot_array_iter_valid(__SA, __IT)\
    (__SA && oslo_slot_array_exists(__SA, __IT))

oslo_inline
void _oslo_slot_array_iter_advance_func(oslo_dyn_array(uint32_t) indices, uint32_t* it)
{
    if (!indices) {
       *it = oslo_slot_array_INVALID_HANDLE; 
        return;
    }

    (*it)++;
    for (; *it < (uint32_t)oslo_dyn_array_size(indices); ++*it)
    {\
        if (indices[*it] != oslo_slot_array_INVALID_HANDLE)\
        {\
            break;\
        }\
    }\
}

oslo_inline
uint32_t _oslo_slot_array_iter_find_first_valid_index(oslo_dyn_array(uint32_t) indices)
{
    if (!indices) return oslo_slot_array_INVALID_HANDLE;

    for (uint32_t i = 0; i < (uint32_t)oslo_dyn_array_size(indices); ++i)
    {
        if (indices[i] != oslo_slot_array_INVALID_HANDLE)
        {
            return i;
        }
    }
    return oslo_slot_array_INVALID_HANDLE;
}

#define oslo_slot_array_iter_new(__SA) (_oslo_slot_array_iter_find_first_valid_index((__SA) ? (__SA)->indices : 0))

#define oslo_slot_array_iter_advance(__SA, __IT)\
    _oslo_slot_array_iter_advance_func((__SA) ? (__SA)->indices : NULL, &(__IT))

#define oslo_slot_array_iter_get(__SA, __IT)\
    oslo_slot_array_get(__SA, __IT)

#define oslo_slot_array_iter_getp(__SA, __IT)\
    oslo_slot_array_getp(__SA, __IT)

#pragma endregion

#pragma region AUDIO

typedef struct oslo_audio_source_t
{
    int32_t channels;
    int32_t sample_rate;
    void* samples;
    int32_t sample_count;
} oslo_audio_source_t;

typedef uint32_t oslo_audio_source_id;
typedef uint32_t oslo_audio_instance_id;

typedef struct oslo_audio_instance_t
{
    oslo_audio_source_id src;
    float volume;
    float pitch;
    bool loop;
    bool persistent;
    bool playing;
    double sample_position;
} oslo_audio_instance_t;

typedef void (*oslo_audio_commit)(int16_t* output, uint32_t num_channels, uint32_t sample_rate, uint32_t frame_count);

typedef struct oslo_audio_t
{
    oslo_slot_array(oslo_audio_source_t) sources;
    oslo_slot_array(oslo_audio_instance_t) instances;
    float max_volume;
    float min_volume;
    void* sample_out;

    void* user_data;

    oslo_audio_commit commit;
} oslo_audio_t;

void oslo_audio_register_commit(oslo_audio_commit commit);
oslo_audio_source_id oslo_audio_load_from_file(const char* path);
oslo_audio_instance_id oslo_audio_create_instance(oslo_audio_instance_t* inst);
void oslo_audio_mutex_lock();
void oslo_audio_mutex_unlock();

void oslo_audio_play_source(oslo_audio_source_id src, float volume);
void oslo_audio_play(oslo_audio_instance_id inst);
void oslo_audio_pause(oslo_audio_instance_id inst);
void oslo_audio_stop(oslo_audio_instance_id inst);
void oslo_audio_restart(oslo_audio_instance_id inst);
bool oslo_audio_is_playing(oslo_audio_instance_id inst);
float oslo_audio_get_volume(oslo_audio_instance_id inst);
void oslo_audio_set_volume(oslo_audio_instance_id inst, float volume);

bool oslo_audio_load_ogg_from_file(const char* path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples);
bool oslo_audio_load_wav_from_file(const char* path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples);
bool oslo_audio_load_mp3_from_file(const char* path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples);

#pragma endregion

#pragma region OSLO_2D_STRUCTS
/*================================================================================
// Oslo 2D
================================================================================*/

// Helper macro for an in place for-range loop
#define oslo_for_range_i(__COUNT)\
    for (uint32_t i = 0; i < __COUNT; ++i)

typedef struct oslo_rect_t
{
    vec2 min;
    vec2 max;
} oslo_rect_t;

typedef enum oslo_error_code
{
    OSLO_SHADER_COMPILE_ERROR,
    OSLO_SHADER_LINKER_ERROR,
    OSLO_PLATFORM_INIT_ERROR,
    OSLO_GFX_INIT_ERROR,
    OSLO_AUDIO_INIT_ERROR,
    OSLO_LOAD_ERROR,
    SLOT_ARRAY_ERROR
} oslo_error_code;

typedef struct oslo_desc_t
{
	u32 window_width;
	u32 window_height;
	const char* window_title;
	bool debug_graphics;
    float max_fps;

	void(*init)(void*);
	void(*update)(void*);
	void(*shutdown)(void*);
    void(*on_oslo_error)(oslo_error_code, const char*);

	void* user_data;
} oslo_desc_t;

typedef struct oslo_gfx_vertex_t
{
    vec4 position;
    vec4 color;
    vec2 uv;
    float tex_index;
} oslo_gfx_vertex_t;

typedef struct oslo_gfx_texture_t
{
    int id;
    uint32_t width;
    uint32_t height;
    uint32_t channels;
} oslo_gfx_texture_t;

#define MAX_BATCH_TEXTURES 32

typedef uint32_t oslo_texture_id;

typedef struct oslo_gfx_quad_batch_t
{
    int vao;
    int vbo;
    int ibo;

    oslo_gfx_vertex_t* vertices;
    oslo_gfx_vertex_t* vert_ptr;

    float texture_index;
    oslo_texture_id bound_textures[MAX_BATCH_TEXTURES];

    uint32_t max_indices;
    uint32_t index_count;
} oslo_gfx_quad_batch_t;

typedef struct draw_texture_section_desc_t
{
    vec2 position;
    float rotation;
    vec2 size;
    vec4 color;
    oslo_rect_t section;
    oslo_texture_id texture;
    bool flip_horizontal;
} draw_texture_section_desc_t;

typedef struct draw_textured_quad_desc_t
{
    vec4 quad[4];
    vec4 color;
    oslo_rect_t section;
    oslo_texture_id texture;
    bool flip_horizontal;
} draw_textured_quad_desc_t;

typedef struct draw_texture_desc_t
{
    vec2 position;
    float rotation;
    vec2 size;
    vec4 color;
    oslo_texture_id texture;
} draw_texture_desc_t;

typedef struct draw_quad_desc_t
{
    vec2 position;
    float rotation;
    vec2 size;
    vec4 color;
} draw_quad_desc_t;

typedef struct oslo_gfx_t
{
    int shader;
    mat4 projection;
    oslo_texture_id white_texture;

    oslo_gfx_quad_batch_t default_batch;

    oslo_slot_array(oslo_gfx_texture_t) textures;
} oslo_gfx_t;

typedef enum oslo_mouse_button_code
{
    OSLO_MOUSE_LBUTTON,
    OSLO_MOUSE_RBUTTON,
    OSLO_MOUSE_MBUTTON,
    OSLO_MOUSE_BUTTON_CODE_COUNT
} oslo_mouse_button_code;

typedef enum oslo_keycode
{
    OSLO_KEYCODE_INVALID,
    OSLO_KEYCODE_SPACE,
    OSLO_KEYCODE_APOSTROPHE,  /* ' */
    OSLO_KEYCODE_COMMA,  /* , */
    OSLO_KEYCODE_MINUS,  /* - */
    OSLO_KEYCODE_PERIOD,  /* . */
    OSLO_KEYCODE_SLASH,  /* / */
    OSLO_KEYCODE_0,
    OSLO_KEYCODE_1,
    OSLO_KEYCODE_2,
    OSLO_KEYCODE_3,
    OSLO_KEYCODE_4,
    OSLO_KEYCODE_5,
    OSLO_KEYCODE_6,
    OSLO_KEYCODE_7,
    OSLO_KEYCODE_8,
    OSLO_KEYCODE_9,
    OSLO_KEYCODE_SEMICOLON,  /* ; */
    OSLO_KEYCODE_EQUAL,  /* = */
    OSLO_KEYCODE_A,
    OSLO_KEYCODE_B,
    OSLO_KEYCODE_C,
    OSLO_KEYCODE_D,
    OSLO_KEYCODE_E,
    OSLO_KEYCODE_F,
    OSLO_KEYCODE_G,
    OSLO_KEYCODE_H,
    OSLO_KEYCODE_I,
    OSLO_KEYCODE_J,
    OSLO_KEYCODE_K,
    OSLO_KEYCODE_L,
    OSLO_KEYCODE_M,
    OSLO_KEYCODE_N,
    OSLO_KEYCODE_O,
    OSLO_KEYCODE_P,
    OSLO_KEYCODE_Q,
    OSLO_KEYCODE_R,
    OSLO_KEYCODE_S,
    OSLO_KEYCODE_T,
    OSLO_KEYCODE_U,
    OSLO_KEYCODE_V,
    OSLO_KEYCODE_W,
    OSLO_KEYCODE_X,
    OSLO_KEYCODE_Y,
    OSLO_KEYCODE_Z,
    OSLO_KEYCODE_LEFT_BRACKET,  /* [ */
    OSLO_KEYCODE_BACKSLASH,  /* \ */
    OSLO_KEYCODE_RIGHT_BRACKET,  /* ] */
    OSLO_KEYCODE_GRAVE_ACCENT,  /* ` */
    OSLO_KEYCODE_WORLD_1, /* non-US #1 */
    OSLO_KEYCODE_WORLD_2, /* non-US #2 */
    OSLO_KEYCODE_ESC,
    OSLO_KEYCODE_ENTER,
    OSLO_KEYCODE_TAB,
    OSLO_KEYCODE_BACKSPACE,
    OSLO_KEYCODE_INSERT,
    OSLO_KEYCODE_DELETE,
    OSLO_KEYCODE_RIGHT,
    OSLO_KEYCODE_LEFT,
    OSLO_KEYCODE_DOWN,
    OSLO_KEYCODE_UP,
    OSLO_KEYCODE_PAGE_UP,
    OSLO_KEYCODE_PAGE_DOWN,
    OSLO_KEYCODE_HOME,
    OSLO_KEYCODE_END,
    OSLO_KEYCODE_CAPS_LOCK,
    OSLO_KEYCODE_SCROLL_LOCK,
    OSLO_KEYCODE_NUM_LOCK,
    OSLO_KEYCODE_PRINT_SCREEN,
    OSLO_KEYCODE_PAUSE,
    OSLO_KEYCODE_F1,
    OSLO_KEYCODE_F2,
    OSLO_KEYCODE_F3,
    OSLO_KEYCODE_F4,
    OSLO_KEYCODE_F5,
    OSLO_KEYCODE_F6,
    OSLO_KEYCODE_F7,
    OSLO_KEYCODE_F8,
    OSLO_KEYCODE_F9,
    OSLO_KEYCODE_F10,
    OSLO_KEYCODE_F11,
    OSLO_KEYCODE_F12,
    OSLO_KEYCODE_F13,
    OSLO_KEYCODE_F14,
    OSLO_KEYCODE_F15,
    OSLO_KEYCODE_F16,
    OSLO_KEYCODE_F17,
    OSLO_KEYCODE_F18,
    OSLO_KEYCODE_F19,
    OSLO_KEYCODE_F20,
    OSLO_KEYCODE_F21,
    OSLO_KEYCODE_F22,
    OSLO_KEYCODE_F23,
    OSLO_KEYCODE_F24,
    OSLO_KEYCODE_F25,
    OSLO_KEYCODE_KP_0,
    OSLO_KEYCODE_KP_1,
    OSLO_KEYCODE_KP_2,
    OSLO_KEYCODE_KP_3,
    OSLO_KEYCODE_KP_4,
    OSLO_KEYCODE_KP_5,
    OSLO_KEYCODE_KP_6,
    OSLO_KEYCODE_KP_7,
    OSLO_KEYCODE_KP_8,
    OSLO_KEYCODE_KP_9,
    OSLO_KEYCODE_KP_DECIMAL,
    OSLO_KEYCODE_KP_DIVIDE,
    OSLO_KEYCODE_KP_MULTIPLY,
    OSLO_KEYCODE_KP_SUBTRACT,
    OSLO_KEYCODE_KP_ADD,
    OSLO_KEYCODE_KP_ENTER,
    OSLO_KEYCODE_KP_EQUAL,
    OSLO_KEYCODE_LEFT_SHIFT,
    OSLO_KEYCODE_LEFT_CONTROL,
    OSLO_KEYCODE_LEFT_ALT,
    OSLO_KEYCODE_LEFT_SUPER,
    OSLO_KEYCODE_RIGHT_SHIFT,
    OSLO_KEYCODE_RIGHT_CONTROL,
    OSLO_KEYCODE_RIGHT_ALT,
    OSLO_KEYCODE_RIGHT_SUPER,
    OSLO_KEYCODE_MENU,
    OSLO_KEYCODE_COUNT
} oslo_keycode;

typedef struct oslo_mouse_t
{
    b32 button_map[OSLO_MOUSE_BUTTON_CODE_COUNT];
    b32 prev_map[OSLO_MOUSE_BUTTON_CODE_COUNT];
    vec2 position;
    vec2 delta;
    vec2 wheel;
    b32 moved_this_frame;
    b32 locked;
} oslo_mouse_t;

typedef struct oslo_input_t
{
    b32 key_map[OSLO_KEYCODE_COUNT];
    b32 prev_key_map[OSLO_KEYCODE_COUNT];
    oslo_mouse_t mouse;
} oslo_input_t;

typedef struct oslo_time_t
{
    float previous;
    float elapsed;
    float delta;
} oslo_time_t;

typedef struct oslo_t
{
	oslo_desc_t desc;
    oslo_gfx_t gfx;
    oslo_audio_t audio;
    oslo_input_t input;
    oslo_time_t time;
    bool running;

    struct GLFWwindow* window;
} oslo_t;

typedef struct oslo_baked_char_t
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
} oslo_baked_char_t;

typedef struct oslo_font_t
{
    oslo_baked_char_t glyphs[96];
    oslo_texture_id texture;
} oslo_font_t;
#pragma endregion

#pragma region OSLO_GFX
void oslo_gfx_begin();
void oslo_gfx_end();
void oslo_gfx_draw_quad(vec2 position, float rotation, vec2 size, vec4 color);
oslo_texture_id oslo_gfx_load_texture(const char* path);
void oslo_gfx_unload_texture(oslo_texture_id texture);
void oslo_gfx_draw_texture(vec2 position, float rotation, vec2 size, vec4 tint, oslo_texture_id texture);
void oslo_gfx_draw_texture_section(vec2 position, float rotation, vec2 size, vec4 tint, oslo_texture_id texture, oslo_rect_t rect, bool flip_horizontal);
void oslo_gfx_draw_textured_quad(vec4 quad[4], vec4 tint, oslo_texture_id texture, oslo_rect_t rect, bool flip_horizontal);
oslo_texture_id oslo_gfx_create_texture(void* data, uint32_t width, uint32_t height, uint32_t num_channels);
void oslo_gfx_text(const char* text, vec2 position, float size, vec4 color, oslo_font_t* font);
bool oslo_gfx_quad_batch_create(size_t max_quads, oslo_gfx_quad_batch_t* out_batch);
void oslo_gfx_quad_batch_destroy(oslo_gfx_quad_batch_t* batch);
void oslo_gfx_quad_batch_render(oslo_gfx_quad_batch_t* batch);
void oslo_gfx_quad_batch_update_content(oslo_gfx_quad_batch_t* batch);
void oslo_gfx_quad_batch_reset(oslo_gfx_quad_batch_t* batch);
bool oslo_gfx_quad_batch_is_full(oslo_gfx_quad_batch_t* batch);
void oslo_gfx_quad_batch_draw_quad(oslo_gfx_quad_batch_t* batch, draw_quad_desc_t* desc);
void oslo_gfx_quad_batch_draw_texture_section(oslo_gfx_quad_batch_t* batch, draw_texture_section_desc_t* desc);
void oslo_gfx_quad_batch_draw_textured_quad(oslo_gfx_quad_batch_t* batch, draw_textured_quad_desc_t* desc);
void oslo_gfx_quad_batch_draw_texture(oslo_gfx_quad_batch_t* batch, draw_texture_desc_t* desc);
#pragma endregion

#pragma region FILESYSTEM
char* oslo_read_file_contents(const char* file_path, const char* mode, size_t* sz);
int32_t oslo_file_size_in_bytes(const char* file_path);
#pragma endregion

#pragma region FONTS
// Fonts
bool oslo_load_font_from_file(const char* path, uint32_t point_size, oslo_font_t* out_font);
bool oslo_load_font_from_memory(void* memory, size_t len, uint32_t point_size, oslo_font_t* out_fount);
void oslo_unload_font(oslo_font_t* font);
#pragma endregion

#pragma region INPUT
// Input
b32 oslo_is_key_pressed(oslo_keycode code);
b32 oslo_was_key_pressed(oslo_keycode code);
b32 oslo_is_key_released(oslo_keycode code);
vec2 oslo_get_mouse_position();
vec2 oslo_get_mouse_delta();
b32 oslo_is_mouse_button_pressed(oslo_mouse_button_code code);
b32 oslo_was_mouse_button_pressed(oslo_mouse_button_code code);
b32 oslo_is_mouse_button_released(oslo_mouse_button_code code);

#pragma endregion

#pragma region WINDOW
void oslo_set_window_title(const char* title);
#pragma endregion

#pragma region HASH
oslo_inline 
uint32_t oslo_hash_uint32_t(uint32_t x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

#define oslo_hash_u32_ip(__X, __OUT)\
    do {\
        __OUT = ((__X >> 16) ^ __X) * 0x45d9f3b;\
        __OUT = ((__OUT >> 16) ^ __OUT) * 0x45d9f3b;\
        __OUT = (__OUT >> 16) ^ __OUT;\
    } while (0) 

oslo_inline 
uint32_t oslo_hash_u64(uint64_t x)
{
    x = (x ^ (x >> 31) ^ (x >> 62)) * UINT64_C(0x319642b2d24d8ec3);
    x = (x ^ (x >> 27) ^ (x >> 54)) * UINT64_C(0x96de1b173f119089);
    x = x ^ (x >> 30) ^ (x >> 60);
    return (uint32_t)x; 
}

// http://www.cse.yorku.ca/~oz/hash.html
// djb2 hash by dan bernstein
oslo_inline 
uint32_t oslo_hash_str(const char* str)
{
    uint32_t hash = 5381;
    s32 c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

oslo_inline 
uint64_t oslo_hash_str64(const char* str)
{
    uint32_t hash1 = 5381;
    uint32_t hash2 = 52711;
    uint32_t i = oslo_string_length(str);
    while(i--) 
    {
        char c = str[ i ];
        hash1 = (hash1 * 33) ^ c;
        hash2 = (hash2 * 33) ^ c;
    }

    return (hash1 >> 0) * 4096 + (hash2 >> 0);
}

oslo_inline
bool oslo_compare_bytes(void* b0, void* b1, size_t len)
{
    return 0 == memcmp(b0, b1, len);
}

// Hash generic bytes using (ripped directly from Sean Barret's stb_ds.h)
#define OSLO_SIZE_T_BITS  ((sizeof(size_t)) * 8)
#define OSLO_SIPHASH_C_ROUNDS 1
#define OSLO_SIPHASH_D_ROUNDS 1
#define OSLO_rotate_left(__V, __N)   (((__V) << (__N)) | ((__V) >> (oslo_SIZE_T_BITS - (__N))))
#define OSLO_rotate_right(__V, __N)  (((__V) >> (__N)) | ((__V) << (oslo_SIZE_T_BITS - (__N))))

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127) // conditional expression is constant, for do..while(0) and sizeof()==
#endif

oslo_inline 
size_t oslo_hash_siphash_bytes(void *p, size_t len, size_t seed)
{
  unsigned char *d = (unsigned char *) p;
  size_t i,j;
  size_t v0,v1,v2,v3, data;

  // hash that works on 32- or 64-bit registers without knowing which we have
  // (computes different results on 32-bit and 64-bit platform)
  // derived from siphash, but on 32-bit platforms very different as it uses 4 32-bit state not 4 64-bit
  v0 = ((((size_t) 0x736f6d65 << 16) << 16) + 0x70736575) ^  seed;
  v1 = ((((size_t) 0x646f7261 << 16) << 16) + 0x6e646f6d) ^ ~seed;
  v2 = ((((size_t) 0x6c796765 << 16) << 16) + 0x6e657261) ^  seed;
  v3 = ((((size_t) 0x74656462 << 16) << 16) + 0x79746573) ^ ~seed;

  #ifdef STBDS_TEST_SIPHASH_2_4
  // hardcoded with key material in the siphash test vectors
  v0 ^= 0x0706050403020100ull ^  seed;
  v1 ^= 0x0f0e0d0c0b0a0908ull ^ ~seed;
  v2 ^= 0x0706050403020100ull ^  seed;
  v3 ^= 0x0f0e0d0c0b0a0908ull ^ ~seed;
  #endif

  #define oslo_sipround() \
    do {                   \
      v0 += v1; v1 = oslo_rotate_left(v1, 13);  v1 ^= v0; v0 = oslo_rotate_left(v0,OSLO_SIZE_T_BITS/2); \
      v2 += v3; v3 = oslo_rotate_left(v3, 16);  v3 ^= v2;                                                 \
      v2 += v1; v1 = oslo_rotate_left(v1, 17);  v1 ^= v2; v2 = oslo_rotate_left(v2,OSLO_SIZE_T_BITS/2); \
      v0 += v3; v3 = oslo_rotate_left(v3, 21);  v3 ^= v0;                                                 \
    } while (0)

  for (i=0; i+sizeof(size_t) <= len; i += sizeof(size_t), d += sizeof(size_t)) {
    data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    data |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // discarded if size_t == 4

    v3 ^= data;
    for (j=0; j < OSLO_SIPHASH_C_ROUNDS; ++j)
      oslo_sipround();
    v0 ^= data;
  }
  data = len << (OSLO_SIZE_T_BITS-8);
  switch (len - i) {
    case 7: data |= ((size_t) d[6] << 24) << 24; // fall through
    case 6: data |= ((size_t) d[5] << 20) << 20; // fall through
    case 5: data |= ((size_t) d[4] << 16) << 16; // fall through
    case 4: data |= (d[3] << 24); // fall through
    case 3: data |= (d[2] << 16); // fall through
    case 2: data |= (d[1] << 8); // fall through
    case 1: data |= d[0]; // fall through
    case 0: break;
  }
  v3 ^= data;
  for (j=0; j < OSLO_SIPHASH_C_ROUNDS; ++j)
    oslo_sipround();
  v0 ^= data;
  v2 ^= 0xff;
  for (j=0; j < OSLO_SIPHASH_D_ROUNDS; ++j)
    oslo_sipround();

#if 0
  return v0^v1^v2^v3;
#else
  return v1^v2^v3; // slightly stronger since v0^v3 in above cancels out final round operation? I tweeted at the authors of SipHash about this but they didn't reply
#endif
}

oslo_inline
size_t oslo_hash_bytes(void *p, size_t len, size_t seed)
{
#if 0
  return oslo_hash_siphash_bytes(p,len,seed);
#else
  unsigned char *d = (unsigned char *) p;

  // Len == 4 (off for now, so to force 64 bit hash)
  if (len == 4) {
    unsigned int hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    hash ^= seed;
    hash *= 0xcc9e2d51;
    hash = (hash << 17) | (hash >> 15);
    hash *= 0x1b873593;
    hash ^= seed;
    hash = (hash << 19) | (hash >> 13);
    hash = hash*5 + 0xe6546b64;
    hash ^= hash >> 16;
    hash *= 0x85ebca6b;
    hash ^= seed;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= hash >> 16;
    return (((size_t) hash << 16 << 16) | hash) ^ seed;
  } else if (len == 8 && sizeof(size_t) == 8) {
    size_t hash = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
    hash |= (size_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16; // avoid warning if size_t == 4
    hash ^= seed;
    hash = (~hash) + (hash << 21);
    hash ^= oslo_rotate_right(hash,24);
    hash *= 265;
    hash ^= oslo_rotate_right(hash,14);
    hash ^= seed;
    hash *= 21;
    hash ^= oslo_rotate_right(hash,28);
    hash += (hash << 31);
    hash = (~hash) + (hash << 18);
    return hash;
  } else {
    return oslo_hash_siphash_bytes(p,len,seed);
  }
#endif
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif

#pragma endregion

#pragma region HASH_TABLE

#define OSLO_HASH_TABLE_HASH_SEED         0x31415296
#define OSLO_HASH_TABLE_INVALID_INDEX     UINT32_MAX

typedef enum oslo_hash_table_entry_state
{
    OSLO_HASH_TABLE_ENTRY_INACTIVE = 0x00,
    OSLO_HASH_TABLE_ENTRY_ACTIVE = 0x01
} oslo_hash_table_entry_state;

#define __oslo_hash_table_entry(__HMK, __HMV)\
    struct\
    {\
        __HMK key;\
        __HMV val;\
        oslo_hash_table_entry_state state;\
    }

#define oslo_hash_table(__HMK, __HMV)\
    struct {\
        __oslo_hash_table_entry(__HMK, __HMV)* data;\
        __HMK tmp_key;\
        __HMV tmp_val;\
        size_t stride;\
        size_t klpvl;\
        size_t tmp_idx;\
    }*

// Need a way to create a temporary key so I can take the address of it

#define oslo_hash_table_new(__K, __V)\
    NULL

void __oslo_hash_table_init_impl(void** ht, size_t sz);

#define oslo_hash_table_init(__HT, __K, __V)\
    do {\
        size_t entry_sz = sizeof(__K) + sizeof(__V) + sizeof(oslo_hash_table_entry_state);\
        size_t ht_sz = sizeof(__K) + sizeof(__V) + sizeof(void*) + 3 * sizeof(size_t);\
        __oslo_hash_table_init_impl((void**)&(__HT), ht_sz);\
        memset((__HT), 0, ht_sz);\
        oslo_dyn_array_reserve(__HT->data, 2);\
        __HT->data[0].state = OSLO_HASH_TABLE_ENTRY_INACTIVE;\
        __HT->data[1].state = OSLO_HASH_TABLE_ENTRY_INACTIVE;\
        uintptr_t d0 = (uintptr_t)&((__HT)->data[0]);\
        uintptr_t d1 = (uintptr_t)&((__HT)->data[1]);\
        ptrdiff_t diff = (d1 - d0);\
        ptrdiff_t klpvl = (uintptr_t)&(__HT->data[0].state) - (uintptr_t)(&__HT->data[0]);\
        (__HT)->stride = (size_t)(diff);\
        (__HT)->klpvl = (size_t)(klpvl);\
    } while (0)

#define oslo_hash_table_reserve(_HT, _KT, _VT, _CT)\
    do {\
        if ((_HT) == NULL) {\
            oslo_hash_table_init((_HT), _KT, _VT);\
        }\
        oslo_dyn_array_reserve((_HT)->data, _CT);\
    } while (0)

    // ((__HT) != NULL ? (__HT)->size : 0) // oslo_dyn_array_size((__HT)->data) : 0)
#define oslo_hash_table_size(__HT)\
    ((__HT) != NULL ? oslo_dyn_array_size((__HT)->data) : 0)

#define oslo_hash_table_capacity(__HT)\
    ((__HT) != NULL ? oslo_dyn_array_capacity((__HT)->data) : 0)

#define oslo_hash_table_load_factor(__HT)\
    (oslo_hash_table_capacity(__HT) ? (float)(oslo_hash_table_size(__HT)) / (float)(oslo_hash_table_capacity(__HT)) : 0.f)

#define oslo_hash_table_grow(__HT, __C)\
    ((__HT)->data = oslo_dyn_array_resize_impl((__HT)->data, sizeof(*((__HT)->data)), (__C)))

#define oslo_hash_table_empty(__HT)\
    ((__HT) != NULL ? oslo_dyn_array_size((__HT)->data) == 0 : true)

#define oslo_hash_table_clear(__HT)\
    do {\
        if ((__HT) != NULL) {\
            uint32_t capacity = oslo_dyn_array_capacity((__HT)->data);\
            for (uint32_t i = 0; i < capacity; ++i) {\
                (__HT)->data[i].state = oslo_HASH_TABLE_ENTRY_INACTIVE;\
            }\
            /*memset((__HT)->data, 0, oslo_dyn_array_capacity((__HT)->data) * );*/\
            oslo_dyn_array_clear((__HT)->data);\
        }\
    } while (0)

#define oslo_hash_table_free(__HT)\
    do {\
        if ((__HT) != NULL) {\
            oslo_dyn_array_free((__HT)->data);\
            (__HT)->data = NULL;\
            oslo_free(__HT);\
            (__HT) = NULL;\
        }\
    } while (0)

// Find available slot to insert k/v pair into
#define oslo_hash_table_insert(__HT, __HMK, __HMV)\
    do {\
        /* Check for null hash table, init if necessary */\
        if ((__HT) == NULL) {\
            oslo_hash_table_init((__HT), (__HMK), (__HMV));\
        }\
    \
        /* Grow table if necessary */\
        uint32_t __CAP = oslo_hash_table_capacity(__HT);\
        float __LF = oslo_hash_table_load_factor(__HT);\
        if (__LF >= 0.5f || !__CAP)\
        {\
            uint32_t NEW_CAP = __CAP ? __CAP * 2 : 2;\
            size_t ENTRY_SZ = sizeof((__HT)->tmp_key) + sizeof((__HT)->tmp_val) + sizeof(oslo_hash_table_entry_state);\
            oslo_dyn_array_reserve((__HT)->data, NEW_CAP);\
            /**((void **)&(__HT->data)) = oslo_dyn_array_resize_impl(__HT->data, ENTRY_SZ, NEW_CAP);*/\
            /* Iterate through data and set state to null, from __CAP -> __CAP * 2 */\
            /* Memset here instead */\
            for (uint32_t __I = __CAP; __I < NEW_CAP; ++__I) {\
                (__HT)->data[__I].state = OSLO_HASH_TABLE_ENTRY_INACTIVE;\
            }\
            __CAP = oslo_hash_table_capacity(__HT);\
        }\
    \
        /* Get hash of key */\
        (__HT)->tmp_key = (__HMK);\
        size_t __HSH = oslo_hash_bytes((void*)&((__HT)->tmp_key), sizeof((__HT)->tmp_key), OSLO_HASH_TABLE_HASH_SEED);\
        size_t __HSH_IDX = __HSH % __CAP;\
        (__HT)->tmp_key = (__HT)->data[__HSH_IDX].key;\
        uint32_t c = 0;\
    \
        /* Find valid idx and place data */\
        while (\
            c < __CAP\
            && __HSH != oslo_hash_bytes((void*)&(__HT)->tmp_key, sizeof((__HT)->tmp_key), OSLO_HASH_TABLE_HASH_SEED)\
            && (__HT)->data[__HSH_IDX].state == OSLO_HASH_TABLE_ENTRY_ACTIVE)\
        {\
            __HSH_IDX = ((__HSH_IDX + 1) % __CAP);\
            (__HT)->tmp_key = (__HT)->data[__HSH_IDX].key;\
            ++c;\
        }\
        (__HT)->data[__HSH_IDX].key = (__HMK);\
        (__HT)->data[__HSH_IDX].val = (__HMV);\
        (__HT)->data[__HSH_IDX].state = OSLO_HASH_TABLE_ENTRY_ACTIVE;\
        oslo_dyn_array_head((__HT)->data)->size++;\
    } while (0)

// Need size difference between two entries
// Need size of key + val

oslo_inline
uint32_t oslo_hash_table_get_key_index_func(void** data, void* key, size_t key_len, size_t val_len, size_t stride, size_t klpvl)
{
    if (!data || !key) return OSLO_HASH_TABLE_INVALID_INDEX;

    // Need a better way to handle this. Can't do it like this anymore.
    // Need to fix this. Seriously messing me up.
    uint32_t capacity = oslo_dyn_array_capacity(*data);
    size_t idx = (size_t)OSLO_HASH_TABLE_INVALID_INDEX;
    size_t hash = (size_t)oslo_hash_bytes(key, key_len, OSLO_HASH_TABLE_HASH_SEED);
    size_t hash_idx = (hash % capacity);

    // Iterate through data 
    for (size_t i = hash_idx, c = 0; c < capacity; ++c, i = ((i + 1) % capacity))
    {
        size_t offset = (i * stride);
        void* k = ((char*)(*data) + (offset));  
        size_t kh = oslo_hash_bytes(k, key_len, OSLO_HASH_TABLE_HASH_SEED);
        bool comp = oslo_compare_bytes(k, key, key_len);
        oslo_hash_table_entry_state state = *(oslo_hash_table_entry_state*)((char*)(*data) + offset + (klpvl)); 
        if (comp && hash == kh && state == OSLO_HASH_TABLE_ENTRY_ACTIVE) {
            idx = i;
            break;
        }
    }
    return (uint32_t)idx;
}

// Get key at index
#define oslo_hash_table_getk(__HT, __I)\
    (((__HT))->data[(__I)].key)

// Get val at index
#define oslo_hash_table_geti(__HT, __I)\
    ((__HT)->data[(__I)].val)

// Could search for the index in the macro instead now. Does this help me?
#define oslo_hash_table_get(__HT, __HTK)\
    ((__HT)->tmp_key = (__HTK),\
        (oslo_hash_table_geti((__HT),\
            oslo_hash_table_get_key_index_func((void**)&(__HT)->data, (void*)&((__HT)->tmp_key),\
                sizeof((__HT)->tmp_key), sizeof((__HT)->tmp_val), (__HT)->stride, (__HT)->klpvl)))) 

#define oslo_hash_table_getp(__HT, __HTK)\
    (\
        (__HT)->tmp_key = (__HTK),\
        ((__HT)->tmp_idx = (uint32_t)oslo_hash_table_get_key_index_func((void**)&(__HT->data), (void*)&(__HT->tmp_key), sizeof(__HT->tmp_key),\
            sizeof(__HT->tmp_val), __HT->stride, __HT->klpvl)),\
        ((__HT)->tmp_idx != OSLO_HASH_TABLE_INVALID_INDEX ? &oslo_hash_table_geti((__HT), (__HT)->tmp_idx) : NULL)\
    )

#define oslo_hash_table_key_exists(__HT, __HTK)\
    ((__HT)->tmp_key = (__HTK),\
        (oslo_hash_table_get_key_index_func((void**)&(__HT->data), (void*)&(__HT->tmp_key), sizeof(__HT->tmp_key),\
            sizeof(__HT->tmp_val), __HT->stride, __HT->klpvl) != OSLO_HASH_TABLE_INVALID_INDEX))

// uint32_t oslo_hash_table_get_key_index_func(void** data, void* key, size_t key_len, size_t val_len, size_t stride, size_t klpvl)

#define oslo_hash_table_exists(__HT, __HTK)\
		(__HT && oslo_hash_table_key_exists((__HT), (__HTK)))

#define oslo_hash_table_erase(__HT, __HTK)\
    do {\
        /* Get idx for key */\
        (__HT)->tmp_key = (__HTK);\
        uint32_t __IDX = oslo_hash_table_get_key_index_func((void**)&(__HT)->data, (void*)&((__HT)->tmp_key), sizeof((__HT)->tmp_key), sizeof((__HT)->tmp_val), (__HT)->stride, (__HT)->klpvl);\
        if (__IDX != OSLO_HASH_TABLE_INVALID_INDEX) {\
            (__HT)->data[__IDX].state = OSLO_HASH_TABLE_ENTRY_INACTIVE;\
            if (oslo_dyn_array_head((__HT)->data)->size) oslo_dyn_array_head((__HT)->data)->size--;\
        }\
    } while (0)

/*===== Hash Table Iterator ====*/

typedef uint32_t oslo_hash_table_iter;

oslo_inline
uint32_t __oslo_find_first_valid_iterator(void* data, size_t key_len, size_t val_len, uint32_t idx, size_t stride, size_t klpvl)
{
    uint32_t it = (uint32_t)idx;
    for (; it < (uint32_t)oslo_dyn_array_capacity(data); ++it)
    {
        size_t offset = (it * stride);
        oslo_hash_table_entry_state state = *(oslo_hash_table_entry_state*)((uint8_t*)data + offset + (klpvl));
        if (state == OSLO_HASH_TABLE_ENTRY_ACTIVE)
        {
            break;
        }
    }
    return it;
}

/* Find first valid iterator idx */
#define oslo_hash_table_iter_new(__HT)\
    (__HT ? __oslo_find_first_valid_iterator((__HT)->data, sizeof((__HT)->tmp_key), sizeof((__HT)->tmp_val), 0, (__HT)->stride, (__HT)->klpvl) : 0)

#define oslo_hash_table_iter_valid(__HT, __IT)\
    ((__IT) < oslo_hash_table_capacity((__HT)))

// Have to be able to do this for hash table...
oslo_inline
void __oslo_hash_table_iter_advance_func(void** data, size_t key_len, size_t val_len, uint32_t* it, size_t stride, size_t klpvl)
{
    (*it)++;
    for (; *it < (uint32_t)oslo_dyn_array_capacity(*data); ++*it)
    {
        size_t offset = (size_t)(*it * stride);
        oslo_hash_table_entry_state state = *(oslo_hash_table_entry_state*)((uint8_t*)*data + offset + (klpvl));
        if (state == OSLO_HASH_TABLE_ENTRY_ACTIVE)
        {
            break;
        }
    }
}

#define oslo_hash_table_find_valid_iter(__HT, __IT)\
    ((__IT) = __oslo_find_first_valid_iterator((void**)&(__HT)->data, sizeof((__HT)->tmp_key), sizeof((__HT)->tmp_val), (__IT), (__HT)->stride, (__HT)->klpvl))

#define oslo_hash_table_iter_advance(__HT, __IT)\
    (__oslo_hash_table_iter_advance_func((void**)&(__HT)->data, sizeof((__HT)->tmp_key), sizeof((__HT)->tmp_val), &(__IT), (__HT)->stride, (__HT)->klpvl))

#define oslo_hash_table_iter_get(__HT, __IT)\
    oslo_hash_table_geti(__HT, __IT)

#define oslo_hash_table_iter_getp(__HT, __IT)\
    (&(oslo_hash_table_geti(__HT, __IT)))

#define oslo_hash_table_iter_getk(__HT, __IT)\
    (oslo_hash_table_getk(__HT, __IT))

#define oslo_hash_table_iter_getkp(__HT, __IT)\
    (&(oslo_hash_table_getk(__HT, __IT)))

#pragma endregion

#ifdef OSLO_IMPL
#pragma region EXTERNAL_IMPL

// Created as a global variable to avoid sending the instance around. It will have conflicts if working with dll.
static oslo_t* instance = NULL;

#define GLAD_IMPL
#include "external/glad/glad_impl.h"

#define GLFW_IMPL
#include "external/glfw/glfw_impl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "external/stb/stb_truetype.h"

#include "external/stb/stb_vorbis.c"
#include "external/dr_libs/dr_wav.h"
#include "external/dr_libs/dr_mp3.h"

#define MINIAUDIO_IMPLEMENTATION
#include "external/miniaudio/miniaudio.h"

#pragma endregion

#pragma region INPUT_CALLBACKS
// Input callbacks
void __glfw_key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods);
void __glfw_char_callback(GLFWwindow* window, uint32_t codepoint);
void __glfw_mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods);
void __glfw_mouse_cursor_position_callback(GLFWwindow* window, f64 x, f64 y);
void __glfw_mouse_scroll_wheel_callback(GLFWwindow* window, f64 xoffset, f64 yoffset);
void __glfw_mouse_cursor_enter_callback(GLFWwindow* window, s32 entered);
void __glfw_frame_buffer_size_callback(GLFWwindow* window, s32 width, s32 height);
void __glfw_drop_callback(GLFWwindow* window);
#pragma endregion

#pragma region GFX
// GFX
void oslo_gfx_init(oslo_t* oslo);
void oslo_gfx_shutdown(oslo_t* oslo);
void oslo_gfx_begin_batch(oslo_t* oslo);
void oslo_gfx_next_batch(oslo_t* oslo);
void unload_texture(oslo_gfx_texture_t* texture);
#pragma endregion

#pragma region AUDIO
void oslo_audio_init();
void oslo_audio_shutdown();
#pragma endregion

#pragma region INPUT
// Input
void oslo_input_init(oslo_t* oslo);
void oslo_update_input(oslo_input_t* input);
#pragma endregion

#pragma region TIME
// Time
float oslo_get_delta_time();
float oslo_get_elapsed_time();
#pragma endregion

#pragma region SHADERS
const char* v_src_2D = 
"#version 330 core\n"
"layout (location = 0) in vec4 aPos;\n"
"layout (location = 1) in vec4 aColor;\n"
"layout (location = 2) in vec2 aUV;\n"
"layout (location = 3) in float aTextureIndex;\n"
"uniform mat4 u_projection;\n"
"out vec4 color;\n"
"out vec2 uv;\n"
"out float tex_index;\n"
"void main()\n"
"{\n"
"   color = aColor;\n"
"   tex_index = aTextureIndex;\n"
"   uv = aUV;\n"
"   gl_Position = u_projection * aPos;\n"
"}\0";

const char* f_src_2D =
"#version 330 core\n"
"in vec4 color;\n"
"in float tex_index;\n"
"in vec2 uv;\n"
"out vec4 FragColor;\n"
"uniform sampler2D u_textures[32];\n"
"void main()\n"
"{\n"
"	vec4 final_color = color;\n"
"	switch (int(tex_index))							\n"
"	{														\n"
"	case  0: final_color *= texture(u_textures[0], uv); break;	\n"
"	case  1: final_color *= texture(u_textures[1], uv); break;	\n"
"	case  2: final_color *= texture(u_textures[2], uv); break;	\n"
"	case  3: final_color *= texture(u_textures[3], uv); break;	\n"
"	case  4: final_color *= texture(u_textures[4], uv); break;	\n"
"	case  5: final_color *= texture(u_textures[5], uv); break;	\n"
"	case  6: final_color *= texture(u_textures[6], uv); break;	\n"
"	case  7: final_color *= texture(u_textures[7], uv); break;	\n"
"	case  8: final_color *= texture(u_textures[8], uv); break;	\n"
"	case  9: final_color *= texture(u_textures[9], uv); break;	\n"
"	case 10: final_color *= texture(u_textures[10], uv); break;	\n"
"	case 11: final_color *= texture(u_textures[11], uv); break;	\n"
"	case 12: final_color *= texture(u_textures[12], uv); break;	\n"
"	case 13: final_color *= texture(u_textures[13], uv); break;	\n"
"	case 14: final_color *= texture(u_textures[14], uv); break;	\n"
"	case 15: final_color *= texture(u_textures[15], uv); break;	\n"
"	case 16: final_color *= texture(u_textures[16], uv); break;	\n"
"	case 17: final_color *= texture(u_textures[17], uv); break;	\n"
"	case 18: final_color *= texture(u_textures[18], uv); break;	\n"
"	case 19: final_color *= texture(u_textures[19], uv); break;	\n"
"	case 20: final_color *= texture(u_textures[20], uv); break;	\n"
"	case 21: final_color *= texture(u_textures[21], uv); break;	\n"
"	case 22: final_color *= texture(u_textures[22], uv); break;	\n"
"	case 23: final_color *= texture(u_textures[23], uv); break;	\n"
"	case 24: final_color *= texture(u_textures[24], uv); break;	\n"
"	case 25: final_color *= texture(u_textures[25], uv); break;	\n"
"	case 26: final_color *= texture(u_textures[26], uv); break;	\n"
"	case 27: final_color *= texture(u_textures[27], uv); break;	\n"
"	case 28: final_color *= texture(u_textures[28], uv); break;	\n"
"	case 29: final_color *= texture(u_textures[29], uv); break;	\n"
"	case 30: final_color *= texture(u_textures[30], uv); break;	\n"
"	case 31: final_color *= texture(u_textures[31], uv); break;	\n"
"	}\n"
"   FragColor = final_color;\n"
"}\n\0";
#pragma endregion

#pragma region GFX_DEFINES

#define MAX_QUADS 20000
#define MAX_VERTICES MAX_QUADS * 4
#define MAX_INDICES MAX_QUADS * 6

static vec2 _uvs[] =
{
	{ 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 1.0f, 1.0f },
	{ 0.0f, 1.0f }
};

static vec4 quad_positions[] =
{
	{ -1.0f, -1.0f, 0.0f, 1.0f }, // TL
	{  1.0f, -1.0f, 0.0f, 1.0f }, // TR
	{  1.0f,  1.0f, 0.0f, 1.0f }, // BR
	{ -1.0f,  1.0f, 0.0f, 1.0f } // BL
};
#pragma endregion

#pragma region OSLO_MAIN

oslo_desc_t oslo_main();

#pragma endregion

#pragma region UTILITY_FUNCS

#define oslo_min(A, B) ((A) < (B) ? (A) : (B))
#define oslo_clamp(V, MIN, MAX) ((V) > (MAX) ? (MAX) : (V) < (MIN) ? (MIN) : (V))

void unload_texture(oslo_gfx_texture_t* texture)
{
    glDeleteTextures(1, &texture->id);
}

vec2 get_framebuffer_size(oslo_t* oslo)
{
    uint32_t w = 0;
    uint32_t h = 0;
    glfwGetFramebufferSize(oslo->window, &w, &h);

    return v2(w, h);
}

void notify_error(oslo_t* oslo, oslo_error_code code, const char* msg)
{
    if (oslo->desc.on_oslo_error != NULL)
    {
        oslo->desc.on_oslo_error(code, msg);
    }
}

float oslo_get_elapsed_time()
{
    return glfwGetTime();
}

void oslo_platform_sleep(float ms)
{
    #if (defined OSLO_PLATFORM_WIN)

            timeBeginPeriod(1);
            Sleep((uint64_t)ms);
            timeEndPeriod(1);

    #elif (defined OSLO_PLATFORM_APPLE)

            usleep(ms * 1000.f); // unistd.h
    #else
	    if (ms < 0.f) {
		return;
	    }

	    struct timespec ts = default_val();
	    int32_t res = 0;
	    ts.tv_sec = ms / 1000.f;
	    ts.tv_nsec = ((uint64_t)ms % 1000) * 1000000;
	    do {
		res = nanosleep(&ts, &ts);
	    } while (res && errno == EINTR);

            // usleep(ms * 1000.f); // unistd.h
    #endif
}

bool oslo_platform_file_exists(const char* file_path)
{
    const char* path = file_path;

    #ifdef OSLO_PLATFORM_ANDROID
        const char* internal_data_path = oslo_app()->android.internal_data_path;
        oslo_snprintfc(tmp_path, 1024, "%s/%s", internal_data_path, file_path);
        path = tmp_path;
    #endif

    FILE* fp = fopen(path, "r");
    if (fp) {
        fclose(fp);
        return true;
    }
    return false;
}

oslo_inline uint32_t 
string_length(const char* txt)
{
    uint32_t sz = 0;
    while (txt != NULL && txt[ sz ] != '\0') 
    {
        sz++;
    }
    return sz;
}

oslo_inline void
oslo_str_to_lower
(
    const char* src,
    char* buffer,
    size_t buffer_sz
)
{
    size_t src_sz = string_length(src);
    size_t len = oslo_min(src_sz, buffer_sz);

    for (uint32_t i = 0; i < len; ++i) {
        buffer[i] = tolower(src[i]);
    }
}

oslo_inline bool 
oslo_string_compare_equal
(
    const char*     txt, 
    const char*     cmp 
)
{
    // Grab sizes of both strings
    uint32_t a_sz = string_length(txt);
    uint32_t b_sz = string_length(cmp);

    // Return false if sizes do not match
    if (a_sz != b_sz) 
    {
        return false;
    }

    for(uint32_t i = 0; i < a_sz; ++i) 
    {
        if (*txt++ != *cmp++)
        {
            return false;
        }
    };

    return true;
}

oslo_inline 
void oslo_get_file_extension
(
    char* buffer,
    uint32_t buffer_size,
    const char* file_path 
)
{
    uint32_t str_len = string_length(file_path);
    const char* at = (file_path + str_len - 1);
    while (*at != '.' && at != file_path)
    {
        at--;
    }

    if (*at == '.')
    {
        at++;
        uint32_t i = 0; 
        while (*at)
        {
            char c = *at;
            buffer[i++] = *at++;
        }
        buffer[i] = '\0';
    }
}

#pragma endregion

#pragma region MAIN
int main(int argc, char *argv[])
{
    if (!glfwInit())
        return -1;

    oslo_desc_t desc = oslo_main();

    instance = malloc(sizeof(oslo_t));
    memset(instance, 0, sizeof(oslo_t));
    
    instance->running = false;
    instance->desc = desc;

#if (defined PLATFORM_APPLE)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#else
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, platform->settings.video.graphics.opengl.major_version);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, platform->settings.video.graphics.opengl.minor_version);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (desc.debug_graphics)
    {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    }
#endif

    
    instance->window = glfwCreateWindow(desc.window_width, desc.window_height, desc.window_title, NULL, NULL);
    if (!instance->window)
    {
        notify_error(instance, OSLO_PLATFORM_INIT_ERROR, "Failed create window!");
        free(instance);
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(instance->window);
    glfwSwapInterval(1);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        notify_error(instance, OSLO_GFX_INIT_ERROR, "Failed to initialize glad");
        glfwDestroyWindow(instance->window);
        free(instance);
        glfwTerminate();

        return -1;
    }
    
    oslo_input_init(instance);
    oslo_gfx_init(instance);
    oslo_audio_init();

    if (desc.init != NULL)
	{
		desc.init(desc.user_data);
	}

    instance->running = true;

    instance->time.previous = glfwGetTime();
    double target_fps = 1.0 / desc.max_fps;

    while(instance->running)
    {
        oslo_time_t* time = &instance->time;

        double frame_start = glfwGetTime();

        float current_time = glfwGetTime();
        time->delta = current_time - time->previous;

        oslo_update_input(&instance->input);
         /* Poll for and process events */
        glfwPollEvents();

         // Render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        if (desc.update != NULL)
		{
			desc.update(desc.user_data);
		}

        /* Swap front and back buffers */
        glfwSwapBuffers(instance->window);

        double frame_time = glfwGetTime() - frame_start;

        if (target_fps > frame_time)
        {
            oslo_platform_sleep((target_fps - frame_time) * 1000);
            time->previous = glfwGetTime();
        }

        time->previous = current_time;
        instance->running = !glfwWindowShouldClose(instance->window);
    }

    if (desc.shutdown != NULL)
	{
		desc.shutdown(desc.user_data);
	}

    oslo_audio_shutdown();
    oslo_gfx_shutdown(instance);
    glfwDestroyWindow(instance->window);
    free(instance);
    glfwTerminate();

    return 0;
}

#pragma endregion

#pragma region GFX
/*========================
// GFX
========================*/

void load_shader(oslo_t* oslo)
{
    oslo_gfx_t* gfx = &oslo->gfx;
    // Create and compile shader
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];
    
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &v_src_2D, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        notify_error(oslo, OSLO_SHADER_COMPILE_ERROR, infoLog);
    };

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &f_src_2D, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        notify_error(oslo, OSLO_SHADER_COMPILE_ERROR, infoLog);
    };

    // shader Program
    gfx->shader = glCreateProgram();
    glAttachShader(gfx->shader, vertex);
    glAttachShader(gfx->shader, fragment);
    glLinkProgram(gfx->shader);
    // print linking errors if any
    glGetProgramiv(gfx->shader, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(gfx->shader, 512, NULL, infoLog);
        notify_error(oslo, OSLO_SHADER_LINKER_ERROR, infoLog);
    }
    
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}  

void setup_projection(oslo_t* oslo)
{
    vec2 fbs = get_framebuffer_size(oslo);

    oslo_gfx_t* gfx = &oslo->gfx;

    // Setup projection based on window size
    gfx->projection = mat4_ortho(0.0f, fbs.x, fbs.y, 0.0f, -1.0f, 1.0f);  

    // Upload projection
    glUseProgram(gfx->shader);
    glUniformMatrix4fv(glGetUniformLocation(gfx->shader, "u_projection"), 1, GL_FALSE, &gfx->projection.elements[0]);
    glUseProgram(0);
}

bool oslo_gfx_quad_batch_create(size_t max_quads, oslo_gfx_quad_batch_t* out_batch)
{
    size_t size = (max_quads * 4) * sizeof(oslo_gfx_vertex_t);

    out_batch->vertices = malloc(size);
    memset(out_batch->vertices, 0, size);

    u32 max_indices = max_quads * 6;
    out_batch->max_indices = max_indices;
    size_t indices_size = max_indices * sizeof(u32);
    u32* indices = malloc(indices_size);
	memset(indices, 0, indices_size);

	u32 offset = 0;
	for (u32 i = 0; i < max_indices; i += 6)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;

		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

    out_batch->index_count = 0;

    glGenVertexArrays(1, &out_batch->vao);
    glGenBuffers(1, &out_batch->vbo);
    glGenBuffers(1, &out_batch->ibo);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(out_batch->vao);

    glBindBuffer(GL_ARRAY_BUFFER, out_batch->vbo);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICES * sizeof(oslo_gfx_vertex_t), out_batch->vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, out_batch->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAX_INDICES * sizeof(u32), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(oslo_gfx_vertex_t), (void*)offsetof(oslo_gfx_vertex_t, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(oslo_gfx_vertex_t), (void*)offsetof(oslo_gfx_vertex_t, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(oslo_gfx_vertex_t), (void*)offsetof(oslo_gfx_vertex_t, uv));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(oslo_gfx_vertex_t), (void*)offsetof(oslo_gfx_vertex_t, tex_index));
    glEnableVertexAttribArray(3);

    out_batch->bound_textures[0] = instance->gfx.white_texture;
    out_batch->texture_index = 1;

    return true;
}

void oslo_gfx_quad_batch_destroy(oslo_gfx_quad_batch_t* batch)
{
    free(batch->vertices);
    glDeleteVertexArrays(1, &batch->vao);
    glDeleteBuffers(1, &batch->vbo);
    glDeleteBuffers(1, &batch->ibo);
}

void oslo_gfx_quad_batch_render(oslo_gfx_quad_batch_t* batch)
{
    if (batch->index_count > 0)
    {
        glUseProgram(instance->gfx.shader);

        // Upload textures
        for (uint32_t i = 0; i < MAX_BATCH_TEXTURES; ++i)
        {
            oslo_gfx_texture_t* texture = oslo_slot_array_getp(instance->gfx.textures, batch->bound_textures[i]);
            int texture_id = 0;
            if (texture != NULL)
            {
                texture_id = texture->id;
            }

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture_id);

            char uniform_name[50];
            sprintf(uniform_name, "u_textures[%i]", i);
            glUniform1i(glGetUniformLocation(instance->gfx.shader, uniform_name), i);
        }

        // draw mesh
        glDrawElements(GL_TRIANGLES, batch->index_count, GL_UNSIGNED_INT, NULL);
    }
}

void oslo_gfx_quad_batch_update_content(oslo_gfx_quad_batch_t* batch)
{
    glBindVertexArray(batch->vao);
    // Update vbo data
    uint32_t data_size = (uint32_t)((uint8_t*)batch->vert_ptr - (uint8_t*)batch->vertices);
    glBufferSubData(GL_ARRAY_BUFFER, 0, data_size, batch->vertices);
}

void oslo_gfx_quad_batch_reset(oslo_gfx_quad_batch_t* batch)
{
    batch->index_count = 0;
	batch->vert_ptr = batch->vertices;
    batch->texture_index = 1; // 0 is for white texture
}

void oslo_gfx_quad_batch_draw_texture_section(oslo_gfx_quad_batch_t* batch, draw_texture_section_desc_t* desc)
{
    oslo_gfx_t* gfx = &instance->gfx;
    oslo_gfx_texture_t* p_texture = oslo_slot_array_getp(gfx->textures, desc->texture);
    uint32_t width = p_texture->width;
    uint32_t height = p_texture->height;

    float sprite_width = (float)(desc->section.max.x - desc->section.min.x) / width;
	float sprite_height = (float)(desc->section.max.y - desc->section.min.y) / height;
	float sprite_pos_x = (float)desc->section.min.x / width;
	float sprite_pos_y = (float)desc->section.min.y / height;

    vec2 sprite_uvs[4] = { 0 };

    if (desc->flip_horizontal)
	{
		sprite_uvs[0] = v2(sprite_pos_x + sprite_width, sprite_pos_y);
		sprite_uvs[1] = v2(sprite_pos_x, sprite_pos_y);
		sprite_uvs[2] = v2(sprite_pos_x, sprite_pos_y + sprite_height);
		sprite_uvs[3] = v2(sprite_pos_x + sprite_width, sprite_pos_y + sprite_height);
	}
	else
	{
		sprite_uvs[0] = v2(sprite_pos_x, sprite_pos_y);
		sprite_uvs[1] = v2(sprite_pos_x + sprite_width, sprite_pos_y);
		sprite_uvs[2] = v2(sprite_pos_x + sprite_width, sprite_pos_y + sprite_height);
		sprite_uvs[3] = v2(sprite_pos_x, sprite_pos_y + sprite_height);
	}

    if (oslo_gfx_quad_batch_is_full(batch))
        return;//oslo_gfx_next_batch(instance);

    mat4 model = mat4_translate(desc->position.x, desc->position.y, 0);
    model = mat4_mul(model, mat4_translatev(v3(0.5f * desc->size.x, 0.5f * desc->size.y, 0)));
	model = mat4_mul(model, mat4_rotate(desc->rotation, 0, 0, 1));
	model = mat4_mul(model, mat4_translatev(v3(-0.5f * desc->size.x, -0.5f * desc->size.y, 0)));
	model = mat4_mul(model, mat4_scale(desc->size.x, desc->size.y, 1.0f));

    float texture_index = 0.0f;
	for (uint32_t i = 1; i < batch->texture_index; ++i)
	{
		if (batch->bound_textures[i] == desc->texture)
		{
			texture_index = (float)i;
			break;
		}
	}

    if (texture_index == 0)
	{
		texture_index = (float)batch->texture_index;
		batch->bound_textures[(int)batch->texture_index] = desc->texture;
		batch->texture_index++;
	}

    for (size_t i = 0; i < 4; ++i)
	{
		batch->vert_ptr->position = mat4_mul_vec4(model, quad_positions[i]);
		batch->vert_ptr->color = v4(desc->color.x, desc->color.y, desc->color.z, desc->color.w);
		batch->vert_ptr->uv = sprite_uvs[i];
        batch->vert_ptr->tex_index = texture_index; // 0 means white texture
		batch->vert_ptr++;
	}

    batch->index_count += 6;
}

void oslo_gfx_quad_batch_draw_textured_quad(oslo_gfx_quad_batch_t* batch, draw_textured_quad_desc_t* desc)
{
    oslo_gfx_t* gfx = &instance->gfx;
    oslo_gfx_texture_t* p_texture = oslo_slot_array_getp(gfx->textures, desc->texture);
    uint32_t width = p_texture->width;
    uint32_t height = p_texture->height;

    float sprite_width = (float)(desc->section.max.x - desc->section.min.x) / width;
	float sprite_height = (float)(desc->section.max.y - desc->section.min.y) / height;
	float sprite_pos_x = (float)desc->section.min.x / width;
	float sprite_pos_y = (float)desc->section.min.y / height;

    vec2 sprite_uvs[4] = { 0 };

    if (desc->flip_horizontal)
	{
		sprite_uvs[0] = v2(sprite_pos_x + sprite_width, sprite_pos_y);
		sprite_uvs[1] = v2(sprite_pos_x, sprite_pos_y);
		sprite_uvs[2] = v2(sprite_pos_x, sprite_pos_y + sprite_height);
		sprite_uvs[3] = v2(sprite_pos_x + sprite_width, sprite_pos_y + sprite_height);
	}
	else
	{
		sprite_uvs[0] = v2(sprite_pos_x, sprite_pos_y);
		sprite_uvs[1] = v2(sprite_pos_x + sprite_width, sprite_pos_y);
		sprite_uvs[2] = v2(sprite_pos_x + sprite_width, sprite_pos_y + sprite_height);
		sprite_uvs[3] = v2(sprite_pos_x, sprite_pos_y + sprite_height);
	}

    if (oslo_gfx_quad_batch_is_full(batch))
        return;//oslo_gfx_next_batch(instance);

    float texture_index = 0.0f;
	for (uint32_t i = 1; i < batch->texture_index; ++i)
	{
		if (batch->bound_textures[i] == desc->texture)
		{
			texture_index = (float)i;
			break;
		}
	}

    if (texture_index == 0)
	{
		texture_index = (float)batch->texture_index;
		batch->bound_textures[(int)batch->texture_index] = desc->texture;
		batch->texture_index++;
	}

    for (size_t i = 0; i < 4; ++i)
	{
		batch->vert_ptr->position = desc->quad[i];
		batch->vert_ptr->color = v4(desc->color.x, desc->color.y, desc->color.z, desc->color.w);
		batch->vert_ptr->uv = sprite_uvs[i];
        batch->vert_ptr->tex_index = texture_index; // 0 means white texture
		batch->vert_ptr++;
	}

    batch->index_count += 6;
}

void oslo_gfx_quad_batch_draw_quad(oslo_gfx_quad_batch_t* batch, draw_quad_desc_t* desc)
{
    oslo_gfx_t* gfx = &instance->gfx;

    if (oslo_gfx_quad_batch_is_full(batch))
        return;

    mat4 model = mat4_translate(desc->position.x, desc->position.y, 0);
    model = mat4_mul(model, mat4_translatev(v3(0.5f * desc->size.x, 0.5f * desc->size.y, 0)));
	model = mat4_mul(model, mat4_rotate(desc->rotation, 0, 0, 1));
	model = mat4_mul(model, mat4_translatev(v3(-0.5f * desc->size.x, -0.5f * desc->size.y, 0)));
	model = mat4_mul(model, mat4_scale(desc->size.x, desc->size.y, 1.0f));

    for (size_t i = 0; i < 4; ++i)
	{
		batch->vert_ptr->position = mat4_mul_vec4(model, quad_positions[i]);
		batch->vert_ptr->color = v4(desc->color.x, desc->color.y, desc->color.z, desc->color.w);
		batch->vert_ptr->uv = _uvs[i];
        batch->vert_ptr->tex_index = 0; // 0 means white texture
		batch->vert_ptr++;
	}

    batch->index_count += 6;
}

void oslo_gfx_quad_batch_draw_texture(oslo_gfx_quad_batch_t* batch, draw_texture_desc_t* desc)
{
    oslo_gfx_t* gfx = &instance->gfx;

    if (oslo_gfx_quad_batch_is_full(batch))
        return;//oslo_gfx_next_batch(instance);

    mat4 model = mat4_translate(desc->position.x, desc->position.y, 0);
    model = mat4_mul(model, mat4_translatev(v3(0.5f * desc->size.x, 0.5f * desc->size.y, 0)));
	model = mat4_mul(model, mat4_rotate(desc->rotation, 0, 0, 1));
	model = mat4_mul(model, mat4_translatev(v3(-0.5f * desc->size.x, -0.5f * desc->size.y, 0)));
	model = mat4_mul(model, mat4_scale(desc->size.x, desc->size.y, 1.0f));

    float texture_index = 0.0f;
	for (uint32_t i = 1; i < batch->texture_index; ++i)
	{
		if (batch->bound_textures[i] == desc->texture)
		{
			texture_index = (float)i;
			break;
		}
	}

    if (texture_index == 0)
	{
		texture_index = (float)batch->texture_index;
		batch->bound_textures[(int)batch->texture_index] = desc->texture;
		batch->texture_index++;
	}

    for (size_t i = 0; i < 4; ++i)
	{
		batch->vert_ptr->position = mat4_mul_vec4(model, quad_positions[i]);
		batch->vert_ptr->color = v4(desc->color.x, desc->color.y, desc->color.z, desc->color.w);
		batch->vert_ptr->uv = _uvs[i];
        batch->vert_ptr->tex_index = texture_index; // 0 means white texture
		batch->vert_ptr++;
	}

    batch->index_count += 6;
}

bool oslo_gfx_quad_batch_is_full(oslo_gfx_quad_batch_t* batch)
{
    return (batch->index_count >= batch->max_indices) || (batch->texture_index >= MAX_BATCH_TEXTURES);
}

void oslo_gfx_init(oslo_t* oslo)
{
    oslo_gfx_t* gfx = &oslo->gfx;
    memset(gfx, 0, sizeof(oslo_gfx_t));

    gfx->textures = oslo_slot_array_new(oslo_gfx_texture_t);
    
    load_shader(oslo);
    setup_projection(oslo);

    // Textures
    uint32_t white_texture_data = 0xffffffff;
    gfx->white_texture = oslo_gfx_create_texture(&white_texture_data, 1, 1, 4);

    oslo_gfx_quad_batch_create(MAX_QUADS, &gfx->default_batch);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_BLEND );
}

void oslo_gfx_shutdown(oslo_t* oslo)
{
    oslo_gfx_quad_batch_destroy(&oslo->gfx.default_batch);

    for (oslo_slot_array_iter it = 1; oslo_slot_array_iter_valid(oslo->gfx.textures, it); oslo_slot_array_iter_advance(oslo->gfx.textures, it))
    {
        oslo_gfx_texture_t* texture = oslo_slot_array_getp(oslo->gfx.textures, it);
        unload_texture(texture);
    }

    oslo_slot_array_free(oslo->gfx.textures);
    glDeleteProgram(oslo->gfx.shader);
}

void oslo_gfx_begin()
{
    // Update projection? 
    // We can update projection only when framebuffer size changes, or on demand
    oslo_gfx_begin_batch(instance);
}

void oslo_gfx_end()
{
    oslo_gfx_quad_batch_update_content(&instance->gfx.default_batch);
    oslo_gfx_quad_batch_render(&instance->gfx.default_batch);
}

void oslo_gfx_begin_batch(oslo_t* oslo)
{
    oslo_gfx_t* gfx = &oslo->gfx;
    oslo_gfx_quad_batch_reset(&oslo->gfx.default_batch);
}

void oslo_gfx_next_batch(oslo_t* oslo)
{
    oslo_gfx_quad_batch_update_content(&oslo->gfx.default_batch);
    oslo_gfx_quad_batch_render(&oslo->gfx.default_batch);
    oslo_gfx_begin_batch(oslo);
}

void oslo_gfx_draw_quad(vec2 position, float rotation, vec2 size, vec4 color)
{
    if (oslo_gfx_quad_batch_is_full(&instance->gfx.default_batch))
        oslo_gfx_next_batch(instance);

    oslo_gfx_quad_batch_draw_quad(&instance->gfx.default_batch, &(draw_quad_desc_t)
    {
        .position = position,
        .rotation = rotation,
        .size = size,
        .color = color
    });
}

void oslo_gfx_draw_texture(vec2 position, float rotation, vec2 size, vec4 tint, oslo_texture_id texture)
{
    if (oslo_gfx_quad_batch_is_full(&instance->gfx.default_batch))
        oslo_gfx_next_batch(instance);

    oslo_gfx_quad_batch_draw_texture(&instance->gfx.default_batch, &(draw_texture_desc_t)
    {
        .position = position,
        .rotation = rotation,
        .size = size,
        .color = tint,
        .texture = texture
    });
}

void oslo_gfx_draw_textured_quad(vec4 quad[4], vec4 tint, oslo_texture_id texture, oslo_rect_t rect, bool flip_horizontal)
{
   if (oslo_gfx_quad_batch_is_full(&instance->gfx.default_batch))
        oslo_gfx_next_batch(instance);

    oslo_gfx_quad_batch_draw_textured_quad(&instance->gfx.default_batch, &(draw_textured_quad_desc_t)
    {
        .quad = {quad[0], quad[1], quad[2], quad[3]},
        .color = tint,
        .texture = texture,
        .section = rect,
        .flip_horizontal = flip_horizontal
    });
}

void oslo_gfx_draw_texture_section(vec2 position, float rotation, vec2 size, vec4 tint, oslo_texture_id texture, oslo_rect_t rect, bool flip_horizontal)
{
    if (oslo_gfx_quad_batch_is_full(&instance->gfx.default_batch))
        oslo_gfx_next_batch(instance);

    oslo_gfx_quad_batch_draw_texture_section(&instance->gfx.default_batch, &(draw_texture_section_desc_t)
    {
        .position = position,
        .rotation = rotation,
        .size = size,
        .color = tint,
        .texture = texture,
        .section = rect,
        .flip_horizontal = flip_horizontal
    });
}

oslo_texture_id oslo_gfx_create_texture(void* data, uint32_t width, uint32_t height, uint32_t num_channels)
{
    oslo_gfx_texture_t texture = default_val();
    texture.width = width;
    texture.height = height;
    texture.channels = num_channels;

    GLenum internal_format = 0, data_format = 0;
    if (num_channels == 4)
    {
        internal_format = GL_RGBA8;
        data_format = GL_RGBA;
    }
    else
    {
        internal_format = GL_RGB8;
        data_format = GL_RGB;
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &texture.id);
    glTextureStorage2D(texture.id, 1, internal_format, width, height);

    glTextureParameteri(texture.id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(texture.id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(texture.id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(texture.id, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureSubImage2D(texture.id, 0, 0, 0, width, height, data_format, GL_UNSIGNED_BYTE, data);

    oslo_gfx_t* gfx = &instance->gfx;
    return oslo_slot_array_insert(gfx->textures, texture);
}

oslo_texture_id oslo_gfx_load_texture(const char* path)
{
    int width, height, channels;
    unsigned char *data = stbi_load(path, &width, &height, &channels, 0);
    oslo_texture_id texture = oslo_gfx_create_texture(data, width, height, channels);   
    stbi_image_free(data);
    return texture;
}

void oslo_gfx_unload_texture(oslo_texture_id texture)
{
    oslo_gfx_texture_t* p_texture = oslo_slot_array_getp(instance->gfx.textures, texture);
    if (p_texture != NULL)
    {
        unload_texture(p_texture);
        oslo_slot_array_erase(instance->gfx.textures, texture);
    }
}

void oslo_gfx_text(const char* text, vec2 position, float size, vec4 color, oslo_font_t* font)
{
    while (text[0] != '\0')
    {
        char c = text[0];
        if (c >= 32 && c < 127)
        {
            stbtt_aligned_quad q = default_val();
			stbtt_GetBakedQuad((stbtt_bakedchar*)&font->glyphs[0], 512, 512, c-32, &position.x, &position.y, &q, 1);

            oslo_rect_t section = 
            {
                .min = {q.s0 * 512, q.t0 * 512},
                .max = {q.s1 * 512, q.t1 * 512}
            };

            vec4 quad[4] = 
            {
                {q.x0, q.y0, 0.0f, 1.0f},
                {q.x1, q.y0, 0.0f, 1.0f},
                {q.x1, q.y1, 0.0f, 1.0f},
                {q.x0, q.y1, 0.0f, 1.0f}
            };

            oslo_gfx_draw_textured_quad(quad, color, font->texture, section, false);
        }

        ++text;
    }
}

#pragma endregion

#pragma region INPUT
/*========================
// Input
========================*/

oslo_keycode glfw_key_to_oslo_keycode(u32 code)
{
    switch (code)
    {
    case GLFW_KEY_A:                return OSLO_KEYCODE_A; break;
    case GLFW_KEY_B:                return OSLO_KEYCODE_B; break;
    case GLFW_KEY_C:                return OSLO_KEYCODE_C; break;
    case GLFW_KEY_D:                return OSLO_KEYCODE_D; break;
    case GLFW_KEY_E:                return OSLO_KEYCODE_E; break;
    case GLFW_KEY_F:                return OSLO_KEYCODE_F; break;
    case GLFW_KEY_G:                return OSLO_KEYCODE_G; break;
    case GLFW_KEY_H:                return OSLO_KEYCODE_H; break;
    case GLFW_KEY_I:                return OSLO_KEYCODE_I; break;
    case GLFW_KEY_J:                return OSLO_KEYCODE_J; break;
    case GLFW_KEY_K:                return OSLO_KEYCODE_K; break;
    case GLFW_KEY_L:                return OSLO_KEYCODE_L; break;
    case GLFW_KEY_M:                return OSLO_KEYCODE_M; break;
    case GLFW_KEY_N:                return OSLO_KEYCODE_N; break;
    case GLFW_KEY_O:                return OSLO_KEYCODE_O; break;
    case GLFW_KEY_P:                return OSLO_KEYCODE_P; break;
    case GLFW_KEY_Q:                return OSLO_KEYCODE_Q; break;
    case GLFW_KEY_R:                return OSLO_KEYCODE_R; break;
    case GLFW_KEY_S:                return OSLO_KEYCODE_S; break;
    case GLFW_KEY_T:                return OSLO_KEYCODE_T; break;
    case GLFW_KEY_U:                return OSLO_KEYCODE_U; break;
    case GLFW_KEY_V:                return OSLO_KEYCODE_V; break;
    case GLFW_KEY_W:                return OSLO_KEYCODE_W; break;
    case GLFW_KEY_X:                return OSLO_KEYCODE_X; break;
    case GLFW_KEY_Y:                return OSLO_KEYCODE_Y; break;
    case GLFW_KEY_Z:                return OSLO_KEYCODE_Z; break;
    case GLFW_KEY_LEFT_SHIFT:       return OSLO_KEYCODE_LEFT_SHIFT; break;
    case GLFW_KEY_RIGHT_SHIFT:      return OSLO_KEYCODE_RIGHT_SHIFT; break;
    case GLFW_KEY_LEFT_ALT:         return OSLO_KEYCODE_LEFT_ALT; break;
    case GLFW_KEY_RIGHT_ALT:        return OSLO_KEYCODE_RIGHT_ALT; break;
    case GLFW_KEY_LEFT_CONTROL:     return OSLO_KEYCODE_LEFT_CONTROL; break;
    case GLFW_KEY_RIGHT_CONTROL:    return OSLO_KEYCODE_RIGHT_CONTROL; break;
    case GLFW_KEY_BACKSPACE:        return OSLO_KEYCODE_BACKSPACE; break;
    case GLFW_KEY_BACKSLASH:        return OSLO_KEYCODE_BACKSLASH; break;
    case GLFW_KEY_SLASH:            return OSLO_KEYCODE_SLASH; break;
    case GLFW_KEY_GRAVE_ACCENT:     return OSLO_KEYCODE_GRAVE_ACCENT; break;
    case GLFW_KEY_COMMA:            return OSLO_KEYCODE_COMMA; break;
    case GLFW_KEY_PERIOD:           return OSLO_KEYCODE_PERIOD; break;
    case GLFW_KEY_ESCAPE:           return OSLO_KEYCODE_ESC; break;
    case GLFW_KEY_SPACE:            return OSLO_KEYCODE_SPACE; break;
    case GLFW_KEY_LEFT:             return OSLO_KEYCODE_LEFT; break;
    case GLFW_KEY_UP:               return OSLO_KEYCODE_UP; break;
    case GLFW_KEY_RIGHT:            return OSLO_KEYCODE_RIGHT; break;
    case GLFW_KEY_DOWN:             return OSLO_KEYCODE_DOWN; break;
    case GLFW_KEY_0:                return OSLO_KEYCODE_0; break;
    case GLFW_KEY_1:                return OSLO_KEYCODE_1; break;
    case GLFW_KEY_2:                return OSLO_KEYCODE_2; break;
    case GLFW_KEY_3:                return OSLO_KEYCODE_3; break;
    case GLFW_KEY_4:                return OSLO_KEYCODE_4; break;
    case GLFW_KEY_5:                return OSLO_KEYCODE_5; break;
    case GLFW_KEY_6:                return OSLO_KEYCODE_6; break;
    case GLFW_KEY_7:                return OSLO_KEYCODE_7; break;
    case GLFW_KEY_8:                return OSLO_KEYCODE_8; break;
    case GLFW_KEY_9:                return OSLO_KEYCODE_9; break;
    case GLFW_KEY_KP_0:             return OSLO_KEYCODE_KP_0; break;
    case GLFW_KEY_KP_1:             return OSLO_KEYCODE_KP_1; break;
    case GLFW_KEY_KP_2:             return OSLO_KEYCODE_KP_2; break;
    case GLFW_KEY_KP_3:             return OSLO_KEYCODE_KP_3; break;
    case GLFW_KEY_KP_4:             return OSLO_KEYCODE_KP_4; break;
    case GLFW_KEY_KP_5:             return OSLO_KEYCODE_KP_5; break;
    case GLFW_KEY_KP_6:             return OSLO_KEYCODE_KP_6; break;
    case GLFW_KEY_KP_7:             return OSLO_KEYCODE_KP_7; break;
    case GLFW_KEY_KP_8:             return OSLO_KEYCODE_KP_8; break;
    case GLFW_KEY_KP_9:             return OSLO_KEYCODE_KP_9; break;
    case GLFW_KEY_CAPS_LOCK:        return OSLO_KEYCODE_CAPS_LOCK; break;
    case GLFW_KEY_DELETE:           return OSLO_KEYCODE_DELETE; break;
    case GLFW_KEY_END:              return OSLO_KEYCODE_END; break;
    case GLFW_KEY_F1:               return OSLO_KEYCODE_F1; break;
    case GLFW_KEY_F2:               return OSLO_KEYCODE_F2; break;
    case GLFW_KEY_F3:               return OSLO_KEYCODE_F3; break;
    case GLFW_KEY_F4:               return OSLO_KEYCODE_F4; break;
    case GLFW_KEY_F5:               return OSLO_KEYCODE_F5; break;
    case GLFW_KEY_F6:               return OSLO_KEYCODE_F6; break;
    case GLFW_KEY_F7:               return OSLO_KEYCODE_F7; break;
    case GLFW_KEY_F8:               return OSLO_KEYCODE_F8; break;
    case GLFW_KEY_F9:               return OSLO_KEYCODE_F9; break;
    case GLFW_KEY_F10:              return OSLO_KEYCODE_F10; break;
    case GLFW_KEY_F11:              return OSLO_KEYCODE_F11; break;
    case GLFW_KEY_F12:              return OSLO_KEYCODE_F12; break;
    case GLFW_KEY_HOME:             return OSLO_KEYCODE_HOME; break;
    case GLFW_KEY_EQUAL:            return OSLO_KEYCODE_EQUAL; break;
    case GLFW_KEY_MINUS:            return OSLO_KEYCODE_MINUS; break;
    case GLFW_KEY_LEFT_BRACKET:     return OSLO_KEYCODE_LEFT_BRACKET; break;
    case GLFW_KEY_RIGHT_BRACKET:    return OSLO_KEYCODE_RIGHT_BRACKET; break;
    case GLFW_KEY_SEMICOLON:        return OSLO_KEYCODE_SEMICOLON; break;
    case GLFW_KEY_ENTER:            return OSLO_KEYCODE_ENTER; break;
    case GLFW_KEY_INSERT:           return OSLO_KEYCODE_INSERT; break;
    case GLFW_KEY_PAGE_UP:          return OSLO_KEYCODE_PAGE_UP; break;
    case GLFW_KEY_PAGE_DOWN:        return OSLO_KEYCODE_PAGE_DOWN; break;
    case GLFW_KEY_NUM_LOCK:         return OSLO_KEYCODE_NUM_LOCK; break;
    case GLFW_KEY_TAB:              return OSLO_KEYCODE_TAB; break;
    case GLFW_KEY_KP_MULTIPLY:      return OSLO_KEYCODE_KP_MULTIPLY; break;
    case GLFW_KEY_KP_DIVIDE:        return OSLO_KEYCODE_KP_DIVIDE; break;
    case GLFW_KEY_KP_ADD:           return OSLO_KEYCODE_KP_ADD; break;
    case GLFW_KEY_KP_SUBTRACT:      return OSLO_KEYCODE_KP_SUBTRACT; break;
    case GLFW_KEY_KP_ENTER:         return OSLO_KEYCODE_KP_ENTER; break;
    case GLFW_KEY_KP_DECIMAL:       return OSLO_KEYCODE_KP_DECIMAL; break;
    case GLFW_KEY_PAUSE:            return OSLO_KEYCODE_PAUSE; break;
    case GLFW_KEY_PRINT_SCREEN:     return OSLO_KEYCODE_PRINT_SCREEN; break;
    default:                        return OSLO_KEYCODE_COUNT; break;
    }

    // Shouldn't reach here
    return OSLO_KEYCODE_COUNT;
}

oslo_mouse_button_code glfw_to_oslo_button_code(s32 code)
{
    switch (code)
    {
        case GLFW_MOUSE_BUTTON_LEFT:    return OSLO_MOUSE_LBUTTON; break;
        case GLFW_MOUSE_BUTTON_RIGHT:   return OSLO_MOUSE_RBUTTON; break;
        case GLFW_MOUSE_BUTTON_MIDDLE: return OSLO_MOUSE_MBUTTON; break;
    }   

    // Shouldn't reach here
    return OSLO_MOUSE_BUTTON_CODE_COUNT;
}

void oslo_input_init(oslo_t* oslo)
{
    glfwMakeContextCurrent(oslo->window);
    glfwSetKeyCallback(oslo->window, &__glfw_key_callback);
    glfwSetCharCallback(oslo->window, &__glfw_char_callback);
    glfwSetMouseButtonCallback(oslo->window, &__glfw_mouse_button_callback);
    glfwSetCursorEnterCallback(oslo->window, &__glfw_mouse_cursor_enter_callback);
    glfwSetCursorPosCallback(oslo->window, &__glfw_mouse_cursor_position_callback);
    glfwSetScrollCallback(oslo->window, &__glfw_mouse_scroll_wheel_callback);
    glfwSetFramebufferSizeCallback(oslo->window, &__glfw_frame_buffer_size_callback);
}

void oslo_update_input(oslo_input_t* input)
{
    oslo_for_range_i(OSLO_KEYCODE_COUNT)
    {
        input->prev_key_map[i] = input->key_map[i];
    }

    oslo_for_range_i(OSLO_MOUSE_BUTTON_CODE_COUNT)
    {
        input->mouse.prev_map[i] = input->mouse.button_map[i];
    }

    input->mouse.wheel = v2(0,0);
    input->mouse.delta = v2(0, 0);
    input->mouse.moved_this_frame = false;
}

b32 oslo_is_key_pressed(oslo_keycode code)
{
    oslo_input_t* input = &instance->input;
    if (code >= OSLO_KEYCODE_COUNT)
    {
        return false;
    }

    return input->key_map[code];
}

b32 oslo_was_key_pressed(oslo_keycode code)
{
    oslo_input_t* input = &instance->input;
    if (code >= OSLO_KEYCODE_COUNT)
    {
        return false;
    }

    return input->prev_key_map[code];
}

b32 oslo_is_key_released(oslo_keycode code)
{
    oslo_input_t* input = &instance->input;
    if (code >= OSLO_KEYCODE_COUNT)
    {
        return false;
    }

    return input->prev_key_map[code] && !input->key_map[code];
}

vec2 oslo_get_mouse_position()
{
    oslo_input_t* input = &instance->input;
    return input->mouse.position;
}

vec2 oslo_get_mouse_delta()
{
    oslo_input_t* input = &instance->input;
    return input->mouse.delta;
}

b32 oslo_is_mouse_button_pressed(oslo_mouse_button_code code)
{
    oslo_input_t* input = &instance->input;

    if (code >= OSLO_MOUSE_BUTTON_CODE_COUNT)
    {
        return false;
    }

    return input->mouse.button_map[code];
}

b32 oslo_was_mouse_button_pressed(oslo_mouse_button_code code)
{
    oslo_input_t* input = &instance->input;

    if (code >= OSLO_MOUSE_BUTTON_CODE_COUNT)
    {
        return false;
    }

    return input->mouse.prev_map[code];
}

b32 oslo_is_mouse_button_released(oslo_mouse_button_code code)
{
    oslo_input_t* input = &instance->input;

    if (code >= OSLO_MOUSE_BUTTON_CODE_COUNT)
    {
        return false;
    }

    return input->mouse.prev_map[code] && !input->mouse.button_map[code];
}

// Input callbacks
void __glfw_key_callback(GLFWwindow* window, s32 key, s32 scancode, s32 action, s32 mods)
{
    oslo_input_t* input = &instance->input;
    oslo_keycode keycode = glfw_key_to_oslo_keycode(key);

    if (keycode != OSLO_KEYCODE_COUNT)
    {
        b32 pressed = action > 0;
        input->key_map[keycode] = pressed;
    }
}

void __glfw_char_callback(GLFWwindow* window, uint32_t codepoint)
{

}

void __glfw_mouse_button_callback(GLFWwindow* window, s32 button, s32 action, s32 mods)
{
    oslo_input_t* input = &instance->input;
    
    oslo_mouse_button_code code = glfw_to_oslo_button_code(button);
    switch(action)
    {
        // Released
        case 0:
        {
            if ((uint32_t)code < (uint32_t)OSLO_MOUSE_BUTTON_CODE_COUNT)
            {
                input->mouse.button_map[code] = false;   
            }
        } break;

        // Pressed
        case 1:
        case 2:
        {
            if ((uint32_t)code < (uint32_t)OSLO_MOUSE_BUTTON_CODE_COUNT)
            {
                input->mouse.button_map[code] = true;   
            }
        } break;
    }
}

void __glfw_mouse_cursor_position_callback(GLFWwindow* window, f64 x, f64 y)
{
    oslo_input_t* input = &instance->input;
    // Calculate delta
    input->mouse.delta = vec2_sub(v2(x, y), input->mouse.position);
    
    // Update position
    input->mouse.position.x = x;
    input->mouse.position.y = y;
}

void __glfw_mouse_scroll_wheel_callback(GLFWwindow* window, f64 xoffset, f64 yoffset)
{

}

void __glfw_mouse_cursor_enter_callback(GLFWwindow* window, s32 entered)
{

}

void __glfw_frame_buffer_size_callback(GLFWwindow* window, s32 width, s32 height)
{
    // Do we want to support a fixed aspect ratio?
    // If so we need to change this to preserve the ratio specified.
    // Also we'll need to support a way to transform from window space to world space
    // Since right now both share the same coords
    instance->gfx.projection = mat4_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);

    // Upload projection
    glUseProgram(instance->gfx.shader);
    glUniformMatrix4fv(glGetUniformLocation(instance->gfx.shader, "u_projection"), 1, GL_FALSE, &instance->gfx.projection.elements[0]);
    glUseProgram(0);

    glViewport(0, 0, width, height);
}

void __glfw_drop_callback(GLFWwindow* window)
{

}

void oslo_set_window_title(const char* title)
{
    glfwSetWindowTitle(instance->window, title);
}

float oslo_get_delta_time()
{
    return instance->time.delta;
}

#pragma endregion

#pragma region AUDIO

typedef struct miniaudio_data_t
{
    ma_context context;
    ma_device device;
    ma_device_config device_config;
    ma_mutex lock;
} miniaudio_data_t;

void ma_audio_commit(ma_device* device, void* output, const void* input, ma_uint32 frame_count)
{
    oslo_audio_t* audio = &instance->audio;
    miniaudio_data_t* ma = (miniaudio_data_t*)audio->user_data;
    memset(output, 0, frame_count * device->playback.channels * ma_get_bytes_per_sample(device->playback.format));

    u32 destroy_count = 0;
    uint32_t handles_to_destroy[32];
    if(!audio->instances)
        return;

    if (audio->commit)
    {
        audio->commit((int16_t*)output, 2, ma->device_config.sampleRate, frame_count);
    }

    oslo_audio_mutex_lock();
    {
        for (
            oslo_slot_array_iter it = oslo_slot_array_iter_new(audio->instances);
            oslo_slot_array_iter_valid(audio->instances, it);
            oslo_slot_array_iter_advance(audio->instances, it)
        )
        {
            if (it == -1)
                continue;

            oslo_audio_instance_t* inst = oslo_slot_array_iter_getp(audio->instances, it);
            oslo_audio_source_t* src = oslo_slot_array_getp(audio->sources, inst->src);

            // Easy out if the instance is not playing currently or the source is invalid
            if ((!inst->playing) || !src)
            {
                if (!inst->persistent)
                    handles_to_destroy[destroy_count++] = it;
                
                continue;
            }

            s16* sample_out = (s16*)output;
            s16* samples = (s16*)src->samples;

            u64 samples_to_write = (u64)frame_count;
            f64 sample_volume = inst->volume;

            // Write to channels
            for (u64 write_sample = 0; write_sample < samples_to_write; ++write_sample)
            {
                s32 channels = src->channels;
                f64 start_sample_position = inst->sample_position;
                s16 start_left_sample;
                s16 start_right_sample;

                // Not sure about this line of code...
                f64 target_sample_position = start_sample_position + (f64)channels * (f64)inst->pitch;

                if (target_sample_position >= src->sample_count)
                {
                    target_sample_position -= src->sample_count;
                }

                s16 target_left_sample = 0;
                s16 target_right_sample = 0;

                {
                    u64 left_idx = (u64)start_sample_position;
                    if (channels > 1)
                    {
                        left_idx &= ~((u64)(0x01));
                    }
                    u64 right_idx = left_idx + (channels - 1);

                    s16 first_left_sample = samples[left_idx];
                    s16 first_right_sample = samples[right_idx];
                    s16 second_left_sample = samples[left_idx + channels];
                    s16 second_right_sample = samples[right_idx + channels];

                    start_left_sample = (s16)(first_left_sample + (second_left_sample - first_left_sample) * (start_sample_position / channels - (u64)(start_sample_position / channels)));
                    start_right_sample = (s16)(first_right_sample + (second_right_sample - first_right_sample) * (start_sample_position / channels - (u64)(start_sample_position / channels)));
                } 

                s16 left_sample = (s16)((((s64)start_left_sample + (s64)target_left_sample) / 2) * sample_volume);
                s16 right_sample = (s16)((((s64)start_right_sample + (s64)target_right_sample) / 2) * sample_volume);

                *sample_out++ += left_sample;  // Left
                *sample_out++ += right_sample; // Right

                // Possibly need fixed sampling instead
                inst->sample_position = target_sample_position;

                // Loop sound if necessary
                if(inst->sample_position >= src->sample_count - channels - 1)
                {
                    if(inst->loop)
                    {
                        // inst->sample_position -= src->sample_count;
                        inst->sample_position = 0;
                    }
                    else
                    {
                        inst->playing = false;
                        inst->sample_position = 0;
                        if (!inst->persistent)
                        {
                            // Need to destroy the instance at this point...
                            handles_to_destroy[destroy_count++] = it;
                        }
                        break;
                    }
                }
            }
        }

        // Destroy instances
        for (uint32_t i = 0; i < destroy_count; ++i)
        {
            oslo_slot_array_erase(audio->instances, handles_to_destroy[i]);
        }
    }

    oslo_audio_mutex_unlock();
}

void oslo_audio_init()
{
    oslo_audio_t* audio = &instance->audio;
    audio->sources = oslo_slot_array_new(oslo_audio_source_t);
    audio->instances = oslo_slot_array_new(oslo_audio_instance_t);
    audio->max_volume = 1.0f;
    audio->min_volume = 0.0f;
    audio->commit = NULL;

    audio->user_data = malloc(sizeof(miniaudio_data_t));
    memset(audio->user_data, 0, sizeof(miniaudio_data_t));

    oslo_slot_array_reserve(audio->instances, 1024);
    
    miniaudio_data_t* output = (miniaudio_data_t*)audio->user_data;
    ma_result result = default_val();
    ma_context_config ctx_config = ma_context_config_init();
    
    result = ma_context_init(NULL, 0, &ctx_config, &output->context);
    if (result != MA_SUCCESS)
    {
        notify_error(instance, OSLO_AUDIO_INIT_ERROR, "Failed to init audio");
        return;
    }

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;  // NULL for the default playback AUDIO.System.device.
    config.playback.format = ma_format_s16;
    config.playback.channels = 2;
    config.capture.pDeviceID = NULL;  // NULL for the default capture AUDIO.System.device.
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = 44100;
    config.dataCallback = &ma_audio_commit;
    config.pUserData = NULL;

    ma_device_info* pPlaybackDeviceInfos; 
    ma_uint32 playbackDeviceCount;
    ma_device_info* pCaptureDeviceInfos; 
    ma_uint32 captureDeviceCount;
    ma_uint32 iDevice;
    
    result = ma_context_get_devices(&output->context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
    if (result != MA_SUCCESS) 
    {
        notify_error(instance, OSLO_AUDIO_INIT_ERROR, "Failed to get audio devices");
        return;
    }

    output->device_config = config;

    if ((ma_device_init(NULL, &output->device_config, &output->device)) != MA_SUCCESS)
    {
        notify_error(instance, OSLO_AUDIO_INIT_ERROR, "Failed to init device");
        return;
    }

    if ((ma_device_start(&output->device)) != MA_SUCCESS) 
    {
        notify_error(instance, OSLO_AUDIO_INIT_ERROR, "Failed to start device");
        return;
    }

    // Initialize the mutex, ya dummy
    if (ma_mutex_init(&output->lock) != MA_SUCCESS)
    {
        notify_error(instance, OSLO_AUDIO_INIT_ERROR, "Failed to init audio mutex");
        return;
    }
}

void oslo_audio_shutdown()
{
    oslo_audio_t* audio = &instance->audio;

    miniaudio_data_t* ma = (miniaudio_data_t*)audio->user_data; 

    ma_context_uninit(&ma->context);
    ma_device_uninit(&ma->device);
    ma_mutex_uninit(&ma->lock);

    oslo_slot_array_free(audio->sources);
    oslo_slot_array_free(audio->instances);

    miniaudio_data_t* data = (miniaudio_data_t*)audio->user_data;
    free(data);
}

bool oslo_audio_load_ogg_from_file(const char* path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples)
{
    size_t len = 0;
    char* file_data = oslo_read_file_contents(path, "rb", &len);
    *sample_count = stb_vorbis_decode_memory((const unsigned char*)file_data, (size_t)len, channels, sample_rate, (s16**)samples);
    free(file_data);

    if (!*samples || *sample_count == -1)
    {
        *samples = NULL;
        notify_error(instance, OSLO_LOAD_ERROR, "Failed to load .ogg file");
        return false;
    }
    
    *sample_count *= *channels;

    return true;
}

bool oslo_audio_load_wav_from_file(const char* path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples)
{
    size_t len = 0;
    char* file_data = oslo_read_file_contents(path, "rb", &len);
    uint64_t total_pcm_frame_count = 0;
    *samples =  drwav_open_memory_and_read_pcm_frames_s16(file_data, len, (uint32_t*)channels, (uint32_t*)sample_rate, &total_pcm_frame_count, NULL);
    free(file_data);

    if (!*samples) 
    {
        *samples = NULL; 
        notify_error(instance, OSLO_LOAD_ERROR, "Failed to load .wav file");
        return false; 
    }

    *sample_count = total_pcm_frame_count * *channels;

    return true;
}

bool oslo_audio_load_mp3_from_file(const char* path, int32_t* sample_count, int32_t* channels, int32_t* sample_rate, void** samples)
{
    size_t len = 0;
    char* file_data = oslo_read_file_contents(path, "rb", &len);
    uint64_t total_pcm_frame_count = 0;
    drmp3_config cfg = default_val();
    *samples =  drmp3_open_memory_and_read_pcm_frames_s16(file_data, len, &cfg, (drmp3_uint64*)&total_pcm_frame_count, NULL);
    free(file_data);

    if (!*samples)
    {
        *samples = NULL; 
        notify_error(instance, OSLO_LOAD_ERROR, "Failed to load .mp3 file");
        return false; 
    }

    *channels = cfg.channels;
    *sample_rate = cfg.sampleRate;
    *sample_count = total_pcm_frame_count * *channels;

    return true;
}

void oslo_audio_mutex_lock()
{
    oslo_audio_t* audio = &instance->audio;
    miniaudio_data_t* ma = (miniaudio_data_t*)audio->user_data;
    ma_mutex_lock(&ma->lock);
}

void oslo_audio_mutex_unlock()
{
    oslo_audio_t* audio = &instance->audio;
    miniaudio_data_t* ma = (miniaudio_data_t*)audio->user_data;
    ma_mutex_unlock(&ma->lock);
}

void oslo_audio_register_commit(oslo_audio_commit commit)
{
    oslo_audio_t* audio = &instance->audio;
    audio->commit = commit;
}

oslo_audio_source_id oslo_audio_load_from_file(const char* path)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_source_t src = default_val();
    uint32_t handle = -1;
    bool32_t load_successful = false;

    if(!oslo_platform_file_exists(path)) {
        notify_error(instance, OSLO_LOAD_ERROR, "Failed to load audio file");
        return handle;
    }

    char ext[64] = default_val();
    oslo_str_to_lower(path, ext, sizeof(ext));
    oslo_get_file_extension(ext, sizeof(ext), ext);

    // Load OGG data
    if (oslo_string_compare_equal(ext, "ogg"))
    {
        load_successful = oslo_audio_load_ogg_from_file (
            path, 
            &src.sample_count, 
            &src.channels,
            &src.sample_rate, 
            &src.samples
        );
    }

    // Load WAV data
    if (oslo_string_compare_equal(ext, "wav"))
    {
        load_successful = oslo_audio_load_wav_from_file (
            path, 
            &src.sample_count, 
            &src.channels,
            &src.sample_rate, 
            &src.samples
        );
    }

    if (oslo_string_compare_equal(ext, "mp3"))
    {
        load_successful = oslo_audio_load_mp3_from_file (
            path, 
            &src.sample_count, 
            &src.channels,
            &src.sample_rate, 
            &src.samples
        );
    }

    // Load raw source into memory and return handle id
    if (load_successful)
    {
        // Add to resource cache
        handle = oslo_slot_array_insert(audio->sources, src);
    }

    return handle;
}

oslo_audio_instance_id oslo_audio_create_instance(oslo_audio_instance_t* inst)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_mutex_lock();
    uint32_t hndl = oslo_slot_array_insert(audio->instances, *inst);
    oslo_audio_mutex_unlock();
    return hndl;
}

void oslo_audio_play_source(oslo_audio_source_id src, float volume)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_instance_t decl = default_val();
    decl.src = src;
    decl.volume = oslo_clamp(volume, audio->min_volume, audio->max_volume);
    decl.pitch = 1.0f;
    decl.persistent = false;
    uint32_t inst = oslo_audio_create_instance(&decl);
    oslo_audio_play(inst);
}

void oslo_audio_play(oslo_audio_instance_id inst)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_mutex_lock();
    if (inst != -1)
    {
        oslo_slot_array_getp(audio->instances, inst)->playing = true;
    }
    oslo_audio_mutex_unlock();
}

void oslo_audio_pause(oslo_audio_instance_id inst)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_mutex_lock();
    if (inst != -1)
    {
        oslo_slot_array_getp(audio->instances, inst)->playing = false;
    }
    oslo_audio_mutex_unlock();
}

void oslo_audio_stop(oslo_audio_instance_id inst)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_mutex_lock();
    if (inst != -1)
    {
        oslo_audio_instance_t* ip = oslo_slot_array_getp(audio->instances, inst);
        ip->playing = false;
        ip->sample_position = 0;
    }
    oslo_audio_mutex_unlock();
}

void oslo_audio_restart(oslo_audio_instance_id inst)
{
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_mutex_lock();
    if (inst != -1)
    {
        oslo_slot_array_getp(audio->instances, inst)->sample_position = 0;
    }
    oslo_audio_mutex_unlock();
}

bool oslo_audio_is_playing(oslo_audio_instance_id inst)
{
    bool playing = false;
    oslo_audio_t* audio = &instance->audio;
    oslo_audio_mutex_lock();
    if (inst != -1)
    {
        playing = oslo_slot_array_getp(audio->instances, inst)->playing;
    }
    oslo_audio_mutex_unlock();

    return playing;
}

float oslo_audio_get_volume(oslo_audio_instance_id inst)
{
    oslo_audio_t* audio = &instance->audio;
    if (inst != -1)
    {
        return oslo_slot_array_getp(audio->instances, inst)->volume;
    }

    return 0.0f;
}

void oslo_audio_set_volume(oslo_audio_instance_id inst, float volume)
{
    oslo_audio_t* audio = &instance->audio;
    if (inst != -1)
    {
        oslo_slot_array_getp(audio->instances, inst)->volume = volume;
    }   
}

#pragma endregion

#pragma region FILESYSTEM
// Files
int32_t oslo_file_size_in_bytes(const char* file_path)
{
    #ifdef OSLO_PLATFORM_WIN

        HANDLE hFile = CreateFile(file_path, GENERIC_READ, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 
        FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile==INVALID_HANDLE_VALUE)
            return -1; // error condition, could call GetLastError to find out more

        LARGE_INTEGER size;
        if (!GetFileSizeEx(hFile, &size))
        {
            CloseHandle(hFile);
            return -1; // error condition, could call GetLastError to find out more
        }

        CloseHandle(hFile);
        return (uint32_t)(size.QuadPart);
    #else

        struct stat st;
        stat(file_path, &st);
        return (int32_t)st.st_size; 
    #endif
}

char* oslo_read_file_contents(const char* file_path, const char* mode, size_t* sz)
{
    const char* path = file_path;

    #ifdef OSLO_PLATFORM_ANDROID
        const char* internal_data_path = oslo_app()->android.internal_data_path;
        oslo_snprintfc(tmp_path, 1024, "%s/%s", internal_data_path, file_path);
        path = tmp_path;
    #endif

    char* buffer = 0;
    FILE* fp = fopen(path, mode);
    size_t read_sz = 0;
    if (fp)
    {
        read_sz = oslo_file_size_in_bytes(file_path);
        buffer = (char*)malloc(read_sz + 1);
        if (buffer) {
           size_t _r = fread(buffer, 1, read_sz, fp);
        }
        buffer[read_sz] = '\0';
        fclose(fp);
        if (sz) *sz = read_sz;
    }

    return buffer;
}
#pragma endregion

#pragma region FONTS
// Fonts
bool oslo_load_font_from_file(const char* path, uint32_t point_size, oslo_font_t* out_font)
{
    size_t len = 0;
    char* ttf = oslo_read_file_contents(path, "rb", &len);
    if (!point_size) 
    {
        point_size = 16;
    }
    
    bool ret = oslo_load_font_from_memory(ttf, len, point_size, out_font);
    if (!ret)
    {
        notify_error(instance, OSLO_LOAD_ERROR, "Failed to load font!");
    }

    free(ttf);
    return ret;
}

void oslo_unload_font(oslo_font_t* font)
{
    if (font != NULL)
    {
        oslo_gfx_unload_texture(font->texture);
    }
}

bool oslo_load_font_from_memory(void* memory, size_t len, uint32_t point_size, oslo_font_t* out_font)
{
    if (!point_size) 
    {
        point_size = 16;
    }

    const uint32_t num_comps = 4;
    u8* alpha_bitmap = (uint8_t*)malloc(512 * 512);
    u8* flipmap = (uint8_t*)malloc(512 * 512 * num_comps);
    memset(alpha_bitmap, 0, 512 * 512);
    memset(flipmap, 0, 512 * 512 * num_comps);
    s32 v = stbtt_BakeFontBitmap((u8*)memory, 0, (float)point_size, alpha_bitmap, 512, 512, 32, 96, (stbtt_bakedchar*)out_font->glyphs); // no guarantee this fits!

    // Flip texture
    u32 r = 512 - 1;
    for (u32 i = 0; i < 512; ++i)
    {
        for (u32 j = 0; j < 512; ++j)
        {
            u32 i0 = i * 512 + j;
            u32 i1 = i * 512 * num_comps + j * num_comps;
            u8 a = alpha_bitmap[i0];
            flipmap[i1 + 0] = 255;
            flipmap[i1 + 1] = 255;
            flipmap[i1 + 2] = 255;
            flipmap[i1 + 3] = a;
        }
        r--;
    } 

    out_font->texture = oslo_gfx_create_texture(flipmap, 512, 512, 4);

    bool success = false;
    if (v <= 0) 
    {
        notify_error(instance, OSLO_LOAD_ERROR, "Font failed to load, Baked texture too small!");
    }
    else
    {
        success = true;
    }

    free(alpha_bitmap);
    free(flipmap);

    return success;
}

#pragma endregion

#pragma region DYN_ARRAY
/*========================
// Dynamic Array
========================*/

void* oslo_dyn_array_resize_impl(void* arr, size_t sz, size_t amount) 
{
    size_t capacity;

    if (arr) {
        capacity = amount;  
    } else {
        capacity = 0;
    }

    // Create new oslo_dyn_array with just the header information
    oslo_dyn_array* data = (oslo_dyn_array*)realloc(arr ? oslo_dyn_array_head(arr) : 0, capacity * sz + sizeof(oslo_dyn_array));

    if (data) {
        if (!arr) {
            data->size = 0;
        }
        data->capacity = (int32_t)capacity;
        return ((int32_t*)data + 2);
    }

    return NULL;
}

void** oslo_dyn_array_init(void** arr, size_t val_len)
{
    if (*arr == NULL) {
        oslo_dyn_array* data = (oslo_dyn_array*)malloc(val_len + sizeof(oslo_dyn_array));  // Allocate capacity of one
        data->size = 0;
        data->capacity = 1;
        *arr = ((int32_t*)data + 2);
    }
    return arr;
}

void oslo_dyn_array_push_data(void** arr, void* val, size_t val_len)
{
    if (*arr == NULL) {
        oslo_dyn_array_init(arr, val_len);
    }
    if (oslo_dyn_array_need_grow(*arr, 1)) 
    {
        int32_t capacity = oslo_dyn_array_capacity(*arr) * 2;

        // Create new oslo_dyn_array with just the header information
        oslo_dyn_array* data = (oslo_dyn_array*)realloc(oslo_dyn_array_head(*arr), capacity * val_len + sizeof(oslo_dyn_array));

        if (data) {
            data->capacity = capacity;
            *arr = ((int32_t*)data + 2);
        }
    }
    size_t offset = oslo_dyn_array_size(*arr);
    memcpy(((uint8_t*)(*arr)) + offset * val_len, val, val_len);
    oslo_dyn_array_head(*arr)->size++;
}

#pragma endregion

#pragma region SLOT_ARRAY
/*========================
// Slot Array
========================*/

void** oslo_slot_array_init(void** sa, size_t sz)
{
    if (*sa == NULL) {
        *sa = malloc(sz);
        memset(*sa, 0, sz);
        return sa;
    }
    else {
        return NULL;
    }
}

#pragma endregion

#pragma region HASH_TABLE
void __oslo_hash_table_init_impl(void** ht, size_t sz)
{
    *ht = malloc(sz);
}
#pragma endregion
#endif