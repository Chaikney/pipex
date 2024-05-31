/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:30:35 by chaikney          #+#    #+#             */
/*   Updated: 2024/05/10 14:30:37 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <stdio.h>	// perror
# include <fcntl.h>	// open
# include <unistd.h>	// access, pipe
# include "libft/libft.h"
# include <sys/wait.h>	// waitpid

char	**get_path(char **envp);
char	*find_command(char *cmd, char **envp);
#endif
