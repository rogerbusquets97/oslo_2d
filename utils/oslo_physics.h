#pragma once

#define PIXELS_PER_METER 50
#define G 9.81f

typedef enum oslo_physics_shape_type
{
    Box2D,
    Circle,
    Polygon2D
} oslo_physics_shape_type;

typedef struct oslo_physics_circle_t
{
    float radius;
    vec2 center;
} oslo_physics_circle_t;

typedef struct oslo_physics_polygon_t
{
    int t;
} oslo_physics_polygon_t;

typedef struct oslo_physics_box_2d_t
{
    int t;
} oslo_physics_box_2d_t;

typedef struct oslo_physics_shape_t
{
    oslo_physics_shape_type type;
    union
    {
        oslo_physics_circle_t circle;
        oslo_physics_box_2d_t box;
    };

} oslo_physics_shape_t;

typedef struct oslo_physics_body_t
{
    // Linear motion
    vec2 position;
    vec2 velocity;
    vec2 acceleration;

    // Forces and torque
    vec2 sum_forces;
    float sum_torque;

     // Mass and Moment of Inertia
    float mass;
    float inv_mass;

} oslo_physics_body_t;

OSLO_API_DECL void oslo_physics_body_add_force(oslo_physics_body_t* body, vec2 force);
OSLO_API_DECL void oslo_physics_body_add_torque(oslo_physics_body_t* body, float torque);
OSLO_API_DECL void oslo_physics_body_apply_impluse(oslo_physics_body_t* body, vec2 impulse);

typedef struct oslo_physics_t
{
    oslo_dyn_array(vec2) forces;
    oslo_dyn_array(float) torques;
    oslo_dyn_array(oslo_physics_body_t) bodies;
} oslo_physics_t;

OSLO_API_DECL void oslo_physics_init(oslo_physics_t* out_physics);
OSLO_API_DECL void oslo_physics_shutdown(oslo_physics_t* physics);
OSLO_API_DECL void oslo_physics_update(oslo_physics_t* physics);

OSLO_API_DECL oslo_physics_body_t* oslo_physics_add_body(oslo_physics_t* physics, vec2 position, float mass);
OSLO_API_DECL void oslo_physics_remove_body(oslo_physics_t* physics, oslo_physics_body_t* body);

OSLO_API_DECL void oslo_physics_add_force(oslo_physics_t* physics, vec2 force);
OSLO_API_DECL void oslo_physics_add_torque(oslo_physics_t* physics, float torque);

#ifdef OSLO_PHYSICS_IMPL

void oslo_physics_body_integrate_forces(oslo_physics_body_t* body);
void oslo_physics_body_integrate_velocities(oslo_physics_body_t* body);

void oslo_physics_init(oslo_physics_t* out_physics)
{
    if (out_physics != NULL)
    {
        out_physics->bodies = oslo_dyn_array_new(oslo_physics_body_t);
        out_physics->forces = oslo_dyn_array_new(vec2);
        out_physics->torques = oslo_dyn_array_new(float);
    }
}

void oslo_physics_shutdown(oslo_physics_t* physics)
{
    if (physics != NULL)
    {
        oslo_dyn_array_free(physics->bodies);
        oslo_dyn_array_free(physics->forces);
        oslo_dyn_array_free(physics->torques);
    }
}

void oslo_physics_update(oslo_physics_t* physics)
{
    if (physics != NULL)
    {
        for (uint32_t i = 0; i < oslo_dyn_array_size(physics->bodies); ++i)
        {
            oslo_physics_body_t* body = &physics->bodies[i];
            
            // Apply gravity
            vec2 weight = vec2_ctor(0.0f, body->mass * G * PIXELS_PER_METER);
            oslo_physics_body_add_force(body, weight);

            // Apply forces
            for (uint32_t f = 0; f < oslo_dyn_array_size(physics->forces); ++f)
            {
                vec2* force = &physics->forces[f];
                oslo_physics_body_add_force(body, *force);
            }
        }


        for (uint32_t i = 0; i < oslo_dyn_array_size(physics->bodies); ++i)
        {
            oslo_physics_body_t* body = &physics->bodies[i];
            oslo_physics_body_integrate_forces(body);
        }

        // Check collisions

        for (uint32_t i = 0; i < oslo_dyn_array_size(physics->bodies); ++i)
        {
            oslo_physics_body_t* body = &physics->bodies[i];
            oslo_physics_body_integrate_velocities(body);
        }
    }
}

oslo_physics_body_t* oslo_physics_add_body(oslo_physics_t* physics, vec2 position, float mass)
{
    oslo_physics_body_t body = default_val();
    body.position = position;
    body.velocity = v2(0,0);
    body.acceleration = v2(0,0);
    body.sum_forces = v2(0,0);
    body.sum_torque = 0.0f;
    body.mass = mass;
    if (mass != 0.0f)
    {
        body.inv_mass = 1.0f/mass;
    }
    else
    {
        body.inv_mass = 0.0f;
    }

    oslo_dyn_array_push(physics->bodies, body);
    size_t size = oslo_dyn_array_size(physics->bodies);
    return &physics->bodies[size - 1];
}

void oslo_physics_remove_body(oslo_physics_t* physics, oslo_physics_body_t* body)
{
    // TODO
}

void oslo_physics_add_force(oslo_physics_t* physics, vec2 force)
{
    if (physics != NULL)
    {
        oslo_dyn_array_push(physics->forces, force);
    }
}

void oslo_physics_add_torque(oslo_physics_t* physics, float torque)
{
    if (physics != NULL)
    {
        oslo_dyn_array_push(physics->torques, torque);
    }
}

void oslo_physics_body_add_force(oslo_physics_body_t* body, vec2 force)
{
    body->sum_forces = vec2_add(body->sum_forces, force);
}

void oslo_physics_body_add_torque(oslo_physics_body_t* body, float torque)
{
    body->sum_torque += torque;
}

void oslo_physics_body_apply_impluse(oslo_physics_body_t* body, vec2 impulse)
{
    body->velocity = vec2_add(body->velocity, vec2_scale(impulse, body->inv_mass));
}

void oslo_physics_body_integrate_forces(oslo_physics_body_t* body)
{
    float dt = oslo_get_delta_time();

    body->acceleration = vec2_scale(body->sum_forces, body->inv_mass);
    body->velocity = vec2_add(body->velocity, vec2_scale(body->acceleration, dt));

    // Clear all the forces and torque acting on the object before the next physics step
    body->sum_forces = v2(0,0);
    body->sum_torque = 0.0f;
}

void oslo_physics_body_integrate_velocities(oslo_physics_body_t* body)
{
    float dt = oslo_get_delta_time();

    // Integrate the velocity to find the new position
    body->position = vec2_add(body->position, vec2_scale(body->velocity, dt));
}

#endif