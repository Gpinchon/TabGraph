/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/22 01:23:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/22 01:30:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>

struct hdr_parser
{
	char	intro[10];
};

int	load_hdr(char *path)
{
	hdr_parser	parser;

	FILE * stream = fopen(path, "rb");
	fread(&parser.intro, sizeof(char), 10, stream);
	if (strncmp(parser.intro, "#?RADIANCE", 10))
		return (-1);
}