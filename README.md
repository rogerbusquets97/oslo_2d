# oslo_2d
Header only 2D framework heavily inspired by https://github.com/MrFrenik/gunslinger. (definetely check this out).

C99 framework to get started with 2D application.
## Features
- Header-Only
- All external dependecies included
- Sprite batch rendering
- Text rendering
- User Input
- Audio

This is not intended to be the most performant solution, but a way to quickly start prototyping 2D stuff. 
## Basic Example
A simple c99 example using:

```
#define OSLO_IMPL
#include "oslo.h"

typedef struct my_data_t
{
    oslo_texture_id texture;
} my_data_t;

void my_init(void* user_data)
{
    my_data_t* data = (my_data_t*)user_data;
    data->texture = oslo_gfx_load_texture("my_texture_path.png");
}

void my_update(void* user_data)
{
    my_data_t* data = (my_data_t*)user_data;

    vec2 mpos = oslo_get_mouse_position();
    
    oslo_gfx_begin();
    oslo_gfx_draw_texture(mpos, 0.0f, v2(64, 64), OSLO_COLOR_WHITE, data->texture);
    oslo_gfx_end();
}

void my_shutdown(void* user_data)
{
    my_data_t* data = (my_data_t*)user_data;
    free(data);
}

oslo_desc_t oslo_main()
{
    my_data_t* data = malloc(sizeof(my_data_t));
    memset(data, 0, sizeof(my_data_t));

    return (oslo_desc_t)
    {
      .window_width = 640,
      .window_height = 480,
      .window_title = "Example 01",
      .user_data = data,
      .init = my_init,
      .update = my_update,
      .shutdown = my_shutdown,
      .on_oslo_error = NULL,
      .max_fps = 60
    };
}
```



