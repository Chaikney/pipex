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
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

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
// - add a leading / to cmd
// - split the pieces of PATH so they can be checked
// - (this also requires the first 5 chars of the line to be removed.)
// - test the parts of path:
// -- does path + cmd = an executable?
// -- if YES we have our command: keep that and discard the rest.
// FIXME Free one last part from ft_split. What? secondary pointer.
// TODO Should this return a fully-qualified path to use?
// FIXME Invalid frees. This is a mess!
// TODO Must split to get any parameters that may be part of the command
// FIXME Segfaults if it cannot find the command
char	*find_command(char *cmd, char **envp)
{
	char	**pathparts;
	char	*candidate;
    char	*slashed;

    while (*envp != NULL)
    {
        if (ft_strncmp(*envp, "PATH=", 5) == 0)
            break;
        envp++;
    }
    ft_printf("reading from: %s", *envp +5);
 	pathparts = ft_split(*envp + 5, ':');
	while (*pathparts != NULL)
	{
		slashed = ft_strjoin(*pathparts, "/");
		ft_printf("\ntesting %s", slashed);
        candidate = ft_strjoin(slashed, cmd);
        free (slashed);
        ft_printf("\t that equates to: %s", candidate);
		if (access(candidate, X_OK) == 0)
            break ;
        ft_printf("\tNo. freeing %s", candidate);
        free (candidate);
        ft_printf("\tThen freeing residue %s", *pathparts);
        free (*pathparts);
		pathparts++;
	}
    ft_printf("\nI choose: %s", candidate);
    while (*pathparts != NULL)
    {
        ft_printf("\tThen freeing %s\n", *pathparts);
        free (*pathparts);
        pathparts++;
    }
    free (pathparts);
//    free(candidate);
    return (candidate);
}

// Read arguments
// Check that they are valid:
// - file1 exists and is readable
// - cmd1 & 2 exist and are executable
// - file2 can be created. (open a fd to the path which is file2)
// - find the executable by making a path out if it:
// -- get system path vars
// -- add cmd var to path
// -- test it exists
// -- use it in execve
// DONE Split path and use it to locate cmd1 and cmd2
// TODO Call execve with located cmd
// TODO fork and dup a process
// TODO Stick two pieces together
int	main(int argc, char *argv[], char *envp[])
{
     char	*prog;
     pid_t	child;

    /* print_args(argv); */
    /* print_args(envp); */
    if (argc == 5)
    {
        printf("First file: %s\n", argv[1]);
        printf("run in: %s\n", argv[2]);
        printf("then Piped to: %s\n", argv[3]);
        printf("and the results saved in: %s\n", argv[4]);
        /* in = open(argv[1], O_RDONLY); */
        /* out = open(argv[4], O_CREAT); */
        // FIXME This malloc does not get freed.Because cmd1 is altered in find_command?
        /* cmd1 = ft_strjoin("/", argv[2]); */
        /* path = find_path(envp); */
        /* ft_printf("\nFound path: %s", path); */
        /* ft_printf("\nSeeking cmd..."); */
        prog = find_command(argv[2], envp);
        child = fork();
        if (child == -1)
            exit(EXIT_FAILURE);
        execve(prog, argv, envp);	// HACK I think the argv should be its own thing, have to make that?
        waitpid(child, NULL, 0);	// NOTE No need for compicated options, I guess.
    free (prog);
    }
    return(0);
}
