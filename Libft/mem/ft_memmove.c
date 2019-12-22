/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ft_memmove.c                                       :+:    :+:            */
/*                                                     +:+                    */
/*   By: pacovali <marvin@codam.nl>                   +#+                     */
/*                                                   +#+                      */
/*   Created: 2019/01/11 10:29:33 by pacovali      #+#    #+#                 */
/*   Updated: 2019/01/19 11:55:31 by pacovali      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t n)
{
	size_t			i;
	unsigned char	*tmp_dst;
	unsigned char	*tmp_src;

	if (dst == NULL && src == NULL)
		return (0);
	i = 0;
	tmp_dst = (unsigned char*)dst;
	tmp_src = (unsigned char*)src;
	if (&tmp_dst[0] < &tmp_src[0])
		while (i < n)
		{
			tmp_dst[i] = tmp_src[i];
			i++;
		}
	else
		while (i < n)
		{
			n--;
			tmp_dst[n] = tmp_src[n];
		}
	return (dst);
}
