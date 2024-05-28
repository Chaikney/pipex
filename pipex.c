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
// DONE YES I need to free things that are ft_strjoined(slashed and candidate)
char	*find_command(char *cmd, char **envp)
{
	char	**pathparts;
	char	*candidate;
	char	*slashed;
	char	*goodpath;
	int		i;

	goodpath = NULL;
	while (*envp != NULL)
	{
		if (ft_strncmp(*envp, "PATH=", 5) == 0)
			break ;
		envp++;
	}
	pathparts = ft_split(*envp + 5, ':');
	while ((*pathparts != NULL) && (!goodpath))
	{
		slashed = ft_strjoin(*pathparts, "/");
		candidate = ft_strjoin(slashed, cmd);
		if (access(candidate, X_OK) == 0)
			goodpath = ft_strdup(candidate);
		free (candidate);
		free(slashed);
		pathparts++;
	}
	i = -1;
	while (pathparts[++i] != NULL)
		free(pathparts[i]);
//	free(pathparts[i]);	// HACK Does this catch a final null-termination of the split?
	/* if (pathparts) */
	/* 	free (pathparts); */
	return (goodpath);
}

// If we fail to find the command, clear the pathparts before exit.
// FIXME args is a *bad* variable name - I am using it in 2 different ways!
// ...1 for the path split parts, one for CLI args - not needed!
// DONE Close open fds
// TODO Free memory -- which?!
// FIXED if args is NULL, we get a segfault (bad for no input file cases and child process fail)
void	exit_and_free(char **args, int fd_in, int fd_out)
{
	int	i;

	if ((fd_in) && (fd_in != -1))
		close(fd_in);
	if ((fd_out) && (fd_out != -1))
		close(fd_out);
	perror("file descriptors closed");
	i = 0;
	if (args)
	{
		while (args[i])
		{
			free(args[i]);
			i++;
		}
		perror("path parts freed");
	}
//	free(*args);
	exit(EXIT_FAILURE);
}


// Wrap the things that you need to do to make the
// command run in whatever process.
// - split any arguments from cmd
// -- We get an array with the cmd as first thing
// -- (same as when argv is read in a program)
// - find cmd in PATH
// - send it off to execve
// NOTE This on its own has no idea of what input / output it should use.
void	run_command(char *cmd, char **envp)
{
	char	*prog;
	char	**args;
	int		i;

	args = ft_split(cmd, ' ');
	i = 0;
	prog = find_command(args[0], envp);
	printf("\nAbout to launch: %s", prog);
	if (!prog)
	{
		perror("prog not found");
		exit_and_free(args, -1, -1);
	}
	// FIXME Syscall param execve(filename) points to unaddressable byte(s)
	if (execve(prog, args, envp) == -1)
	{
		perror("Failed to execute prog");
		exit_and_free(args, -1, -1);
	}
	if (args)
		while (args[i])
			free(args[i++]);
	free (prog);
}

// Make a child process to execute the commnand:
// - fork
// - run command
// - wait for it to come back
// NOTE child == 0 means we are in the child process!
// TODO Should pipe and fork failures go through exit_and_free?
void	make_child(char *cmd, char **envp)
{
	pid_t	child;
	int		tube[2];
	int		status;

	status = 0;
	if (pipe(tube) == -1)
		exit(EXIT_FAILURE);
	child = fork();
	if (child == -1)
		exit(EXIT_FAILURE);
	if (child == 0)
	{
		close(tube[0]);
		dup2(tube[1], STDOUT_FILENO);
		run_command(cmd, envp);
		close(tube[1]);
	}
	else
	{
		close(tube[1]);
		dup2(tube[0], STDIN_FILENO);
		waitpid(child, &status, 0);
		// NOTE WIFEXITSTATUS(status)==1 means the child failed.
		if (WIFEXITED(status) && WEXITSTATUS(status) == 1)
		{
			printf("Child process exited with status: %d\n", WEXITSTATUS(status));
			perror("child process failed");
			exit_and_free(NULL, tube[0], tube[1]);
		}
		close(tube[0]);
	}
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
// ...what needs to be freed? / passed to thing?
// No malloc here but in the split.
// TODO Make a better failure / exit routine that closes files, etc.
int	main(int argc, char *argv[], char *envp[])
{
	int	in_file;
	int	out_file;
	int	i;

	if (argc >= 5)
	{
		in_file = open(argv[1], O_RDONLY, 0777);
		out_file = open(argv[(argc - 1)], O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if ((in_file == -1) || (out_file == -1))
			exit_and_free(NULL, in_file, out_file);
		dup2(in_file, STDIN_FILENO);
		close(in_file);
		i = 2;
		while (i < (argc - 2))
		{
			make_child(argv[i], envp);
			i++;
		}
		dup2(out_file, STDOUT_FILENO);
		close(out_file);
		run_command(argv[(argc - 2)], envp);
		close(in_file);
	}
	else
		ft_printf("Parameters:\nInput and output files, commands inbetween");
	return (0);
}
