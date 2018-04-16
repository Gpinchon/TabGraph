/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blur.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 16:34:46 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 18:29:14 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

static void	blur_pass(int blur, int *color0, int *texture, float radius)
{
	VEC2			direction;
	int				temp;
	static float	angle = 0;

	direction = mat2_mult_vec2(mat2_rotation(angle), new_vec2(1, 1));
	direction = vec2_scale(direction, radius);
	shader_bind_texture(framebuffer_get_shader(blur),
		shader_get_uniform_index(framebuffer_get_shader(blur),
			"in_Texture_Color"), *texture, GL_TEXTURE0);
	shader_set_vec2(framebuffer_get_shader(blur),
		shader_get_uniform_index(framebuffer_get_shader(blur), "in_Direction"),
		direction);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		texture_get_glid(*color0), 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	angle = CYCLE(angle + 0.785398, 0, 2.356194);
	temp = *texture;
	*texture = *color0;
	*color0 = temp;
}

static int	generate_blur_fb(void)
{
	int	fb;

	fb = framebuffer_create(vec2_scale(window_get_size(),
	engine_get()->internal_quality), shader_get_by_name("blur"), 0, 0);
	framebuffer_create_attachement(fb, GL_RGB, GL_RGB16F_ARB);
	framebuffer_setup_attachements(fb);
	return (fb);
}

inline void	texture_blur(int texture, int pass, float radius)
{
	static int	blur = -1;
	int			color0;

	if (blur == -1)
		blur = generate_blur_fb();
	framebuffer_resize(blur, texture_get(texture)->size);
	framebuffer_bind(blur);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	shader_use(framebuffer_get_shader(blur));
	glBindVertexArray(display_quad_get());
	pass *= 4;
	color0 = framebuffer_get_attachement(blur, 0);
	while (pass)
	{
		blur_pass(blur, &color0, &texture, radius);
		pass--;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			texture_get_glid(framebuffer_get_attachement(blur, 0)), 0);
	shader_use(-1);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
