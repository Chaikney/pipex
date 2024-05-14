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
    char	*c;
//    ft_printf("line is: %s", *envp);
    while (*envp != NULL)
    {
 //       ft_printf("line is: %s", *envp);
        if (ft_strncmp(*envp, "PATH=", 5) == 0)
            break;
        envp++;
    }
  //  ft_printf("line is: %s", *envp);
    c = *envp;
    while (*c != '=')
        c++;
    c++;
    *envp = c;
    ft_printf("the path i return is: %s", *envp);
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
// NOTE Use ft_substr or trim
// FIXED Memory leaks, need to free after strjoin
// FIXME Free one last part from ft_split. What?
// TODO Should this return a fully-qualified path to use?
// FIXME Invalid frees. This is a mess!
// - add a leading / to cmd
// - split the pieces of PATH so they can be checked
// - (this also requires the first 5 chars of the line to be removed.)
// - test the parts of path:
// -- does path + cmd = an executable?
// -- if YES we have our command: keep that and discard the rest.
char	*find_command(char *cmd, char **envp)
{
	char	**pathparts;
	char	*candidate;
    char	*slashed;

//    cmd = ft_strjoin("/", cmd);
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
//    free(candidate);
    return (candidate);
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
     char	*prog;

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
    free (prog);
    }
    return(0);
}
