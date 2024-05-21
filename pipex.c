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

// FIXME Too many functions in this file.

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

// Locate PATH and go through entries for cmd is locatable.
// cmd must be the name only, not any of its arguments.
// - Find the PATH= line in env (this could be split out)
// - split the pieces of PATH and add a trailing slash.
// - (this also requires the first 5 chars of the line to be removed.)
// - test the parts of path:
// -- does path + cmd = an executable?
// -- if YES we have our command: keep that and discard the rest.
// FIXME Free one last part from ft_split. What? secondary pointer.
// TODO This should return a fully-qualified path for execve to use.
// FIXME Invalid frees. This is a mess!
// FIXME Segfaults if it cannot find the command
// FIXME Function is too long (prob OK without printfs)
char	*find_command(char *cmd, char **envp)
{
	char	**pathparts;
	char	*candidate;
	char	*slashed;

	while (*envp != NULL)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			break ;
		envp++;
	}
//    ft_printf("reading from: %s", *envp +5);
	pathparts = ft_split(*envp + 5, ':');
	while (*pathparts != NULL)
	{
		slashed = ft_strjoin(*pathparts, "/");
//		ft_printf("\ntesting %s", slashed);
		candidate = ft_strjoin(slashed, cmd);
//        free (slashed);
//        ft_printf("\t that equates to: %s", candidate);
		if (access(candidate, X_OK) == 0)
			break ;
//        ft_printf("\tNo. freeing %s", candidate);
//       free (candidate);
//        ft_printf("\tThen freeing residue %s", *pathparts);
//        free (*pathparts);
		pathparts++;
	}
//    ft_printf("\nI choose: %s", candidate);
	while (*pathparts != NULL)
	{
//        ft_printf("\tThen freeing %s\n", *pathparts);
//       free (*pathparts);
		pathparts++;
	}
//    free (pathparts);
//    free(candidate);
	return (candidate);
}

// If we fail to find the command, clear the pathparts before exit.
// FIXME This doesn't catch anything - does it not get called?
void	exit_and_free(char **args)
{
	int	i;

	ft_printf("Failed to find command; clearing up.");
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

// Wrap the things that you need to do to make the
// command run in whatever process.
// - split any arguments from cmd
// -- We get an array with the cmd as first thing
// -- (same as when argv is read in a program)
// - find cmd in PATH
// - send it off to execve
// NOTE This on its own has no idea of what input / output it should use.
// FIXME If this receives a non-command, it should free mem before exiting.
// FIXME Not sure if the !prog gets triggered
void	run_command(char *cmd, char **envp)
{
	char	*prog;
	char	**args;

	args = ft_split(cmd, ' ');
	print_args(args);
	prog = find_command(args[0], envp);
	ft_printf("Looked for: %s", prog);
	if (!prog)
	{
		ft_printf("Could not find prog: %s", prog);
		exit_and_free(args);
		free(prog);
		exit(EXIT_FAILURE);
	}
	if (execve(prog, args, envp) == -1)
	{
		ft_printf("Failed to execute prog: %s", prog);
		exit_and_free(args);
		free(prog);
		exit(EXIT_FAILURE);
	}
	free (prog);
}

// The child process has to run cmd1 using file1 and our pipe
// We read our input from in_file (i.e. file1)
// We *write* our output (STDOUT) to the pipe[1].
// We do not need to read from the pipe, so we close it.
void	i_am_the_child(char **argv, char **envp, int *tube, int in_file)
{
	close(tube[0]);
	dup2(in_file, STDIN_FILENO);
	dup2(tube[1], STDOUT_FILENO);
	close(tube[1]);
	run_command(argv[2], envp);
}

// The parent process runs cmd2 (argv[3])
// using for its input (STDIN) the output of child process (mario[1])
// Does not need to write to the pipe, so close it immediately.
// We read our input from the read end of the pipe[0]
// We send our output to out_file (file2).
void	i_am_the_parent(char **argv, char **envp, int *tube, int out_file)
{
	close(tube[1]);
	dup2(tube[0], STDIN_FILENO);
	dup2(out_file, STDOUT_FILENO);
	close(tube[0]);
	run_command(argv[3], envp);
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
// - make a pipe to put between processes
// - Create a child process
// - run cmd1, wait for it to return
// - run cmd2
// TODO Should have some sensible limits on the file names-
// ....What could cause bother?
// FIXME There are memleaks if the 1st command is bad
// ...what needs to be freed? / passed to thing? No malloc here but in the split.
int	main(int argc, char *argv[], char *envp[])
{
	int	mario[2];
	int	in_file;
	int	out_file;
	pid_t	child;

	if (argc >= 5)
	{
		in_file = open(argv[1], O_RDONLY);
		if ((pipe(mario) == -1) || (in_file == -1))
			exit(EXIT_FAILURE);
		child = fork();
		if (child == -1)
			exit(EXIT_FAILURE);
		if (child == 0)
			i_am_the_child(argv, envp, mario, in_file);
		waitpid(child, NULL, 0);
		out_file = open(argv[argc-1], O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (out_file == -1)
			exit(EXIT_FAILURE);
		i_am_the_parent(argv, envp, mario, out_file);
		close(out_file);
	}
	return (0);
}
