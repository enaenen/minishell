#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t	ft_strlen(const char *str)
{
	size_t i;
	i = 0;
	while (str[i])
		i++;
	return (i);
}

char	*ft_substr(const char *str, size_t start, size_t len)
{
	size_t	i;
	char	*ret;

	ret = malloc(len + 1 * sizeof(char));
	i = 0;
	while (i < len)
	{
		ret[i] = str[i + start];
		i++;
	}
	ret[i] = '\0';
	return (ret);
}

static void	*ft_memchr(const void *s, int c, size_t n)
{
	char	*cp;

	cp = (char *)s;
	while (n--)
	{
		if (*cp == (char) c)
			return ((void *) cp);
		cp++;
	}
	return (NULL);
}

char	*ft_strtrim(char const *str, char const *set)
{
	char		*ret;
	char const	*start;
	char const	*end;
	size_t		str_len;
	size_t		set_len;

	str_len = ft_strlen(str);
	set_len = ft_strlen(set);
	start = str;
	end = str + str_len -1;
	while (start < end && ft_memchr(set, *start, set_len))
		start++;
	while (start <= end && ft_memchr(set, *end, set_len))
		end--;
	printf("start = %ld, end = %ld\n", start - str, end- start + 1);
	ret = ft_substr(str, start - str, end - start + 1);
	return (ret);
}


char	*ft_strchr(const char *s, int c)
{
	char	cc;
	int		i;
	size_t	len;

	cc = (char)c;
	len = ft_strlen(s);
	if (cc == '\0')
		return ((char *)&s[len]);
	i = 0;
	while (s[i])
	{
		if (s[i] == cc)
			return ((char *)&s[i]);
		i++;
	}
	return (NULL);
}


int main(void)
{
	char *before = "            x ls -l  ";
	char *after;
	after = ft_strtrim(before, " ");
	printf("before = %s\n", before);
	printf("after = %s\n", after);
}