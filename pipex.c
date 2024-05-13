/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                             +:+ +:+         +:+     */
/*   By: chaikney <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 14:30:23 by chaikney          #+#    #+#             */
/*   Updated: 2024/05/10 14:30:25 by chaikney         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft/libft.h"
#include <unistd.h>

// Given the environment variables, return the line containing the PATH
// FIXED Segfaults as there is no protection for a blank PATH in strncmp
// (Still can happen but there will (must?!) always be something in envp)
char	*find_path(char **envp)
{
//    ft_printf("line is: %s", *envp);
    while (*envp != NULL)
    {
 //       ft_printf("line is: %s", *envp);
        if (ft_strncmp(*envp, "PATH=", 5) == 0)
            break;
        envp++;
    }
  //  ft_printf("line is: %s", *envp);
    return (*envp);
}

// print the lines given, like in an argv array.
// TODO Remove before submission, for debugging only.
void	print_args(char **arg)
{
    while (*arg != NULL)
    {
        printf("%s\n", *arg);
        arg++;
    }
}

// Go through PATH entries and see if cmd is locatable.
// DONE Need a / between cmd and pathparts
// (I think the memopry approach here is a bit suspect..)
// TODO Discard the first five chars of path (i.e. PATH=)
// FIXED Memory leaks, need to free after strjoin
// FIXME Free one last part from ft_split.
// TODO Should this return a fully-qualified path to use?
int	find_command(char *path, char *cmd)
{
	char	**pathparts;
	char	*candidate;

//    cmd = ft_strjoin("/", cmd);
	pathparts = ft_split(path, ':');
	while (*pathparts != NULL)
	{
		candidate = ft_strjoin(*pathparts, cmd);
		ft_printf("\n%s", candidate);
		if (access(candidate, X_OK) == 0)
            break ;
        free (candidate);
        free (*pathparts);
		pathparts++;
	}
    while (*pathparts != NULL)
    {
        free (*pathparts);
        pathparts++;
    }
//    free (pathparts);
    ft_printf("\nI choose: %s", candidate);
    free(candidate);
	return (0);
}

// Read arguments
// Check that they are valid:
// - file1 exists and is readable
// - cmd1 & 2 exist and are executable
// - file2 can be created. (open a fd to the path which is file2)
// Run cmd1 with file1 - how?? Why would cmd1 look to stdin,
// or what is the standard way to send it info? execve?
// - find the executable by making a path out if it:
// -- get system path vars
// -- add cmd var to path
// -- test it exists
// -- use it in execve
// TODO Split path and use it to locate cmd1 and cmd2
int	main(int argc, char *argv[], char *envp[])
{
    int	in;
    int	out;
    char	*path;
    char	*cmd1;
//     char	*prog;

    /* print_args(argv); */
    /* print_args(envp); */
    if (argc == 5)
    {
        printf("First file: %s\n", argv[1]);
        printf("run in: %s\n", argv[2]);
        printf("then Piped to: %s\n", argv[3]);
        printf("and the results saved in: %s\n", argv[4]);
        in = open(argv[1], O_RDONLY);
        out = open(argv[4], O_CREAT);
        cmd1 = ft_strjoin("/", argv[2]);
        path = find_path(envp);
        ft_printf("\nFound path: %s", path);
        ft_printf("\nSeeking cmd...");
        find_command(path, cmd1);
        (void) in;
        (void) out;
    }
    free (cmd1);
    return(0);
}
