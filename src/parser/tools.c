/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/19 01:27:17 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 19:37:45 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

char		*convert_backslash(char *str)
{
	unsigned int	i;

	i = 0;
	while(str[i])
	{
		if (str[i] == '\\')
			str[i] = '/';
		i++;
	}
	return (str);
}

char	**split_path(const char *path)
{
	int		i;
	int		j;
	int		pathlen;
	char	**final_path;

	pathlen = strlen(path);
	final_path = (char **)calloc(3, sizeof(char *));
	i = pathlen;
	j = 0;
	while (path[i] != '/' && i > 0)
		i--;
	final_path[0] = (char *)calloc(i + 1, sizeof(char));
	final_path[1] = (char *)calloc((pathlen - i + 1), sizeof(char));
	final_path[2] = NULL;
	strncpy(final_path[0], path, i);
	final_path[0] = ft_strjoinfreebool(final_path[0], "/", 1, 0);
	while (i < pathlen)
	{
		final_path[1][j] = path[i];
		j++;
		i++;
	}
	return (final_path);
}

unsigned long long hash(unsigned char *str)
{
    unsigned long long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return (hash);
}

int		ft_chartablen(char **s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}
