#include <cherry/game/game.h>
#include <cherry/memory.h>
#include <cherry/array.h>
#include <cherry/graphic/render.h>
#include <cherry/stdio.h>
#include <cherry/graphic/shader/shader_collection.h>
#include <cherry/graphic/buffer/buffer_collection.h>
#include <cherry/graphic/shader.h>
#include <cherry/math/math.h>
#include <cherry/graphic/texture.h>
#include <cherry/graphic/node/node_tree.h>
#include <cherry/graphic/node/branch.h>
#include <cherry/graphic/node/twig.h>

struct node_tree *nt1 = NULL;
struct node_tree *nt2 = NULL;

struct game *game_alloc()
{
        struct game *p  = smalloc(sizeof(struct game));
        p->renderers    = array_alloc(sizeof(struct renderer *), ORDERED);
        p->frame        = 0;

        struct renderer *r = renderer_alloc();
        r->pass = render_pass_main_alloc();
        renderer_set_color(r, NULL);
        array_push(p->renderers, &r);

        struct render_stage *stage = render_stage_alloc(r);

        struct shader *s = shader_texture_alloc();

        struct render_queue *queue = render_queue_alloc(&stage->content_queue_list, s);

        struct array *buffers[BUFFERS];
        i16 i;
        for_i(i, BUFFERS) {
                buffers[i] = array_alloc(sizeof(struct device_buffer *), ORDERED);
                struct device_buffer *quad      = buffer_quad_alloc();
                struct device_buffer *z         = buffer_z_alloc(900);
                struct device_buffer *transform = buffer_transform_alloc(900);
                struct device_buffer *color     = buffer_color_alloc(900);
                struct device_buffer *texcoord  = buffer_quad_texcoord_alloc();
                struct device_buffer *texroot   = buffer_quad_texroot_alloc(900);
                struct device_buffer *texrange  = buffer_quad_texrange_alloc(900);
                struct device_buffer *texid     = buffer_texid_alloc(900);
                array_push(buffers[i], &quad);
                array_push(buffers[i], &z);
                array_push(buffers[i], &transform);
                array_push(buffers[i], &color);
                array_push(buffers[i], &texcoord);
                array_push(buffers[i], &texroot);
                array_push(buffers[i], &texrange);
                array_push(buffers[i], &texid);
        }

        struct render_content *content = render_content_alloc(queue, buffers, 6, 900);

        for_i(i, BUFFERS) {
                array_free(buffers[i]);
        }
        struct shader_uniform *project_uniform  = shader_uniform_alloc();
        struct shader_uniform *view_uniform     = shader_uniform_alloc();
        shader_set_uniform(s, SHADER_COLOR_PROJECT, project_uniform);
        shader_set_uniform(s, SHADER_COLOR_VIEW, view_uniform);

        shader_uniform_update(project_uniform, mat4_identity.m, sizeof(mat4_identity));
        shader_uniform_update(view_uniform, mat4_identity.m, sizeof(mat4_identity));

        {
                nt1 = node_tree_alloc(node_alloc(content));
                node_tree_set_branch_z(nt1, branch_z_alloc(1));
                node_tree_set_branch_transform(nt1, branch_transform_alloc(2));
                node_tree_set_branch_color(nt1, branch_color_alloc(3));
                node_tree_set_twig_texroot(nt1, twig_texroot_alloc(5));
                node_tree_set_twig_texrange(nt1, twig_texrange_alloc(6));
                node_tree_set_twig_texid(nt1, twig_texid_alloc(7));
        }
        {
                nt2 = node_tree_alloc(node_alloc(content));
                node_tree_set_branch_z(nt2, branch_z_alloc(1));
                node_tree_set_branch_transform(nt2, branch_transform_alloc(2));
                node_tree_set_branch_color(nt2, branch_color_alloc(3));
                node_tree_set_twig_texroot(nt2, twig_texroot_alloc(5));
                node_tree_set_twig_texrange(nt2, twig_texrange_alloc(6));
                node_tree_set_twig_texid(nt2, twig_texid_alloc(7));
        }
        node_tree_add_node_tree(nt1, nt2);

        node_tree_set_texid(nt1, 1);
        node_tree_set_texid(nt2, 0);

        node_tree_set_scale(nt2, vec3((float[3]){0.5, 0.5, 0.5}));

        /* recalculate transform */
        union mat4 m = mat4_identity;
        branch_transform_traverse(node_tree_get_branch_transform(nt1), m);

        /* recalculate color tree */
        union vec4 v = vec4((float[4]){1, 1, 1, 1});
        branch_color_traverse(node_tree_get_branch_color(nt1), v);

        /* recalculate z order  */
        float z = 0;
        branch_z_traverse(node_tree_get_branch_z(nt1), &z);

        render_content_set_texture(content, 1, texture_alloc_file("res/images/wolf.jpg"));
        render_content_set_texture(content, 0, texture_alloc_file("res/images/test.png"));

        return p;
}

float angle = 0;
union vec3 pos = {0, 0, 0};

void game_update(struct game *p)
{
        if(nt2) {
                angle += 1;
                union vec4 quat = quat_angle_axis(DEG_TO_RAD(angle), (float[3]){0, 0, 1});
                node_tree_set_rotation(nt2, quat);
                pos.x += 0.001;
                node_tree_set_position(nt1, pos);
        }
        union mat4 m = mat4_identity;
        branch_transform_traverse(node_tree_get_branch_transform(nt1), m);
}

void game_render(struct game *p)
{
        /* process all renderers */
        struct renderer **r;
        array_for_each(r, p->renderers) {
                renderer_render(*r, p->frame);
        }
        /* increase frame by 1 */
        p->frame++;
        p->frame %= BUFFERS;
}

void game_free(struct game *p)
{
        if(nt1 && nt2) {
                node_tree_free(nt1);
                node_tree_free(nt2);
        }
        array_deep_free(p->renderers, struct renderer *, renderer_free);
        sfree(p);
}
