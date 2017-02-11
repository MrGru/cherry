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

struct node *test_node;
struct node_tree *nt1;
struct node_tree *nt2;

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
                z->item_size            = sizeof(float);
                transform->item_size    = sizeof(union mat4);
                color->item_size        = sizeof(union vec4);
                texid->item_size        = sizeof(float);
                texroot->item_size      = sizeof(union vec2);
                texrange->item_size     = sizeof(union vec2);
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

        struct node *n1, *n2;
        {
                struct node *n = node_alloc(content);
                float z = 0;
                node_set_data(n, 1, &z, sizeof(z));
                node_set_data(n, 2, mat4_identity.m, sizeof(mat4_identity));
                union vec4 color = vec4((float[4]){1, 1, 1, 1});
                node_set_data(n, 3, color.v, sizeof(color));

                union vec2 root = vec2((float[2]){0.5, 0.5});
                node_set_data(n, 5, root.v, sizeof(root));

                union vec2 range = vec2((float[2]){0.5, 0.5});
                node_set_data(n, 6, range.v, sizeof(range));

                float tid = 1.001f;
                node_set_data(n, 7, &tid, sizeof(tid));
                n1 = n;
        }
        {
                struct node *n = node_alloc(content);
                float z = 0;
                node_set_data(n, 1, &z, sizeof(z));
                node_set_data(n, 2, mat4_identity.m, sizeof(mat4_identity));
                union vec4 color = vec4((float[4]){1, 1, 1, 1});
                node_set_data(n, 3, color.v, sizeof(color));

                union vec2 root = vec2((float[2]){0, 0});
                node_set_data(n, 5, root.v, sizeof(root));

                union vec2 range = vec2((float[2]){1, 1});
                node_set_data(n, 6, range.v, sizeof(range));

                float tid = 0.001f;
                node_set_data(n, 7, &tid, sizeof(tid));
                test_node = n;
                n2 = n;
        }
        nt1 = node_tree_alloc();
        nt2 = node_tree_alloc();
        node_tree_set_node(nt1, n1);
        node_tree_set_node(nt2, n2);
        struct branch_z *bz1 = branch_z_alloc(1);
        struct branch_z *bz2 = branch_z_alloc(1);
        branch_z_add(bz1, bz2);
        node_tree_set_branch_z(nt1, bz1);
        node_tree_set_branch_z(nt2, bz2);
        float z = 0;
        branch_z_traverse(bz1, &z);

        {
                struct texture *t = texture_alloc_file("res/images/wolf.jpg");
                render_content_set_texture(content, 1, t);
        }
        {
                struct texture *t = texture_alloc_file("res/images/test.png");
                render_content_set_texture(content, 0, t);
        }

        return p;
}

float angle = 0;

void game_update(struct game *p)
{
        angle += 1;
        union mat4 m = mat4_new_z_rotation(DEG_TO_RAD(angle));
        m = mat4_scale(m, vec3((float[3]){0.5, 0.5, 0.5}));
        node_set_data(test_node, 2, m.m, sizeof(m));
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
        node_tree_free(nt1);
        node_tree_free(nt2);
        array_deep_free(p->renderers, struct renderer *, renderer_free);
        sfree(p);
}
