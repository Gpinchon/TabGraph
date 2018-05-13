/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blur.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 16:34:46 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 01:04:06 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Window.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"

static Framebuffer	&generate_blur_fb(void)
{
	auto	blur = Framebuffer::create("blur", vec2_scale(Window::size(),
	Engine::internal_quality()), *Shader::get_by_name("blur"), 0, 0);
	blur->create_attachement(GL_RGB, GL_RGB16F_ARB);
	blur->setup_attachements();
	return (*blur);
}

void	Texture::blur(const int &pass, const float &radius)
{
	static Framebuffer	*blur = nullptr;
	Texture				*color0;

	if (!blur)
		blur = &generate_blur_fb();
	blur->resize(size());
	blur->bind();
	glDisable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	blur->shader().use();
	glBindVertexArray(display_quad_get());
	auto totalPass = pass * 4;
	Texture *texture = this;
	color0 = &blur->attachement(0);
	while (totalPass >= 0)
	{
		VEC2			direction;
		Texture			*temp;
		static float	angle = 0;

		direction = mat2_mult_vec2(mat2_rotation(angle), new_vec2(1, 1));
		direction = vec2_scale(direction, radius);
		blur->shader().bind_texture("in_Texture_Color", texture, GL_TEXTURE0);
		blur->shader().set_uniform("in_Direction", direction);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color0->glid(), 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		angle = CYCLE(angle + 0.785398, 0, 2.356194);
		temp = texture;
		texture = color0;
		color0 = temp;
		totalPass--;
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			blur->attachement(0).glid(), 0);
	blur->shader().use(false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
