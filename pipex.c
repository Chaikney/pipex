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

// print the lines given, like in an argv array.
void	print_args(char **arg)
{
    while (*arg != NULL)
    {
        printf("%s\n", *arg);
        arg++;
    }
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
// TODO Write a function to print the envp (could also do argvs)
int	main(int argc, char *argv[], char *envp[])
{
    int	in;
    int	out;
//     char	*prog;

    print_args(argv);
    print_args(envp);
    if (argc == 5)
    {
        printf("First file: %s\n", argv[1]);
        printf("run in: %s\n", argv[2]);
        printf("then Piped to: %s\n", argv[3]);
        printf("and the results saved in: %s\n", argv[4]);
        in = open(argv[1], O_RDONLY);
        out = open(argv[4], O_CREAT);
        if (access(argv[2], X_OK) == 0)
            printf("I could run this");
        if (access(argv[3], X_OK) == 0)
            printf("I could run this");
        execve(argv[2], &argv[1], NULL);
        (void) in;
        (void) out;
    }
    return(0);
}
