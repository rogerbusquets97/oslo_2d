#pragma once

typedef struct oslo_animation_event_t
{
    const char* event_name;
    float timepoint;
    bool triggered;
} oslo_animation_event_t;

typedef struct oslo_animation_frame_t
{
    oslo_rect_t rect;
    float duration;
} oslo_animation_frame_t;

typedef struct oslo_animation_event_listener_t
{
    void(*callback)(const char*, void*);
    void* user_data;
} oslo_animation_event_listener_t;

typedef struct oslo_animation_t
{
    oslo_dyn_array(oslo_animation_frame_t) frames;
    oslo_dyn_array(oslo_animation_event_t) events;
    oslo_dyn_array(oslo_animation_event_listener_t) event_listeners;

    uint32_t current_frame;
    float time;
    float frame_time;

    bool is_playing;
    bool loop;
} oslo_animation_t;

OSLO_API_DECL void oslo_animation_create(oslo_animation_t* anim);
OSLO_API_DECL void oslo_animation_destroy(oslo_animation_t* anim);

OSLO_API_DECL void oslo_animation_add_frame(oslo_animation_t* anim, oslo_animation_frame_t* frame);
OSLO_API_DECL void oslo_animation_add_event(oslo_animation_t* anim, oslo_animation_event_t* event);
OSLO_API_DECL void oslo_animation_add_event_listener(oslo_animation_t* anim, oslo_animation_event_listener_t* listener);

OSLO_API_DECL void oslo_animation_play(oslo_animation_t* anim);
OSLO_API_DECL void oslo_animation_stop(oslo_animation_t* anim);
OSLO_API_DECL void oslo_animation_reset(oslo_animation_t* anim);
OSLO_API_DECL bool oslo_animation_is_playing(oslo_animation_t* anim);
OSLO_API_DECL void oslo_animation_set_loop(oslo_animation_t* anim, bool loop);
OSLO_API_DECL const oslo_animation_frame_t* oslo_animation_get_current_frame(oslo_animation_t* anim);

OSLO_API_DECL void oslo_animation_tick(oslo_animation_t* anim);

#ifdef OSLO_ANIM_IMPL

void oslo_animation_reset_events(oslo_animation_t* anim)
{
    for (int i = 0; i < oslo_dyn_array_size(anim->events); ++i)
    {
        oslo_animation_event_t* e = &anim->events[i];
        e->triggered = false;
    }
}

void oslo_animation_notify_event(oslo_animation_t* anim, oslo_animation_event_t* e)
{
    for (int i = 0; i < oslo_dyn_array_size(anim->event_listeners); ++i)
    {
        oslo_animation_event_listener_t* listener = &anim->event_listeners[i];
        listener->callback(e->event_name, listener->user_data);
    }
}

void oslo_animation_create(oslo_animation_t* anim)
{
    if (anim != NULL)
    {
        anim->current_frame = 0;
        anim->time = 0;
        anim->frame_time = 0;
        anim->is_playing = false;
        anim->loop = false;
        anim->frames = oslo_dyn_array_new(oslo_animation_frame_t);
        anim->events = oslo_dyn_array_new(oslo_animation_event_t);
        anim->event_listeners = oslo_dyn_array_new(oslo_animation_event_listener_t);
    }
}

void oslo_animation_destroy(oslo_animation_t* anim)
{
    if (anim != NULL)
    {
        oslo_dyn_array_free(anim->frames);
        oslo_dyn_array_free(anim->events);
        oslo_dyn_array_free(anim->event_listeners);
    }
}

void oslo_animation_add_frame(oslo_animation_t* anim, oslo_animation_frame_t* frame)
{
    oslo_dyn_array_push(anim->frames, *frame);
}

void oslo_animation_add_event(oslo_animation_t* anim, oslo_animation_event_t* event)
{
    oslo_dyn_array_push(anim->events, *event);
}

void oslo_animation_add_event_listener(oslo_animation_t* anim, oslo_animation_event_listener_t* listener)
{
    oslo_dyn_array_push(anim->event_listeners, *listener);
}

void oslo_animation_tick(oslo_animation_t* anim)
{
    size_t num_frames = oslo_dyn_array_size(anim->frames);
    
    if (anim->is_playing && (num_frames > 0 && anim->current_frame < num_frames))
    {
        float dt = oslo_get_delta_time();
        oslo_animation_frame_t* frame = &anim->frames[anim->current_frame];

        anim->time += dt;
        anim->frame_time += dt;

        if (anim->frame_time >= frame->duration)
        {
            if (anim->loop)
            {
                anim->current_frame = ++anim->current_frame % num_frames;
                anim->frame_time = 0.0;
                oslo_animation_reset_events(anim);
            }
            else if ((anim->current_frame + 1) < num_frames)
            {
                anim->current_frame++;
                anim->frame_time = 0.0f;
            }
            else
            {
                anim->is_playing = false;
                anim->current_frame = 0;
                anim->time = 0.0f;
                anim->frame_time = 0.0f;
                oslo_animation_reset_events(anim);
            }
        }

        for (int i = 0; i < oslo_dyn_array_size(anim->events); ++i)
        {
            oslo_animation_event_t* e = &anim->events[i];
            if (!e->triggered && anim->time >= e->timepoint)
            {
                e->triggered = true;
                oslo_animation_notify_event(anim, e);
            }
        }
    }
}

void oslo_animation_play(oslo_animation_t* anim)
{
    anim->is_playing = true;
    anim->time = 0.0f;
    anim->frame_time = 0.0f;
    anim->current_frame = 0;
}

void oslo_animation_stop(oslo_animation_t* anim)
{
    anim->is_playing = false;
}

void oslo_animation_reset(oslo_animation_t* anim)
{
    anim->frame_time = 0.0f;
    anim->time = 0.0f;
    anim->current_frame = 0;
}

bool oslo_animation_is_playing(oslo_animation_t* anim)
{
    return anim->is_playing;
}

void oslo_animation_set_loop(oslo_animation_t* anim, bool loop)
{
    anim->loop = loop;
}

const oslo_animation_frame_t* oslo_animation_get_current_frame(oslo_animation_t* anim)
{
    return &anim->frames[anim->current_frame];
}

#endif

